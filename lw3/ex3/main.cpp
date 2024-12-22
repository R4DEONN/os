#include <atomic>
#include "thread"
#include "iostream"
#include "fstream"
#include "sstream"
#include "queue"
#include "csignal"

volatile std::atomic_flag exitRequested;

void SignalHandler(int s)
{
	if (s == SIGINT || s == SIGTERM)
	{
		std::cout << "Process " << getpid() << " has received signal #" << s << "\n";
		exitRequested.test_and_set();
	}
}

class Pipe //TODO: Поменять обертку на filedesc с одним файлом
{
public:
	Pipe()
	{
		if (pipe(m_pipe) == -1)
		{
			throw std::system_error(errno, std::generic_category());
		}
	}

	Pipe(const Pipe&) = delete;
	Pipe& operator=(const Pipe&) = delete;

	~Pipe()
	{
		//TODO: исправить
		close(m_pipe[0]);
		close(m_pipe[1]);
	}

	[[nodiscard]] int GetReadPipe() const
	{
		return m_pipe[0];
	}

	[[nodiscard]] int GetWritePipe() const
	{
		return m_pipe[1];
	}

private:
	int m_pipe[2]{};
};

class CommandHandler
{
public:
	CommandHandler()
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
			m_childPid = pid;
			close(m_pipeToChild.GetReadPipe());
			close(m_pipeFromChild.GetWritePipe());
		}
	}

	bool HandleCommand()
	{
		std::string commandLine;
		std::getline(std::cin, commandLine);
		if (commandLine == "exit")
		{
			Exit();
			return false;
		}

		write(m_pipeToChild.GetWritePipe(), commandLine.c_str(), commandLine.size());

		char buffer[256];
		if (read(m_pipeFromChild.GetReadPipe(), buffer, sizeof(buffer)) > 0)
		{
			std::cout << buffer << std::endl;
		}

		return true;
	}

private:
	pid_t m_childPid;
	Pipe m_pipeToChild;
	Pipe m_pipeFromChild;

	void Exit() const
	{
		kill(m_childPid, SIGTERM);
	}

	void ChildThread()
	{
		InitChildThread();

		char buffer[256]; //TODO: надо сделать так, чтобы программа норм работала (в начале сделать количество байтов)
		while (!exitRequested.test() && read(m_pipeToChild.GetReadPipe(), buffer, sizeof(buffer)) > 0)
		{
			ChildHandleCommand(buffer);
		}
		std::cout << "Child is over" << std::endl;
	}

	void InitChildThread()
	{
		sleep(30);
		close(m_pipeToChild.GetWritePipe());
		close(m_pipeFromChild.GetReadPipe());
		std::signal(SIGINT, SignalHandler); //TODO: до форка установить обработчики сигнала
		std::signal(SIGTERM, SignalHandler);
	}

	void ChildHandleCommand(const std::string& command)
	{
		if (command.substr(0, 3) == "add")
		{
			Add(command.substr(4));
		}
		else if (command.substr(0, 12) == "longest_word")
		{
			LongestWord(command.substr(13, command.size() - 16));
		}
		else
		{
			throw std::system_error(errno, std::generic_category());
		}
	}

	void Add(const std::string& args)
	{
		std::istringstream iss(args);
		int sum = 0;
		int number = 0;

		while (iss >> number)
		{
			sum += number;
		}

		std::string result = "Sum is " + std::to_string(sum);
		write(m_pipeFromChild.GetWritePipe(), result.c_str(), result.size());
	}

	void LongestWord(const std::string& fileName)
	{
		std::ifstream ifs(fileName);
		std::string longestWord;
		std::string currentWord;

		while (ifs >> currentWord)
		{
			if (currentWord.size() > longestWord.size())
			{
				longestWord = currentWord;
			}
		}

		std::string result = "Longest word is " + longestWord;
		write(m_pipeFromChild.GetWritePipe(), result.c_str(), result.size());
	}
};

int main()
{
	try
	{
		auto commandHandler = CommandHandler();

		while (!exitRequested.test() && commandHandler.HandleCommand())
		{}

		return EXIT_SUCCESS;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}