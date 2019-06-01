#include "pch.h" //Dont include if you are not using Visual Studio
#include "memwrapper.h"

using namespace MemoryWrapper; //other you will have to write MemoryWrapper::Memory

int main()
{
  Memory m;
  m.gameTitle = "Geometry Dash";
  DWORD processID = m.GetPID();
  
  DWORD base = m.GetModuleBaseAddress(processID, L"GeometryDash.exe");
  DWORD offset = (DWORD) 0xAE8FA;
  
  m.WriteToMemory("\xEB", (base+offset), 1); // We cast byte to a byte pointer because WriteToMemory takes a byte array

  //No need to do 'delete m' because m is not a pointer, it gets popped off the stack when finished
  return 0;
}
