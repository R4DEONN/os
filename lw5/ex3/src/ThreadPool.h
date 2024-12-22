#pragma once

#include <functional>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

class ThreadPool
{
public:
	using Task = std::function<void()>;

	explicit ThreadPool(unsigned numThreads)
		: m_stop(false)
	{
		//TODO: Перевести на jthread
		for (unsigned i = 0; i < numThreads; ++i)
		{
			m_workers.emplace_back([this]
			{ WorkerLoop(); });
		}
	}

	~ThreadPool()
	{
		Stop();
	}

	void Dispatch(Task task)
	{
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			if (m_stop) return;
			m_tasks.push(std::move(task));
		}
		m_taskAvailableCv.notify_one();
	}

	void Wait()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_allTasksFinishedCv.wait(lock, [this]()
		{
			return m_tasks.empty() && m_activeTasks == 0;
		});
	}

	void Stop()
	{
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_stop = true;
		}
		m_taskAvailableCv.notify_all();
		m_workers.clear();
	}

private:
	void WorkerLoop()
	{
		while (true)
		{
			Task task;
			{
				std::unique_lock<std::mutex> lock(m_mutex);
				//TODO: Не будет ли тут зависания если был один поток и еще две задачи в очереди. Написать тест для этой ситуации
				m_taskAvailableCv.wait(lock, [this]()
				{
					return m_stop || !m_tasks.empty();
				});

				if (m_stop && m_tasks.empty()) return;

				task = std::move(m_tasks.front());
				m_tasks.pop();
				++m_activeTasks;
			}

			try
			{
				task();
			}
			catch (...)
			{}

			{
				std::lock_guard<std::mutex> lock(m_mutex);
				--m_activeTasks;
				if (m_tasks.empty() && m_activeTasks == 0)
				{
					m_allTasksFinishedCv.notify_all();
				}
			}
		}
	}

	std::vector<std::jthread> m_workers;
	std::queue<Task> m_tasks;
	std::mutex m_mutex;
	std::condition_variable m_taskAvailableCv;
	std::condition_variable m_allTasksFinishedCv;
	//TODO: Нужен ли здесь atomic , если он всегда беретсч под локом
	unsigned m_activeTasks{ 0 };
	bool m_stop;
};
