#pragma once //Change this depending if you're using Visual Studio
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>

namespace MemoryWrapper
{
	class Memory
	{
	public:
		DWORD processID;
		const char* gameTitle;

		DWORD GetModuleBaseAddress(DWORD, const wchar_t*);

		DWORD GetPID();

		HANDLE GetHandle();

		void WriteToMemory(BYTE*, DWORD, int);

		void ReadFromMemory(DWORD, LPVOID, int);
	};
}
