// James Dickson 2020 - Just a quick tool for dumping Alternate Data Streams.
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <winbase.h>

// Reference:
// https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-backupread


// TODO: implement dump...
void printADS(char *pFilename, BOOL bDump)
{
	BYTE streamid[sizeof(WIN32_STREAM_ID) + sizeof(TCHAR)]; 
	HANDLE hFile = CreateFileA(pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		const DWORD dwBytesToRead = sizeof(streamid);
		DWORD dwBytesRead = 0;
		LPVOID lpContext = NULL;

		// Read the initial stream bytes
		BOOL bRead = BackupRead(hFile, streamid, dwBytesToRead, &dwBytesRead, FALSE, FALSE, &lpContext);

		while (bRead && dwBytesRead)
		{
			LPWIN32_STREAM_ID pStreamId = (LPWIN32_STREAM_ID)streamid;

			if (pStreamId->dwStreamNameSize > 0)
			{
				// Only the first bytes of the stream is read so we need to read the rest as well.
				DWORD dwSizeToAllocate = pStreamId->dwStreamNameSize + sizeof(TCHAR);
				TCHAR* pStreamName = (TCHAR *)malloc(dwSizeToAllocate * sizeof(TCHAR)); 
				DWORD dwBytesAlreadyRead = 6;

				ZeroMemory(pStreamName, dwSizeToAllocate);
				CopyMemory(pStreamName, pStreamId->cStreamName, dwBytesAlreadyRead);

				DWORD dwBytesToReadForName = pStreamId->dwStreamNameSize - dwBytesAlreadyRead;
				BYTE lpBuffer[dwBytesToRead]; 

				if (!BackupRead(hFile, lpBuffer, dwBytesToReadForName, &dwBytesRead, FALSE, FALSE, &lpContext))
				{
					printf("[-] Error: ADS found but could not read entire filename\r\n");
					break; // Should never happen
				}
				else
				{
					CopyMemory((pStreamName + 3), lpBuffer, dwBytesRead);
					printf("[%d] %ls \r\n", pStreamId->dwStreamId, pStreamName);					
				}

				free(pStreamName);				
			}

			if (pStreamId->Size.LowPart != 0 || pStreamId->Size.HighPart != 0)
			{
				DWORD dwLowByteSeeked = 0;
				DWORD dwHighByteSeeked = 0;
				BOOL bSeek = BackupSeek(hFile, pStreamId->Size.LowPart, 0, &dwLowByteSeeked, &dwHighByteSeeked, &lpContext);

				if (dwLowByteSeeked <= 0)
				{
					break;
				}
			}

			// Read next
			bRead = BackupRead(hFile, streamid, dwBytesToRead, &dwBytesRead, FALSE, FALSE, &lpContext);
		}
		
		// Clean up. 
		BackupRead(hFile, NULL, 0, &dwBytesRead, TRUE, FALSE, &lpContext);
		CloseHandle(hFile);
	}
}

int main(int argc, char **argv)
{
	char pFilename[MAX_PATH];
	BOOL bDump = FALSE; // Do not dump by default.

	ZeroMemory(pFilename, MAX_PATH);

	printf("altgalt - yet another ads tool - james dickson 2020\r\n");

	// Just do the old argument parser routine
	for (int i = 0; i < argc; i++)
	{
		if (strcmp(argv[i], "--dump") == 0)
		{
			printf("[+] Should try to dump ADS in the file\r\n");
			bDump = TRUE;
		}
		else if ((i + 1) < argc)
		{
			if (strcmp(argv[i], "--file") == 0)
			{
				i++;
				strncpy_s(pFilename, argv[i], MAX_PATH);

				printf("[+] File selected: %s\r\n", pFilename);
			}
		}

	}

	// Here we go
	if (strlen(pFilename) != 0)
	{
		printADS(pFilename, bDump);
	}
	else
	{
		printf("[-] Bad number of arguments.\r\nUsage: altgalt.exe --file <filename> --dump\r\n");
		return -1;
	}

    return 0;
}

