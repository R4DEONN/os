#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include "iostream"

std::string BytesToMBString(long long bytes)
{
	return std::to_string(bytes / (1024 * 1024)) + "MB";
}

int main()
{
	utsname utsname{};
	if (uname(&utsname) != -1)
	{
		std::cout << "OS: " << utsname.sysname << " " << utsname.release << std::endl;
	}
	else
	{
		std::cout << "Error during system name recognition. Error code: " << errno << std::endl;
	}

	std::cout << "Processors: " << get_nprocs_conf() << std::endl;

	struct sysinfo sysInfo{};
	sysinfo(&sysInfo); //TODO: Exceptiion
	std::cout
		<< "RAM: "
		<< BytesToMBString(sysInfo.freeram)
		<< " / "
		<< BytesToMBString(sysInfo.totalram)
		<< std::endl;
}
