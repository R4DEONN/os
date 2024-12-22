#include "catch2/catch_test_macros.hpp"
#include "../src/ThreadPool.h"

#include <syncstream>
#include <thread>
#include <chrono>
#include <atomic>
#include <sstream>

TEST_CASE("ThreadPool Dispatch and Wait", "[ThreadPool]")
{
	ThreadPool pool(4);
	std::atomic<int> counter = 0;

	SECTION("Tasks are executed")
	{
		for (int i = 0; i < 100; ++i)
		{
			pool.Dispatch([&counter]()
			{
				counter++;
			});
		}
		pool.Wait();
		REQUIRE(counter == 100);
	}
}

TEST_CASE("ThreadPool Stop", "[ThreadPool]")
{
	ThreadPool pool(4);
	std::atomic<int> counter = 0;

	SECTION("Tasks are ignored after Stop")
	{
		for (int i = 0; i < 50; ++i)
		{
			pool.Dispatch([&counter]()
			{
				counter++;
			});
		}
		pool.Stop();
		pool.Dispatch([&counter]()
		{
			counter += 1000;
		});
		pool.Wait();
		REQUIRE(counter == 50);
	}
}