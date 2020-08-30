// James Dickson 2020 - Just a quick tool for dumping Alternate Data Streams.
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <winbase.h>
#include "ads.h"


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
		getADS(pFilename);
		
	}
	else
	{
		printf("[-] Bad number of arguments.\r\nUsage: altgalt.exe --file <filename> --dump\r\n");
		return -1;
	}

    return 0;
}

