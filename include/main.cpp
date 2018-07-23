#include "Recorder.hpp"
#include <iostream>
#include <thread>
#include <filesystem>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
   Data::Recorder mmrData;
   while (!(GetKeyState(VK_CONTROL) & 0x8000) || !(GetKeyState('D') & 0x8000) || !(GetKeyState('Q') & 0x8000))
   {
	   if (GetKeyState(VK_CONTROL) & 0x8000 && GetKeyState('D') & 0x8000 && GetKeyState(VK_SNAPSHOT) & 0x8000)
	   {
		   while (GetKeyState(VK_CONTROL) & 0x8000 && GetKeyState('D') & 0x8000 && GetKeyState(VK_SNAPSHOT) & 0x8000)
		   {
			   // in here we just eat up cycles until the keys are depressed
			   std::this_thread::sleep_for(std::chrono::milliseconds(17));
		   }
		   mmrData.getMMR();
	   }
	   std::this_thread::sleep_for(std::chrono::milliseconds(17));
   }
   return 1;
}