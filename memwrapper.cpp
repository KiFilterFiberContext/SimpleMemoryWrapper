#include "pch.h" //Remove if you are not using Visual Studio
#include "memwrapper.h"

using namespace MemoryWrapper;

DWORD Memory::GetModuleBaseAddress(DWORD procId, const wchar_t* modName)
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

DWORD Memory::GetPID()
{
	HWND hwnd = FindWindowA(0, gameTitle);
	DWORD pid;

	while (hwnd == NULL)
	{
		hwnd = FindWindowA(0, gameTitle);
		std::cout << "Waiting for process...\n\n";
		Sleep(500);
	}

	GetWindowThreadProcessId(hwnd, &pid);

	if (pid == NULL)
	{
		std::cout << "Error with retrieving process id\n";
		Sleep(500);
		return 0;
	}

	return pid;
}

HANDLE Memory::GetHandle()
{
	DWORD pid = GetPID();

	HANDLE h = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	return h;
}

void Memory::WriteToMemory(const char* bytes, DWORD address, int size)
{
	HANDLE h = GetHandle();
	DWORD oldProtection;
	DWORD newProtection;

	std::cin.get(); //waits to write memory

	VirtualProtectEx(h, (LPVOID)address, size, PAGE_EXECUTE_READWRITE, &oldProtection);
	WriteProcessMemory(h, (LPVOID)address, bytes, size, 0);
	VirtualProtectEx(h, (LPVOID)address, size, oldProtection, &newProtection);
	CloseHandle(h);
}

void Memory::ReadFromMemory(DWORD address, LPVOID buffer, int sizeOfBuffer)
{
	HANDLE h = GetHandle();
	ReadProcessMemory(h, (DWORD*)address, &buffer, sizeOfBuffer, 0);
	CloseHandle(h);
}
