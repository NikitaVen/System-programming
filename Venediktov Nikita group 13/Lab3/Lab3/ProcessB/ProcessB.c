#include <windows.h>
#include <stdio.h>

LPCTSTR pipename = "\\\\.\\pipe\\myPipe";

int Fail(HANDLE* pipe)
{
	printf("Failed!\n");
	system("pause");
	if (pipe != NULL)
		CloseHandle(*pipe);
	return -1;
}
int main(void)
{
	DWORD strAddress;
	HANDLE hPipe;
	//creating pipe
	printf("Attempt to create pipe: ");
	hPipe = CreateNamedPipe(pipename, PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES,
		sizeof(DWORD), sizeof(DWORD), 1, NULL);
	if (hPipe == INVALID_HANDLE_VALUE)
		return Fail(NULL);
	printf("Success!\n");

	//connecting to the pipe
	printf("Connecting to pipe...");
	ConnectNamedPipe(hPipe, NULL);
	printf("Connected!\n");

	//reading the string
	printf("Attempt to read the string from the pipe: ");
	if (ReadFile(hPipe, &strAddress, sizeof(strAddress), NULL, NULL) == FALSE)
		return Fail(&hPipe);
	printf("Success!\n");

	printf("The string: %s\n", (char*)strAddress);

	//answering process A
	printf("Attempt to answer Process A: ");
	if (WriteFile(hPipe, &strAddress, sizeof(strAddress), NULL, NULL) == FALSE)
		return Fail(&hPipe);
	printf("Success!\n");

	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);
	printf("Process B is finished\n");
	system("pause");
	return 0;
}