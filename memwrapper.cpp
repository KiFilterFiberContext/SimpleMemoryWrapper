#include "MemWrapper.h"
using namespace MemWrapper;

// INTERNALS ~~~
// honestly idk if all this code works 100% of the time, but it has worked for me so far

bool IntUtils::PlaceHook(const LPVOID source, const LPVOID pHook, const int length = 5)
{
	if (length < 5) { return false; };

	DWORD p;

	VirtualProtect(source, length, PAGE_READWRITE, &p);
	memset(source, 0x90, length); // nop nop nop

	*(BYTE*)source = 0xE9;

	DWORD diff = static_cast<DWORD>( (DWORD)pHook - (DWORD)source ); 
	*(DWORD*)((DWORD)source + 1) = diff; // bad mixup of casts lol

	VirtualProtect(source, length, p, &p);
	return GetLastError() ? false : true; // wait is this wrong, idk I'll fix later
}

DWORD IntUtils::GetFuncAddress(const char* libName, const char* symbol)
{
	HMODULE lib = LoadLibraryA(libName);
	DWORD prAddress = reinterpret_cast<DWORD>( GetProcAddress(lib, symbol) );

	return GetLastError() ? NULL : prAddress;
}

void IntUtils::CreateNewConsole()
{
	AllocConsole();
	freopen_s(&this->consoleOut, "CONOUT$", "w", stdout);
}

void IntUtils::DestroyConsole()
{
	fclose(this->consoleOut);
	FreeConsole();
}

// EXTERNALS ~~~

uintptr_t ExtUtils::GetModBaseAddress(const wchar_t* moduleName, const DWORD processID)
{
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processID);
	if (snap == INVALID_HANDLE_VALUE) { return NULL; };

	MODULEENTRY32 m;
	m.dwSize = sizeof(MODULEENTRY32); 

	if (!Module32First(snap, &m)) { return NULL; }; // bcuz we can't check modules if the first one fails

	do
	{
		if (!wcscmp(m.szModule, moduleName))
		{
			CloseHandle(snap);
			return reinterpret_cast<uintptr_t>( m.modBaseAddr );
		}
	} while (Module32Next(snap, &m));

	CloseHandle(snap);
	return 0;
}

DWORD ExtUtils::GetPID(const char* windowTitle)
{
	HWND hwnd = FindWindowA(NULL, windowTitle);
	DWORD pid;

	while (!hwnd)
	{
		std::cout << "Locating " << windowTitle << std::endl; // uSe pRinTf
		Sleep(250);

		if (GetAsyncKeyState(VK_BACK)) // haven't tried this using console yet
		{
			break;
		}
	}

	GetWindowThreadProcessId(hwnd, &pid);
	return GetLastError() ? NULL : pid; // yes
}

HANDLE ExtUtils::GetHandle()
{
	HANDLE h = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, this->pid);
	return h;
}

bool ExtUtils::WriteToAddr(const DWORD addr, const char* bytes, const size_t length)
{
	DWORD p;

	if (this->gHandle == nullptr) { return false; };

	VirtualProtectEx(gHandle, (LPVOID)(addr), length, PAGE_READWRITE, &p);
	WriteProcessMemory(gHandle, (LPVOID)(addr), bytes, length, 0);
	VirtualProtectEx(gHandle, (LPVOID)(addr), length, p, &p);

	return GetLastError() ? false : true;
}

bool ExtUtils::WriteToAddrs(const std::vector<DWORD>& addrs, const std::vector<const char*>& byteArray)
{
	DWORD p;
	if (this->gHandle == nullptr) { return false; };
	
	for (auto i = 0; i < addrs.size(); i++)
	{
		auto bytes = byteArray[i];
		auto addr = addrs[i]; // why do I use auto here again

		// length isn't working for now fully because null bytes
		size_t length = sizeof(bytes) - 1;

		VirtualProtectEx(gHandle, (LPVOID)(addr), length, PAGE_READWRITE, &p);
		WriteProcessMemory(gHandle, (LPVOID)(addr), bytes, length, 0);
		VirtualProtectEx(gHandle, (LPVOID)(addr), length, p, &p);
	}

	return GetLastError() ? false : true;
}

ExtUtils::ExtUtils(const char* title)
{
	this->windowTitle = title;
	this->pid = this->GetPID(title);
	this->gHandle = this->GetHandle();
}

ExtUtils::~ExtUtils()
{
	//This is a bad idea because what if there's an error :thinking:
	CloseHandle(this->gHandle);
}
