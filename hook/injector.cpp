#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <thread>
#include <chrono>

#include <windows.h>
#include <tlhelp32.h>
#include <easyhook.h>
#include <tchar.h>

int wmain(int argc, WCHAR* argv[])
{
	if (argc != 2) {
		std::wcout << argv[0] << " dll\n";
		return 1;
	}
	WCHAR* dllToInject = argv[1];
	
	DWORD processId = -1;
	
	int tries = 0;
	
	while (processId == -1 && tries++ < 10) {

		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		PROCESSENTRY32 entry;
		entry.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(snapshot, &entry) != FALSE)
		{
			while (Process32Next(snapshot, &entry) != FALSE)
			{
				if (std::string (entry.szExeFile).find ("RenderService-64.exe") != std::string::npos)
				{
					processId = entry.th32ProcessID;
					std::wcout << "found RenderService-64.exe with pid " << processId << "\n";
				}
			}
		}
		
		CloseHandle(snapshot);
		
		if (processId == -1) {
			std::this_thread::sleep_for (std::chrono::milliseconds (100));
		}
	}

	if (processId == -1) {
		std::wcout << "no render service process found\n";
		return 1;
	}

	NTSTATUS nt = RhInjectLibrary(
		processId,
		0,
		EASYHOOK_INJECT_DEFAULT,
		NULL,
		dllToInject,
		nullptr,
		0
	);


	if (nt != 0)
	{
		std::wcout << "RhInjectLibrary failed with error code = " << nt << "\n  " << RtlGetLastErrorString() << "\n";
		return 1;
	}
	
	return 0;
}