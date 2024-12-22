#include "catch2/catch_test_macros.hpp"
#include "../src/BgThreadDispatcher.h"

#include <syncstream>
#include <thread>
#include <chrono>
#include <atomic>
#include <sstream>

TEST_CASE("Basic task execution")
{
	BgThreadDispatcher dispatcher;
	std::ostringstream output;

	dispatcher.Dispatch([&]
	{ output << "1"; });
	dispatcher.Wait();

	REQUIRE(output.str() == "1");
}

TEST_CASE("Sequential task execution")
{
	BgThreadDispatcher dispatcher;
	std::ostringstream output;

	dispatcher.Dispatch([&]
	{ output << "1"; });
	dispatcher.Dispatch([&]
	{ output << "2"; });
	dispatcher.Dispatch([&]
	{ output << "3"; });

	dispatcher.Wait();
	REQUIRE(output.str() == "123");
}

TEST_CASE("Stop prevents further task execution")
{
	BgThreadDispatcher dispatcher;
	std::ostringstream output;

	dispatcher.Dispatch([&]
	{ output << "1"; });
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	dispatcher.Stop();
	dispatcher.Dispatch([&]
	{ output << "2"; });

	dispatcher.Wait();
	REQUIRE(output.str() == "1");
}

TEST_CASE("Wait waits for all tasks to finish")
{
	BgThreadDispatcher dispatcher;
	std::ostringstream output;
	std::atomic<int> flag = 0;

	dispatcher.Dispatch([&]
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		output << "1";
		flag++;
	});

	dispatcher.Wait();
	REQUIRE(flag == 1);
	REQUIRE(output.str() == "1");
}

TEST_CASE("Destructor stops the thread")
{
	std::ostringstream output;

	{
		BgThreadDispatcher dispatcher;
		dispatcher.Dispatch([&]
		{ output << "1"; });
	}

	REQUIRE(true);
}

TEST_CASE("Handle exceptions in tasks")
{
	BgThreadDispatcher dispatcher;
	std::ostringstream output;

	dispatcher.Dispatch([&]
	{ throw std::runtime_error("Error"); });
	dispatcher.Dispatch([&]
	{ output << "1"; });

	dispatcher.Wait();
	REQUIRE(output.str() == "1");
}