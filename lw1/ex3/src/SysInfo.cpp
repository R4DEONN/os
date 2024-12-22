#include "SysInfo.h"
#include "stdexcept"
#ifdef linux
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#endif
#ifdef _WIN32
#include "winapi.h"
#include <sysinfoapi.h>
#endif

#ifdef _WIN32
std::string SysInfo::GetOSName() const
{
	return "Windows";
}

std::string SysInfo::GetOSVersion() const
{
	return GetWindowsVersionString();
}

uint64_t SysInfo::GetFreeMemory() const
{
	uint64_t freeMemory;
	MEMORYSTATUSEX memoryStatus;
	memoryStatus.dwLength = sizeof (memoryStatus);
	if (GlobalMemoryStatusEx(&memoryStatus))
	{
		freeMemory = memoryStatus.ullAvailPhys;
	}
	else
	{
		throw std::logic_error("Exception during available memory size recognize");
	}
	return freeMemory;
}

uint64_t SysInfo::GetTotalMemory() const
{
	uint64_t totalMemory;
	MEMORYSTATUSEX memoryStatus;
	memoryStatus.dwLength = sizeof (memoryStatus);
	if (GlobalMemoryStatusEx(&memoryStatus))
	{
		totalMemory = memoryStatus.ullTotalPhys;
	}
	else
	{
		throw std::logic_error("Exception during available memory size recognize");
	}

	return totalMemory;
}

unsigned SysInfo::GetProcessorCount() const
{
	uint64_t processorsCount;
	SYSTEM_INFO systemInfo;
	GetNativeSystemInfo(&systemInfo);
	processorsCount = systemInfo.dwNumberOfProcessors;

	return processorsCount;
}
#endif

#ifdef linux
std::string SysInfo::GetOSName() const
{
	std::string osname;
	utsname utsname{};
	if (uname(&utsname) != -1)
	{
		osname = utsname.sysname;
	}
	return osname;
}

std::string SysInfo::GetOSVersion() const
{
	std::string osversion;
	utsname utsname{};
	if (uname(&utsname) != -1)
	{
		osversion = utsname.release;
	}
	return osversion;
}

uint64_t SysInfo::GetFreeMemory() const
{
	uint64_t freeMemory;
	struct sysinfo sysInfo{};
	sysinfo(&sysInfo);
	freeMemory = sysInfo.freeram;
	return freeMemory;
}

uint64_t SysInfo::GetTotalMemory() const
{
	uint64_t totalMemory;
	struct sysinfo sysInfo{};
	sysinfo(&sysInfo);
	totalMemory = sysInfo.totalram;
	return totalMemory;
}

unsigned SysInfo::GetProcessorCount() const
{
	uint64_t processorsCount;
	processorsCount = get_nprocs_conf();
	return processorsCount;
}
#endif