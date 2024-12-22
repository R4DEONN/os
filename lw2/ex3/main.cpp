#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <csignal>

using namespace std;

int main() {
	pid_t pid = fork();

	if (pid < 0)
	{
		cerr << "Error at fork child process" << endl;
		return 1;
	}
	else if (pid == 0)
	{
//		std::cout << "Child pid: " << getpid() << std::endl;
		_exit(0);
	}
	else
	{
		pid_t input_pid;
		int status;

		cout << "Child process is created" << endl;

		do
		{
			cin >> input_pid; //TODO: Почему если подать -1, то программа, что-то удаляет

			if (waitpid(input_pid, &status, WNOHANG) == -1) //TODO: выяснить, что если родитель не вызовет waitpid, что будет с child process
			{
				cerr << "Wrong PID" << endl;
			}
			else
			{
				cout << "Process " << input_pid << " canceled." << endl;
				break;
			}
		}
		while (true);
	}

	return 0;
}