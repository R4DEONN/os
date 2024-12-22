#include <chrono>
#include <iostream>
#include <vector>
#include <thread>
#include <unordered_set>
#include <mutex>
#include <cmath>

template<typename T, typename H = std::hash<T>, typename Comp = std::equal_to<T>>
class ThreadsafeSet
{
public:
	void insert(const T& value)
	{
		std::lock_guard<std::mutex> lock(m_mutex); //TODO: Изучить остальные примитивы синхронизации, заиспользовать их для оптимальной работы с множеством
		m_set.insert(value);
	}

	bool contains(const T& value) const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_set.find(value) != m_set.end();
	}

	size_t size() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_set.size();
	}

	std::vector<T> GetElements() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return std::vector<T>(m_set.begin(), m_set.end()); //TODO: Почему бы не сделать константные методы begin и end у Threadsafe
	}

private:
	std::unordered_set<T, H, Comp> m_set;
	mutable std::mutex m_mutex;
};

bool IsPrime(uint64_t number)
{
	if (number < 2) return false;
	for (uint64_t i = 2; i * i <= number; ++i)
	{
		if (number % i == 0) return false;
	}
	return true;
}

void FindPrimesInRange(uint64_t start, uint64_t end, ThreadsafeSet<uint64_t>& primes)
{
	for (uint64_t i = start; i <= end; ++i) //TODO: что будет когда end будет равен максимальному uint64_t
	{
		if (IsPrime(i))
		{
			primes.insert(i);
		}
	}
}

std::vector<std::pair<uint64_t, uint64_t>> DistributeWork(uint64_t upper_limit, int thread_count)
{
	std::vector<std::pair<uint64_t, uint64_t>> ranges;
	uint64_t total_work = 0;

	//TODO: Формула суммы квадратных корней
	for (uint64_t i = 2; i <= upper_limit; ++i)
	{ //TODO: n / log n Формула чтобы в диапазоне целых чисел узнать простые
		total_work += static_cast<uint64_t>(std::sqrt(i));
	}
	//TODO: перенести в таблицу, заиспользовать закон Амдала, чтобы определить какая часть работы не распараллеливается
	uint64_t workPerThread = total_work / thread_count;
	uint64_t start = 2;
	uint64_t accumulated_work = 0;

	for (int i = 0; i < thread_count; ++i)
	{
		uint64_t end = start;

		while (end <= upper_limit && accumulated_work + static_cast<uint64_t>(std::sqrt(end)) < workPerThread)
		{
			accumulated_work += static_cast<uint64_t>(std::sqrt(end));
			++end;
		}

		ranges.emplace_back(start, end - 1);
		start = end;
		accumulated_work = 0;
	}

	// Последний диапазон доходит до upper_limit
	if (ranges.back().second < upper_limit)
	{
		ranges.back().second = upper_limit;
	}

	return ranges;
}

void ParallelPrimeSearch(uint64_t upper_limit, int thread_count)
{
	ThreadsafeSet<uint64_t> primes;
	std::vector<std::thread> threads;

	auto ranges = DistributeWork(upper_limit, thread_count);

	for (const auto& range: ranges)
	{
		threads.emplace_back(FindPrimesInRange, range.first, range.second, std::ref(primes));
	}

	for (auto& thread: threads)
	{
		thread.join();
	}

	auto elements = primes.GetElements();
	std::cout << "Found primes (sample): ";
	for (size_t i = 0; i < std::min(elements.size(), size_t(10)); ++i)
	{
		std::cout << elements[i] << " ";
	}
	std::cout << std::endl;
}

void MeasureExecutionTime(uint64_t upper_limit, int thread_count)
{
	auto start_time = std::chrono::high_resolution_clock::now();
	ParallelPrimeSearch(upper_limit, thread_count);
	auto end_time = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double> elapsed = end_time - start_time;
	std::cout << "Threads: " << thread_count << ", Time: " << elapsed.count() << " seconds\n";
}

int main()
{
	const uint64_t limits[] = { 1000000, 10000000, 100000000 };
	const int max_threads = 16;

	for (uint64_t limit: limits)
	{
		std::cout << "Upper limit: " << limit << "\n";
		for (int threads = 1; threads <= max_threads; ++threads)
		{
			MeasureExecutionTime(limit, threads);
		}
		std::cout << "------------------------\n";
	}

	return 0;
}