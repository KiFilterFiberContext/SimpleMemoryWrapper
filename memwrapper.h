#ifndef MEMWRAPPER_H
#define MEMWRAPPER_H

//includes, but this should be obvious
#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <string>
#include <wchar.h>
#include <vector>

// Here is pretty ok code, not great but not extremely bad I think

//namespace /shrug
namespace MemWrapper
{
	// Class which handles internal memory editing and other stuff for x86 applications
	class IntUtils
	{
	private:
		FILE* consoleOut;

	public:
		// Creates a hook to the supplied source function which we will nop before hand (idk if that makes sense lol)
		// If length is not specified it will default to a length of 5 
		bool PlaceHook(void* source, void* pHook, const int length);

		// Retrieves the address from a library of a function symbol
		// Returns 0 if there is an error
		DWORD GetFuncAddress(const char* libName, const char* symbol);

		// Allocates memory and instantiates a new console
		void CreateNewConsole();

		// Destroys the current console which was spawned using CreateNewConsole
		void DestroyConsole();
	};

	// Class which handles external memory editing and other related stuff for x86 applications
	class ExtUtils
	{
	private:

	public:
		const char* windowTitle;
		DWORD pid;
		HANDLE gHandle;

		// Sets up the class
		ExtUtils(const char* title);

		// Closes the handle but this is bad because what if it fails, then that would be up to the person making stuff
		~ExtUtils();

		// Retrieves the base address of a loaded module with the specified name
		uintptr_t GetModBaseAddress(const wchar_t* moduleName, const DWORD processID);

		// Get the handle to the window with the window name specified in the constructor
		HANDLE GetHandle();

		// Writes to the memory address specified
		bool WriteToAddr(const DWORD addr, const char* bytes, size_t length);

		// Writes to several memory addresses duh
		bool WriteToAddrs(const std::vector<DWORD>& addrs, const std::vector<const char*>& byteArray);

		// Gets the process id of the specified window
		DWORD GetPID(const char* windowTitle);
	};
}

#endif // MEMWRAPPER_H
