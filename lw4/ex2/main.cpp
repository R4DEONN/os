#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <syncstream>
#include <stop_token>
#include <iomanip>

//TODO: stop_source destructor stop_token
void PrintMessage(std::stop_token stoken, const std::string& message, int interval)
{
	while (!stoken.stop_requested())
	{
		auto now = std::chrono::system_clock::now();
		time_t now_time = std::chrono::system_clock::to_time_t(now);
		std::osyncstream sync(std::cout);
		sync << std::put_time(std::localtime(&now_time), "%H:%M:%S ") << message << std::endl;
		sync.flush();
		std::this_thread::sleep_for(std::chrono::seconds(interval));
	}

	auto now = std::chrono::system_clock::now();
	time_t now_time = std::chrono::system_clock::to_time_t(now);
	std::osyncstream sync(std::cout);
	sync << std::put_time(std::localtime(&now_time), "%H:%M:%S ") << message << " thread completed" << std::endl;
}

int main()
{
	//TODO: не использовать stop_source
	std::stop_source stopSource;

	//TODO: Выяснить как передаются параметры в thread
	std::jthread ticThread(PrintMessage, stopSource.get_token(), "Tic", 2);
	std::jthread tacThread(PrintMessage, stopSource.get_token(), "Tac", 3);

	std::string input;
	std::getline(std::cin, input);

	stopSource.request_stop();

	return 0;
}