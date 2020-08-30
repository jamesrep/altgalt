#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <winbase.h>
#include "ads.h"

// Reference:
// https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-backupread



// TODO: implement dump...
void getADS(char *pFilename)
{
	BYTE streamid[sizeof(WIN32_STREAM_ID) + sizeof(TCHAR)];
	HANDLE hFile = CreateFileA(pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		const DWORD DW_STREAMID_SIZE = sizeof(streamid);
		const DWORD DW_INITIAL_COUNT = 6;
		DWORD dwBytesRead = 0;
		LPVOID lpContext = NULL;

		// Read the initial stream bytes
		BOOL bRead = BackupRead(hFile, streamid, DW_STREAMID_SIZE, &dwBytesRead, FALSE, FALSE, &lpContext);


		while (bRead && dwBytesRead > 0)
		{
			LPWIN32_STREAM_ID pStreamId = (LPWIN32_STREAM_ID)streamid;

			if (pStreamId->dwStreamNameSize > 0)
			{
				// Only the first bytes of the stream is read so we need to read the rest as well.
				DWORD dwSizeToAllocate = pStreamId->dwStreamNameSize + sizeof(TCHAR);
				TCHAR* pStreamName = (TCHAR *)malloc(dwSizeToAllocate * sizeof(TCHAR));

				ZeroMemory(pStreamName, dwSizeToAllocate);
				CopyMemory(pStreamName, pStreamId->cStreamName, DW_INITIAL_COUNT);

				DWORD dwBytesToReadForName = pStreamId->dwStreamNameSize - DW_INITIAL_COUNT;
				BYTE *lpBuffer = (BYTE *)malloc(max(dwBytesToReadForName, sizeof(WIN32_STREAM_ID) + 2)); //  buffer size must be greater than the size of WIN32_STREAM_ID

				if (!BackupRead(hFile, lpBuffer, dwBytesToReadForName, &dwBytesRead, FALSE, FALSE, &lpContext))
				{
					printf("[-] Error: ADS found but could not read entire filename\r\n");
					break; // Should never happen
				}
				else
				{
					CopyMemory((pStreamName + 3), lpBuffer, dwBytesRead);
					printf("%ls (%llu)\r\n", pStreamName, pStreamId->Size.QuadPart);

				}

				free(pStreamName);
				free(lpBuffer);
			}

			if (pStreamId->Size.LowPart != 0 || pStreamId->Size.HighPart != 0)
			{
				DWORD dwLowByteSeeked = 0;
				DWORD dwHighByteSeeked = 0;
				BOOL bSeek = BackupSeek(hFile, pStreamId->Size.LowPart, pStreamId->Size.HighPart, &dwLowByteSeeked, &dwHighByteSeeked, &lpContext);

				if (dwLowByteSeeked <= 0 && dwHighByteSeeked <= 0)
				{
					break;
				}
			}

			// Read next
			bRead = BackupRead(hFile, streamid, DW_STREAMID_SIZE, &dwBytesRead, FALSE, FALSE, &lpContext);

		}

		// Clean up. 
		BackupRead(hFile, NULL, 0, &dwBytesRead, TRUE, FALSE, &lpContext);
		CloseHandle(hFile);

		
	}

}

