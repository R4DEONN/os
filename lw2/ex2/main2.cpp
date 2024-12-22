#include <csignal>
#include "fstream"
#include "sstream"
#include "iostream"

using namespace std;

string GetProcessName(pid_t pid)
{
	string filename = "/proc/" + to_string(pid) + "/comm";
	ifstream comm_file(filename);
	if (comm_file.is_open())
	{
		string process_name;
		getline(comm_file, process_name);
		comm_file.close();
		return process_name;
	}
	return "<unknown>";
}

long GetProcessMemory(pid_t pid)
{
	string filename = "/proc/" + to_string(pid) + "/status";
	ifstream status_file(filename);
	if (status_file.is_open())
	{
		string line;
		while (getline(status_file, line))
		{
			if (line.substr(0, 6) == "VmRSS:")
			{
				istringstream iss(line);
				string key;
				long memory;
				iss >> key >> memory;
				status_file.close();
				return memory;
			}
		}
		status_file.close();
	}
	return -1;
}

int main()
{
	const pid_t pid = getpid(); //TODO: Про все процессы вывести
	std::cout
		<< "PID: " << pid << std::endl
		<< "Process name: " << GetProcessName(pid) << std::endl
		<< "Memory usage: " << GetProcessMemory(pid) << " KB" << std::endl;

	return EXIT_SUCCESS;
}