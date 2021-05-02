#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

// STOLEN FROM GUIDED HACKING
DWORD GetProcId(const wchar_t* procName)
{
	DWORD procId = 0;
	HANDLE hSnap = (CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));

	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(hSnap, &procEntry))
		{
			do
			{
				if (!_wcsicmp(procEntry.szExeFile, procName))
				{
					procId = procEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnap, &procEntry));
		}
	}

	CloseHandle(hSnap);
	return procId;
}

// STOLEN FROM GUIDED HACKING
uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
	uintptr_t modBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry))
		{
			do
			{
				if (!_wcsicmp(modEntry.szModule, modName))
				{
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					break;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap);
	return modBaseAddr;
}

int main()
{
	DWORD procId = GetProcId(L"GALAGA.exe");

	HANDLE hProcess = 0;
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

	uintptr_t moduleBase = GetModuleBaseAddress(procId, L"Release_1.dll");

	uintptr_t xAddr = moduleBase + 0x70B62;

	POINT cPos;

	unsigned char xPos = 0;

	while (true)
	{
		GetCursorPos(&cPos);

		if (cPos.x >= 630 && cPos.x <= 1370) {
			xPos = (((cPos.x - 630) * (226 - 16)) / (1370 - 630)) + 16;
		}

		WriteProcessMemory(hProcess, (PVOID)xAddr, &xPos, sizeof(xPos), 0);
	}
}
