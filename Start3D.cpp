// Activates 3D by manipulating memory for Deus Ex Mankind Divided
//

#include "Start3D.h"
#include <windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <vector>


int main()
{
	HWND GameWindow;
	int kez = 0;

	GameWindow = ::FindWindowExA(0, 0, "NxApp", 0); // Deus Ex Window Class Name 
	while ((GameWindow == NULL) & (kez <= 60))
	{
		Sleep(1000);
		GameWindow = ::FindWindowExA(0, 0, "NxApp", 0);
		kez++;
		if (kez == 60)
		{
			MessageBox(nullptr, TEXT("Deus Ex Mankind Divided uninitialized. \nPlease start the game first !!!"), TEXT("WARNING : Enabling 3D Vision"), MB_SYSTEMMODAL | MB_ICONEXCLAMATION | MB_OK);
			return 0;
		}
	}
	

	HWND LauncherMenu = FindWindowA("NxApp", "Deus Ex: Mankind Divided");  // Deus Ex : Launcher Menu  
	if (LauncherMenu != NULL) // for start game with skip menu settings
	{
		
		// "WIN + M" key has been pressed. (for "always on top" OS windows)
		keybd_event(0x5B, 0, 0, 0);
		Sleep(100);
		keybd_event(0x4D, 0, 0, 0);
		Sleep(100);
		keybd_event(0x4D, 0, KEYEVENTF_KEYUP, 0);
		Sleep(100);
		keybd_event(0x5B, 0, KEYEVENTF_KEYUP, 0);

		//It focuses on window and mouse to be able to press RETURN key. 
		
		SetForegroundWindow(GameWindow);
		Sleep(500);

		POINT pt;
		// you should set pt as a position in your Application
		ClientToScreen(GameWindow, &pt);

		// now pt is a screen position  ( mouse position on Launcher Menu)  
		SetCursorPos(pt.x +640, pt.y +70);
		Sleep(500);
		
		//It acts as if the "Return" key has been pressed.
		keybd_event(0x0D, 0, 0, 0);
		Sleep(500);
		keybd_event(0x0D, 0, KEYEVENTF_KEYUP, 0);     
	}



	// wait for loading symbol in the lower right corner of the screen
	Sleep(10000);  

	//Get ProcId of the target process
	DWORD procId = GetProcId(L"DXMD.exe");  //Deus Ex  

	//Getmodulebaseaddress
	uintptr_t moduleBase = GetModuleBaseAddress(procId, L"DXMD.exe");

	//Get Handle to Process
	HANDLE hProcess = 0;
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

	//Ingame options, resolve base address of the pointer chain    
	uintptr_t dynamicPtrBaseAddr = moduleBase + 0x2E1A220;

	//...Game menu >> Options >> Display and Graphics >> Display >> Resolve Stereoscopic pointer chain
	std::vector<unsigned int> StereoscopicOffsets = { 0x10, 0xA8, 0x1A4 }; // memory address of DXMD 3D Vision key
	uintptr_t StereoscopicAddr = FindDMAAddy(hProcess, dynamicPtrBaseAddr, StereoscopicOffsets);

	//Write to it
	int Stereoscopic = 2;  // 3D Vision ON  (0= off  1= SBS  2= 3D Vision)

	// Each time the window of the game is seen, it enters into memory the value of 3D Vision.
	while (FindWindowExA(0, 0, "NxApp", 0)) {
		WriteProcessMemory(hProcess, (BYTE*)StereoscopicAddr, &Stereoscopic, sizeof(Stereoscopic), nullptr);
		Sleep(1000);
	}



	return 0;
}