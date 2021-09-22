# TerminateProcesses
Injects a DLL into explorer.exe which terminates common blue team tools on Windows devices. 

Current list includes: "procexp.exe", "procexp64.exe", "Tcpview.exe", "Autoruns.exe", "Autoruns64.exe", "autorunsc.exe", "autorunsc64.exe", "Taskmgr.exe", "Procmon.exe", "Procmon64.exe", "ida64.exe", "OLLYDBG.EXE", "regedit.exe", "Wireshark.exe"

Pretty simple to thwart - just restart the explorer.exe process using PowerShell or something. Or rename any of the executables. No real persistence mechanism in place. Just some fun.
