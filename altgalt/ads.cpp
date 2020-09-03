#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <winbase.h>

#include "ads.h"

// TODO: Well.... some better error handling and buffer-reads could be nice.

// Reference:
// https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-ntqueryinformationfile


// Function not used but left as an example
LPSTREAMINFO getADSByNtQuery(char *pFilename)
{
	LPSTREAMINFO streamInfo = NULL;
	LPSTREAMINFO baseStream = NULL;
	const DWORD MAX_BUF = 102400;
	HMODULE hNtdll = LoadLibrary(_T("ntdll.dll"));
	NTQUERYINFORMATIONFILE NtQueryInformationFile = (NTQUERYINFORMATIONFILE)GetProcAddress(hNtdll, "NtQueryInformationFile");
	BYTE btsInfoBuffer[MAX_BUF];  // Just test, hence, dont care about buffer issues in this example.
	PFILE_STREAM_INFORMATION pStreamInfo = (PFILE_STREAM_INFORMATION)btsInfoBuffer;
	IO_STATUS_BLOCK ioStatus;
	HANDLE hFile = CreateFileA(pFilename, 0, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	NtQueryInformationFile(hFile, &ioStatus, btsInfoBuffer, sizeof(btsInfoBuffer), 22);
	ULONG currentPos = 0;

	while (pStreamInfo->NextEntryOffset > 0)
	{	
		currentPos += pStreamInfo->NextEntryOffset; 

		if (MAX_BUF < currentPos) break; // Just test, hence, dont care about buffer issues in this example.

		pStreamInfo = (PFILE_STREAM_INFORMATION)(btsInfoBuffer + currentPos);


		if (pStreamInfo->StreamName != NULL)
		{
			if (streamInfo == NULL)
			{
				streamInfo = (LPSTREAMINFO)malloc(sizeof(STREAMINFO));
				baseStream = streamInfo;				
			}
			else
			{
				streamInfo->next = (LPSTREAMINFO)malloc(sizeof(STREAMINFO));
				streamInfo = (LPSTREAMINFO)streamInfo->next;
			}

			streamInfo->next = NULL;			
			lstrcpynW(streamInfo->strStreamName, pStreamInfo->StreamName, min(pStreamInfo->StreamNameLength, MAX_PATH));
			streamInfo->streamLength = pStreamInfo->StreamSize.QuadPart;
		}
	}

	CloseHandle(hFile);
	FreeLibrary(hNtdll);

	return baseStream;
}


void printFirstBytes(wchar_t *pFilename, DWORD byteCount)
{
	HANDLE hFile = CreateFileW(pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
	LPBYTE lpBuffer = (LPBYTE) malloc(byteCount);
	DWORD dwRead = 0;
	BOOL bRead = ReadFile(hFile, lpBuffer, byteCount, &dwRead, NULL);

	if (bRead)
	{
		for (DWORD i = 0; i < dwRead; i++)
		{
			if (isascii(lpBuffer[i])) printf("%c", lpBuffer[i]);			
			else printf(".");			
		}

		printf("    ");

		for (DWORD i = 0; i < dwRead; i++)
		{
			printf("%02x ", lpBuffer[i]);
		}
	}
	else
	{
		printf("[-] error: could not read bytes from file: %ls \r\n", pFilename);
	}

	CloseHandle(hFile);
}
