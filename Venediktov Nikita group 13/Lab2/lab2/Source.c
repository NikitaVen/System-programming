#include <windows.h>
#include <TlHelp32.h>
#include <tchar.h> // for TCHAR-s, _tprinf, TEXT

BOOL GetProcessList();  //walking through processes
BOOL ListProcessModules(DWORD PID); //walking through modules of the process
BOOL ListProcessThreads(DWORD OwnerPID); //walking through threads od the process
void printError(TCHAR* msg);

int main()
{
	GetProcessList();
	system("pause");
	return 0;
}

BOOL GetProcessList()
{
	HANDLE ProcessSnap = INVALID_HANDLE_VALUE;
	HANDLE Process; //particular process
	PROCESSENTRY32 pe; //entry to the processes
	DWORD PriorityClass;

	ProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); //snapshot of all the processes
	//dfFlags - type of info we want to get; ProccessID = 0 because we don't need PID, any particular process
	if (ProcessSnap == INVALID_HANDLE_VALUE)
	{
		printError(TEXT("CreateToolhelp32Snapshot of processes"));
		return FALSE;
	}
	pe.dwSize = sizeof(PROCESSENTRY32);
	
	//attempt to get the info about the first process
	if (!Process32First(ProcessSnap, &pe))
	{
		printError(TEXT("Process32First"));
		CloseHandle(ProcessSnap);
		return FALSE;
	}

	//displaying info of processes in the sequience
	do
	{
		_tprintf(TEXT("\n\n================================="));
		_tprintf(TEXT("\n PROCESS NAME: %s"), pe.szExeFile);
		_tprintf(TEXT("\n================================="));

		PriorityClass = 0;
		Process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ParentProcessID);
		if (Process == NULL)
		{
			printError(TEXT("OpenProcess"));
		}
		else
		{
			PriorityClass = GetPriorityClass(Process);
			if (!PriorityClass)
			{
				printError(TEXT("GetPriorityClass"));
			}
			CloseHandle(Process);
		}

		_tprintf(TEXT("\n  PROCESS ID        = 0x%08X"), pe.th32ProcessID); //%08X print in 8 width and pad with 0's
		_tprintf(TEXT("\n  THREAD COUNT      = %d"), pe.cntThreads);
		_tprintf(TEXT("\n  Parent PROCESS ID = 0x%08X"), pe.th32ParentProcessID);
		_tprintf(TEXT("\n  PRIORITY BASE     = %d"), pe.pcPriClassBase);
		_tprintf(TEXT("\n  MODULE COUNT      = %d"), ModuleCount(pe.th32ProcessID));

		//displaying modules and threads of parcitular processes
		ListProcessModules(pe.th32ProcessID);
		ListProcessThreads(pe.th32ProcessID);

	} while (Process32Next(ProcessSnap, &pe));

	CloseHandle(ProcessSnap);

	return TRUE;
}

int ModuleCount(DWORD PID)
{
	HANDLE ModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me;

	ModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);
	if (ModuleSnap == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	me.dwSize = sizeof(MODULEENTRY32); 

	if (!Module32First(ModuleSnap, &me))
	{
		printError(TEXT("Module32First"));
		CloseHandle(ModuleSnap);
		return 0;
	}
	int amount = 0;
	do
	{
		++amount;
	} while (Module32Next(ModuleSnap, &me));

	CloseHandle(ModuleSnap);

	return amount;
}

BOOL ListProcessModules(DWORD PID)
{
	HANDLE ModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me; // entry to the modules in the process

	ModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);
	if (ModuleSnap == INVALID_HANDLE_VALUE)
	{
		printError(TEXT("CreateToolhelp32Snapshot of modules"));
		return FALSE;
	}

	me.dwSize = sizeof(MODULEENTRY32); //analogy of pe.dwSize

	if (!Module32First(ModuleSnap, &me))
	{
		printError(TEXT("Module32First"));
		CloseHandle(ModuleSnap);
		return FALSE;
	}

	do
	{
		_tprintf(TEXT("\n\n\tMODULE NAME:     %s"), me.szModule);
		_tprintf(TEXT("\n\tPATH           = %s"), me.szExePath);
		_tprintf(TEXT("\n\tPROCESS ID     = 0x%08X"), me.th32ProcessID);
		_tprintf(TEXT("\n\tBASE SIZE      = %d"), me.modBaseSize);

	} while (Module32Next(ModuleSnap, &me));

	CloseHandle(ModuleSnap);

	return TRUE;
}

BOOL ListProcessThreads(DWORD OwnerPID)
{
	HANDLE ThreadSnap = INVALID_HANDLE_VALUE;
	THREADENTRY32 te; // entry  to the threads in the process

	ThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (ThreadSnap == INVALID_HANDLE_VALUE)
	{
		printError(TEXT("CreateToolhelp32Snapshot of threads"));
		return FALSE;
	}

	te.dwSize = sizeof(THREADENTRY32);

	if (!Thread32First(ThreadSnap, &te))
	{
		printError(TEXT("Thread32First"));
		CloseHandle(ThreadSnap);          
		return FALSE;
	}

	do
	{
		if (te.th32OwnerProcessID == OwnerPID)
		{
			_tprintf(TEXT("\n\n\t  THREAD ID      = 0x%08X"), te.th32ThreadID);
			_tprintf(TEXT("\n\t  BASE PRIORITY  = %d"), te.tpBasePri);
			_tprintf(TEXT("\n\t  DELTA PRIORITY = %d"), te.tpDeltaPri);
			_tprintf(TEXT("\n"));
		}
	} while (Thread32Next(ThreadSnap, &te));

	CloseHandle(ThreadSnap);

	return TRUE;
}

void printError(TCHAR* msg)
{
	_tprintf(TEXT("\nWARNING: %s error\n"), msg);
}
