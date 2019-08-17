// AssaultCubeTrainer.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <Windows.h>
#include "proc.h"


int main()
{

	int AMMO_PTR_BASE_ADDR = 0x10F4F4;

	// Get process ID
	DWORD procId = GetProcId(L"ac_client.exe");

	// Get module base address
	uintptr_t moduleBase = GetModuleBaseAddress(procId, L"ac_client.exe");

	// Get process handle
	HANDLE hProcess = 0;
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

	/* MODIFYING AMMO */
	// Resolve base address of ammo pointer chain
	uintptr_t dynamicPtrBaseAddr = moduleBase + AMMO_PTR_BASE_ADDR;

	std::cout << "Ammo ptr addr: 0x" << std::hex << dynamicPtrBaseAddr << std::endl;

	// Resolve ammo pointer chain
	std::vector<unsigned int> ammoOffsets = { 0x374, 0x14, 0x0 }; // found offsets manually using CE
	uintptr_t ammoAddr = FindDMAAddy(hProcess, dynamicPtrBaseAddr, ammoOffsets);

	std::cout << "Ammo addr: 0x" << std::hex << ammoAddr << std::endl;

	//TESTING:

	// Read it
	int ammoVal = 0;

	ReadProcessMemory(hProcess, (BYTE*)ammoAddr, &ammoVal, sizeof(ammoVal), nullptr);

	std::cout << "Found ammo value!! It should be: " << std::dec << ammoVal << std::endl;

	// Write to it
	int newAmmo = 1337;
	
	WriteProcessMemory(hProcess, (BYTE*)ammoAddr, &newAmmo, sizeof(newAmmo), nullptr);

	// Read it
	ReadProcessMemory(hProcess, (BYTE*)ammoAddr, &ammoVal, sizeof(ammoVal), nullptr);

	std::cout << "After modification, ammo should now be: " << std::dec << ammoVal << std::endl;

	getchar();

    return 0;
}