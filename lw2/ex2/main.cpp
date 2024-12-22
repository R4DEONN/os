#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <dirent.h>

using namespace std;

//TODO: Есть ФАТАЛЬНЫЕ недостатки, исправить их по видосу
class DirWrapper
{
public:
	explicit DirWrapper(DIR* dir) : m_dir(dir)
	{
	}

	~DirWrapper()
	{
		if (m_dir)
		{
			closedir(m_dir);
		}
	}

	[[nodiscard]] DIR* get() const
	{
		return m_dir;
	}

private:
	DIR* m_dir;
};

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
				return memory;
			}
		}
	}
	return -1;
}

void ListProcesses() {
	DIR* dir = opendir("/proc");

	if (dir == nullptr) {
		std::cerr << "Could not open /proc directory." << std::endl;
		return;
	}

	auto dirRAII = DirWrapper(dir);
	struct dirent* entry;

	while ((entry = readdir(dirRAII.get())) != nullptr) //TODO: выяснить что делать с entry полученным через readdir (закрывать?)
	{
		if (entry->d_type == DT_DIR && isdigit(entry->d_name[0])) //TODO: предоставить доказательство, что достаточно проверить первый символ
		{
			auto pid = static_cast<pid_t>(std::stoi(entry->d_name));
			std::cout
				<< "PID: " << pid << std::endl
				<< "Process name: " << GetProcessName(pid) << std::endl;

			auto processMemory = GetProcessMemory(pid);
			if (processMemory == -1)
			{
				std::cout << "Can't find memory usage of " << pid << std::endl;
			}
			else
			{
				std::cout << "Memory usage: " << GetProcessMemory(pid) << " KB" << std::endl;
			}

			std::cout << std::endl; //TODO: обрабатывать ошибку из GetProcessmemory
		}
	}
}

int main() {
	ListProcesses();
	return 0;
}