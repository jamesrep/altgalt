#ifndef _ADS_H
#define _ADS_H

#include <windows.h>

// The structs is from the ddk. Just copies them here for now.

typedef struct _IO_STATUS_BLOCK {
	union {
		NTSTATUS Status;
		PVOID    Pointer;
	} DUMMYUNIONNAME;
	ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

// From https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_stream_information
typedef struct _FILE_STREAM_INFORMATION {
	ULONG         NextEntryOffset;
	ULONG         StreamNameLength;
	LARGE_INTEGER StreamSize;
	LARGE_INTEGER StreamAllocationSize;
	WCHAR         StreamName[1];
} FILE_STREAM_INFORMATION, *PFILE_STREAM_INFORMATION;


typedef NTSTATUS(NTAPI *NTQUERYINFORMATIONFILE)(
	IN  HANDLE                 FileHandle,
	OUT PIO_STATUS_BLOCK       IoStatusBlock,
	OUT PVOID                  FileInformation,
	IN  DWORD                  Length,
	IN  DWORD FileInformationClass

	);


void getADS3(char *pFilename);
void getADS(char *pFilename);



#endif
