#include <windows.h>
#include <versionhelpers.h>
#include "iostream"

std::string BytesToMBString(long long bytes)
{
	return std::to_string(bytes / (1024 * 1024)) + "MB";
}

std::string GetWindowsVersionString()
{
	if (IsWindows10OrGreater())
	{
		return "10 or Greater";
	}
	else if (IsWindows8OrGreater())
	{
		return "8 or Greater";
	}
	else if (IsWindows7OrGreater())
	{
		return "7 or Greater";
	}
	else if (IsWindowsXPOrGreater())
	{
		return "XP or Greater";
	}

	return ""; //TODO: Exception
}

int main()
{
	//TODO: Release
	std::cout << "OS: Windows " << GetWindowsVersionString() << std::endl;

	SYSTEM_INFO systemInfo;
	GetNativeSystemInfo(&systemInfo);
	std::cout << "Processors: " << systemInfo.dwNumberOfProcessors << std::endl;

	MEMORYSTATUSEX memoryStatus;
	memoryStatus.dwLength = sizeof (memoryStatus);
	if (GlobalMemoryStatusEx(&memoryStatus))
	{
		std::cout
			<< "RAM: "
			<< BytesToMBString(memoryStatus.ullAvailPhys)
			<< " / "
			<< BytesToMBString(memoryStatus.ullTotalPhys)
			<< std::endl;
	}
	else
	{
		std::cout << "Exception during memory status processing" << std::endl;
	}
}
