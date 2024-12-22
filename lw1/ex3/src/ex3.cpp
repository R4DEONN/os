#include "iostream"
#include "SysInfo.h"

std::string BytesToMBString(long long bytes)
{
	return std::to_string(bytes / (1024 * 1024)) + "MB";
}

int main()
{
	auto sysInfo = SysInfo();
	std::cout << "OS: " << sysInfo.GetOSName() << std::endl;
	std::cout << "Version: " << sysInfo.GetOSVersion() << std::endl;
	std::cout
		<< "Memory: "
		<< BytesToMBString(sysInfo.GetFreeMemory())
		<< " / "
		<< BytesToMBString(sysInfo.GetTotalMemory())
		<< std::endl;
	std::cout << "Processors: " << sysInfo.GetProcessorCount() << std::endl;
}
