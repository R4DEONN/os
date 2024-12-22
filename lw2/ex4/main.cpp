#include <iostream>
#include <fstream>
#include <cctype>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <string>

using namespace std;

char FlipCharCase(char ch)
{
	if (islower(ch))
	{
		return toupper(ch);
	}
	else
	{
		return tolower(ch);
	}
}

void FlipCase(const string& input_file)
{
	ifstream infile(input_file);
	if (!infile)
	{
		cerr << "Ошибка: не удалось открыть файл " << input_file << endl;
		_exit(1);
	}

	string outputFileName = input_file + ".out";
	ofstream outfile(outputFileName);
	if (!outfile)
	{
		cerr << "Ошибка: не удалось создать файл " << outputFileName << endl;
		_exit(1);
	}

	char ch;
	while (infile.get(ch))
	{
		ch = FlipCharCase(ch);
		outfile.put(ch);
	}

	infile.close();
	outfile.close();
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		cerr << "Usage: " << argv[0] << " <files...>" << endl;
		return 1;
	}

	vector<pid_t> childPids;

	for (int i = 1; i < argc; ++i)
	{
		pid_t pid = fork();

		if (pid < 0)
		{
			cerr << "Error during create child process while processing " << argv[i] << endl;
			return 1;
		}
		else if (pid == 0)
		{
			cout << "Process " << getpid() << " is processing " << argv[i] << endl;

			FlipCase(argv[i]);

			cout << "Process " << getpid() << " has finished writing to " << argv[i] << ".out" << endl;
			_exit(0); //TODO: не использщовать exit
		}
		else
		{
			childPids.push_back(pid);
		}
	}

	for (pid_t pid : childPids)
	{
		int status;
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
		{
			cout << "Child process " << pid << " is over" << endl;
		}
	}

	return 0;
}
