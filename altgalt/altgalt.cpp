// James Dickson 2020 - Just a quick tool for dumping Alternate Data Streams.
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <winbase.h>
#include "ads.h"


int main(int argc, char **argv)
{
	char pFilename[MAX_PATH];	// Will get the filename
	BOOL bDump = FALSE;			// Do not dump by default.
	DWORD dumpCount = 10;		// Dump 10 bytes as default

	ZeroMemory(pFilename, MAX_PATH);
	printf("altgalt - yet another ads tool - james dickson 2020\r\n");

	// Just do the old argument parser routine
	for (int i = 0; i < argc; i++)
	{
		if (strcmp(argv[i], "--dump") == 0)
		{
			printf("[+] Should try to dump a few bytes of each ADS\r\n");
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
			else if (strcmp(argv[i], "--count") == 0)
			{
				i++;
				dumpCount = atoi(argv[i]);
				printf("[+] Will dump: %d first bytes\r\n", dumpCount);
			}
		}
	}

	printf("\r\n");

	// Here we go
	if (strlen(pFilename) != 0)
	{
		LPSTREAMINFO streamInfo = getADSByNtQuery(pFilename);

		// Enumerate the streams, list info and clean the memory.
		while (streamInfo != NULL)
		{
			printf("%ls (%llu)\r\n", streamInfo->strStreamName, streamInfo->streamLength);

			if (bDump)
			{
				// No, dont want string libraries with this small tool.
				wchar_t* lpName = wcsstr(streamInfo->strStreamName, L":");

				if (lpName != NULL)
				{
					lpName = lpName + 1;
					wchar_t* lpNameLast = wcschr(lpName, L':');

					if (lpNameLast != NULL)
					{
						__int64 index = lpNameLast - lpName;

						if (index > 0)
						{
							wchar_t lpFinished[MAX_PATH * 2];
							wchar_t lpFinishedFull[MAX_PATH * 2];
							wcsncpy_s(lpFinished, lpName, min(index, MAX_PATH));

							wchar_t strOriginalFile[MAX_PATH * 2];
							size_t tconverted = 0;
							mbstowcs_s(&tconverted, strOriginalFile, (const char*)pFilename, MAX_PATH);

							wcsncpy_s(lpFinishedFull, strOriginalFile, MAX_PATH);
							wcscat_s(lpFinishedFull, L":");
							wcscat_s(lpFinishedFull, lpFinished);

							// Ok. Just print the first bytes in the file
							printFirstBytes(lpFinishedFull, dumpCount);
							printf("\r\n\r\n");
						}
					}
				}
			}

			// Get next item and free memory
			LPSTREAMINFO temp = (LPSTREAMINFO)streamInfo->next;
			free(streamInfo);
			streamInfo = temp;
		}

	}
	else
	{
		printf("[-] Bad number of arguments.\r\nUsage: altgalt.exe --file <filename> --dump --count 16\r\n");
		return -1;
	}

    return 0;
}

