#include <atomic>
#include "thread"
#include "iostream"
#include "queue"
#include "csignal"

std::atomic_flag exitRequested;

void SignalHandler(int s)
{
	if (s == SIGINT || s == SIGTERM)
	{
		std::cout << "Process " << getpid() << " has received signal #" << s << "\n";
		exitRequested.test_and_set();
	}
}

class CommandHandler
{
public:
	bool HandleCommand()
	{
		std::string commandLine;
		std::cin >> commandLine;

		if (commandLine == "exit")
		{
			return false;
		}
		else if (commandLine == "spawn")
		{
			try
			{
				Spawn();
			}
			catch (const std::exception& err)
			{
				std::cerr << err.what() << std::endl;
			}
		}
		else if (commandLine == "kill")
		{
			Kill();
		}

		return true;
	}
private:
	std::queue<pid_t> m_childPids;

	void Spawn()
	{
		auto pid = fork();

		if (pid < 0)
		{
			throw std::logic_error("Error at fork child process");
		}
		else if (pid == 0)
		{
			ChildThread();
		}
		else
		{
			m_childPids.push(pid);
		}
	}

	void ChildThread()
	{
		using namespace std::chrono_literals;
		std::signal(SIGINT, SignalHandler);
		std::signal(SIGTERM, SignalHandler);

		while (!exitRequested.test())
		{
			std::cout << "Child process " << getpid() << " is still alive" << std::endl;
			std::this_thread::sleep_for(5000ms);
		}

		_exit(0); //TODO: Не вызвать exit
	}

	void Kill()
	{
		if (m_childPids.empty())
		{
			std::cout << "No children to kill" << std::endl;
			return;
		}
		pid_t childPid = m_childPids.front();
		m_childPids.pop();
		kill(childPid, SIGTERM); //TODO: Дождаться waitpid
	}
};

int main()
{
	auto commandHandler = CommandHandler();

	while (commandHandler.HandleCommand())
	{}

	return EXIT_SUCCESS;
}