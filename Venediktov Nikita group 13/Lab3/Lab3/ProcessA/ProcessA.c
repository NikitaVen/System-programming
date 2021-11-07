#include <windows.h>
#include <stdio.h>
#include <TlHelp32.h>

#define MAXLENGTH 101
char* processB_name = "ProcessB.exe";
LPCTSTR pipename = "\\\\.\\pipe\\myPipe";


DWORD GetProcessID(char* process_name)
{
	HANDLE Snapshot;
	if ((Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)) == INVALID_HANDLE_VALUE)
		return 0;

	PROCESSENTRY32 processEntry;
	processEntry.dwSize = sizeof(processEntry);

	Process32First(Snapshot, &processEntry);
	do {
		if (strcmp(processEntry.szExeFile, process_name) == 0)
		{
			CloseHandle(Snapshot);
			return processEntry.th32ProcessID;
		}
	} while (Process32Next(Snapshot, &processEntry));

	CloseHandle(Snapshot);
	return 0;
}
int Fail(HANDLE* processB, HANDLE* pipe)
{
	printf("Failed!\n");
	if (processB != NULL)
		CloseHandle(*processB);
	if (pipe != NULL)
		CloseHandle(*pipe);
	system("pause");
	return -1;
}

char* GetString(DWORD size)
{
	char* str = (char*)malloc(size);
	gets_s(str, MAXLENGTH - 1);
	return str;
}


void main(void)
{
	HANDLE hProcessB;
	HANDLE hPipe;
	char* str;
	DWORD strSize;
	LPVOID memory_Address_B, buffer;
	
	//getting string
	printf("Enter the string: ");
	strSize = sizeof(char) * MAXLENGTH;
	str = GetString(strSize);

	//process B opening
	printf("Attempt to open Process B: ");
	hProcessB = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetProcessID(processB_name));
	if (hProcessB == INVALID_HANDLE_VALUE)
		return Fail(NULL, NULL);
	printf("Success!\n");

	//memory allocaing
	memory_Address_B = VirtualAllocEx(hProcessB, 0, strSize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	printf("Memory in process B is alocated\n");

	//string writing 
	printf("Attempt to write the string in Process B: ");
	if (WriteProcessMemory(hProcessB, memory_Address_B, str, strSize, NULL) == FALSE)
		return Fail(&hProcessB, NULL);
	printf("Success!\n");

	//pipe connecting
	printf("Attempt to connect to the named pipe: ");
    hPipe = CreateFile(pipename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hPipe == INVALID_HANDLE_VALUE)
		return Fail(&hProcessB, NULL);
	printf("Success!\n");

	//writing memory address
	printf("Attempt to write memory address: ");
	buffer = memory_Address_B;
	if (WriteFile(hPipe, &buffer, sizeof(buffer), NULL, NULL) == FALSE)
		return Fail(&hProcessB, &hPipe);
	printf("Success!\n");

	//getting answer from process B
	printf("Attempt to get answer from process B: ");
	if (ReadFile(hPipe, &buffer, sizeof(buffer), NULL, NULL) == FALSE)
		return Fail(&hProcessB, &hPipe);
	printf("Success!\n");

	//releasing memory of A and B
	printf("Attempt to release memory in process B: ");
	free(str);
	if (VirtualFreeEx(hProcessB, memory_Address_B, 0, MEM_RELEASE) == FALSE)
		return Fail(&hProcessB, NULL);
	printf("Success!\n");

	CloseHandle(hPipe);
	CloseHandle(hProcessB);

	printf("process A is finished\n");
	system("pause");
	return 0;
}