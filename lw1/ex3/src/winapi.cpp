#ifdef _WIN32
#include "winapi.h"
#include <windows.h>
#include <versionhelpers.h>
#include <stdexcept>

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

	throw std::logic_error("Unknown version of Windows");
}
#endif