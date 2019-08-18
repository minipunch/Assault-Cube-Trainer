// AssaultCubeTrainer.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <Windows.h>
#include "proc.h"
#include "mem.h"

void setAmmo(HANDLE hProcess, uintptr_t playerPtr, int newAmmo) {
	std::cout << "Player ptr addr: 0x" << std::hex << playerPtr << std::endl;

	// Resolve ammo pointer chain
	std::vector<unsigned int> ammoOffsets = { 0x374, 0x14, 0x0 }; // found offsets manually using CE
	uintptr_t ammoAddr = FindDMAAddy(hProcess, playerPtr, ammoOffsets);

	std::cout << "Ammo addr: 0x" << std::hex << ammoAddr << std::endl;

	// Read it (to test)
	int ammoVal = 0;

	ReadProcessMemory(hProcess, (BYTE*)ammoAddr, &ammoVal, sizeof(ammoVal), nullptr);

	std::cout << "Found ammo value!! It should be: " << std::dec << ammoVal << std::endl;

	// Write to it
	WriteProcessMemory(hProcess, (BYTE*)ammoAddr, &newAmmo, sizeof(newAmmo), nullptr);

	// Read it (to test)
	ReadProcessMemory(hProcess, (BYTE*)ammoAddr, &ammoVal, sizeof(ammoVal), nullptr);

	std::cout << "After modification, ammo should now be: " << std::dec << ammoVal << std::endl;
}

void enableGodMode(uintptr_t healthAddr, HANDLE hProcess) { // seems to update HUD, does it actually work tho?
	const int hp = 1337;
	mem::PatchEx((BYTE*)healthAddr, (BYTE*)&hp, sizeof(hp), hProcess);
}

int main()
{
	HANDLE hProcess = 0;

	uintptr_t moduleBase = 0, localPlayerPtr = 0, healthAddr = 0;
	bool bHealth = false, bAmmo = false, bRecoil = false;

	DWORD procId = GetProcId(L"ac_client.exe");

	if (procId) {
		int PLAYER_PTR_BASE_ADDR = 0x10F4F4;
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);
		moduleBase = GetModuleBaseAddress(procId, L"ac_client.exe");
		localPlayerPtr = moduleBase + PLAYER_PTR_BASE_ADDR;
		healthAddr = FindDMAAddy(hProcess, localPlayerPtr, { 0xf8 });
	}
	else {
		std::cout << "Process not found, press enter to exit\n";
		getchar();
		return 0;
	}

	DWORD dwExit = 0;

	while (GetExitCodeProcess(hProcess, &dwExit) && dwExit == STILL_ACTIVE) {
		// activate god mode
		if (GetAsyncKeyState(VK_NUMPAD1) & 1) {
			bHealth = !bHealth;
		}
		else if (GetAsyncKeyState(VK_NUMPAD2) & 1) {
			// set ammo
			setAmmo(hProcess, localPlayerPtr, 1337);
		} else if (GetAsyncKeyState(VK_NUMPAD3) & 1) {
			// no recoil
			mem::NopEx((BYTE*)(moduleBase + 0x63786), 10, hProcess);
		}

		// god mode loop (apparently doesn't work on multiplayer sessions)
		if (bHealth) {
			enableGodMode(healthAddr, hProcess);
		}
		Sleep(10);
	}

	std::cout << "Process not found, press enter to exit\n";

	getchar();
    return 0;
}