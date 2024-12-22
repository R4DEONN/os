 #pragma once

#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <iostream>
#include <utility>

class BgThreadDispatcher
{
public:
	using Task = std::function<void()>;

	BgThreadDispatcher()
	{
		m_worker = std::jthread([this]
		{ WorkerLoop(); });
	}

	~BgThreadDispatcher()
	{
		Stop();
	}

	void Dispatch(Task task)
	{
		if (m_stop) return;

		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_tasks.push(std::move(task));
		}

		m_taskAvailableCv.notify_one();
	}

	void Wait()
	{
		if (m_stop) return;
		std::unique_lock<std::mutex> lock(m_mutex);
		m_allTasksFinishedCv.wait(lock, [this]()
		{
			return m_tasks.empty() && !m_runningTask;
		});
	}

	void Stop()
	{
		m_stop = true;
		m_taskAvailableCv.notify_one();
		if (m_worker.joinable())
		{
			m_worker.join();
		}
	}

private:
	void WorkerLoop() //TODO: stop_token видимо не нужен
	{
		while (true)
		{
			Task task;

			{
				std::unique_lock<std::mutex> lock(m_mutex);
				m_taskAvailableCv.wait(lock, [this]()
				{
					return m_stop || !m_tasks.empty();
				});

				if (m_stop && m_tasks.empty())
					break;

				task = std::move(m_tasks.front());
				m_tasks.pop();
				m_runningTask = true;
			}

			try
			{
				task();
			}
			catch (...)
			{}

			{
				std::lock_guard<std::mutex> lock(m_mutex);
				m_runningTask = false;
			}
			m_allTasksFinishedCv.notify_one(); //TODO: разобраться в разнице между
		}
	}

	std::queue<Task> m_tasks;
	std::mutex m_mutex;
	//TODO: Дать более осмысленное имя для cond vars
	std::condition_variable m_taskAvailableCv;
	std::condition_variable m_allTasksFinishedCv;
	std::jthread m_worker;
	std::atomic<bool> m_stop = false;
	bool m_runningTask = false;
};