#include <atomic>
#include "iostream"
#include "csignal"
#include "fstream"
#include "filesystem"
#include "thread"

std::atomic_flag exitRequested;

void SignalHandler(int s)
{
	if (s == SIGINT || s == SIGTERM)
	{
		std::cout << "Process " << getpid() << " has received signal #" << s << "\n";
		exitRequested.test_and_set();
	}
}

int main()
{
	using namespace std::chrono_literals;
	signal(SIGINT, SignalHandler);
	signal(SIGTERM, SignalHandler);

	std::ofstream temp("temp.txt");
	for (int i = 0; i < 101 && !exitRequested.test(); i++)
	{
		std::cout << i << std::endl;
		std::this_thread::sleep_for(50ms);
	}

	if (!exitRequested.test())
	{
		try
		{
			temp.close();
			auto path = std::filesystem::path("temp.txt");
			std::filesystem::remove(path);
		}
		catch (const std::exception& err)
		{
			std::cout << err.what() << std::endl;
		}

		std::cout << "Done" << std::endl;
	}

}