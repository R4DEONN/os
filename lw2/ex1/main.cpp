#include <windows.h>
#include <psapi.h>
#include "iostream"

int main()
{
	std::cout << "PID: " << getpid() << std::endl;

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, getpid());
	if (hProcess == nullptr)
	{
		return EXIT_FAILURE;
	}

	try
	{

		HMODULE hMod;
		DWORD cbNeeded;
		char processName[MAX_PATH] = "unknown"; //TODO: GetProcessImageFileName()

		if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
		{
			GetModuleBaseNameA(hProcess, hMod, processName, sizeof(processName) / sizeof(char));
		}

		PROCESS_MEMORY_COUNTERS processMemoryCounters;
		if (GetProcessMemoryInfo(hProcess, &processMemoryCounters, sizeof(processMemoryCounters)))
		{
			std::cout << "Process name: " << processName << std::endl
					  << "Memory usage: " << processMemoryCounters.WorkingSetSize / 1024 << " KB" << std::endl;
		}

		CloseHandle(hProcess);

		return EXIT_SUCCESS;
	}
	catch (const std::exception& e)
	{
		CloseHandle(hProcess); //TODO: RAII
		return EXIT_FAILURE;
	}
}