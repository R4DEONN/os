#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <thread>
#include <chrono>
#include <string>
#include <algorithm>
#include <cassert>
#include <iterator>

void PrintHelp()
{
	std::cout << "Usage:\n";
	std::cout << "  app generate OUTPUT_FILE SIZE\n";
	std::cout << "  app sort INPUT_FILE OUTPUT_FILE NUM_THREADS\n";
}

void GenerateRandomNumbers(const std::string& filename, size_t size)
{
	std::ofstream outFile(filename);
	std::mt19937_64 rng(std::random_device{}());
	std::uniform_int_distribution<int64_t> dist(INT64_MIN, INT64_MAX);
	for (size_t i = 0; i < size; ++i)
	{
		outFile << dist(rng) << "\n";
	}
}

void Merge(std::vector<int64_t>& arr, int left, int mid, int right)
{
	std::vector<int64_t> leftPart(arr.begin() + left, arr.begin() + mid + 1);
	std::vector<int64_t> rightPart(arr.begin() + mid + 1, arr.begin() + right + 1);

	auto leftIt = leftPart.begin();
	auto rightIt = rightPart.begin();
	int currentIndex = left;

	while (leftIt != leftPart.end() && rightIt != rightPart.end())
	{
		if (*leftIt <= *rightIt)
		{
			arr[currentIndex++] = *leftIt++;
		}
		else
		{
			arr[currentIndex++] = *rightIt++;
		}
	}

	while (leftIt != leftPart.end())
	{
		arr[currentIndex++] = *leftIt++;
	}

	while (rightIt != rightPart.end())
	{
		arr[currentIndex++] = *rightIt++;
	}
}

void ParallelMergeSort(std::vector<int64_t>& arr, int left, int right, int numThreads) //TODO: Вместо Int использовать size_t
{
	if (left >= right) return;

	if (numThreads <= 1)
	{
		std::sort(arr.begin() + left, arr.begin() + right + 1); //TODO:
		return;
	}

	int mid = left + (right - left) / 2;

	std::jthread leftThread(ParallelMergeSort, std::ref(arr), left, mid, numThreads / 2);
	ParallelMergeSort(arr, mid + 1, right, numThreads - numThreads / 2);

	leftThread.join();

	Merge(arr, left, mid, right);
}

void SortFile(const std::string& inputFile, const std::string& outputFile, int maxThreads)
{
	std::ifstream inFile(inputFile);
	std::vector<int64_t> numbers((std::istream_iterator<int64_t>(inFile)), std::istream_iterator<int64_t>());
	auto copyNumbers = numbers;

	for (int numThreads = 1; numThreads <= maxThreads; ++numThreads)
	{
		auto unsortedNumbers = copyNumbers;
		auto start = std::chrono::high_resolution_clock::now();

		ParallelMergeSort(unsortedNumbers, 0, numbers.size() - 1, numThreads); //TODO: Не вычитать 1, а в right не делать прибавление 1

		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = end - start;

		assert(std::is_sorted(unsortedNumbers.begin(), unsortedNumbers.end()));
		numbers = unsortedNumbers;
		std::cout << "Sorted with " << numThreads << " threads in " << elapsed.count() << " seconds.\n";
	}

	std::ofstream outFile(outputFile);
	for (const auto& number: numbers)
	{
		outFile << number << "\n";
	}
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		PrintHelp();
		return 1;
	}

	std::string command = argv[1];

	if (command == "-h" || command == "--help")
	{
		std::cout << std::thread::hardware_concurrency() << std::endl;
		PrintHelp();
		return 0;
	}
	else if (command == "generate" && argc == 4)
	{
		GenerateRandomNumbers(argv[2], std::stoul(argv[3]));
		return 0;
	}
	else if (command == "sort" && argc == 5)
	{
		SortFile(argv[2], argv[3], std::stoi(argv[4]));
		return 0;
	}
	else
	{
		PrintHelp();
		return 1;
	}
}