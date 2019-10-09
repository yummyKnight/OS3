//
// Created by Admin on 09.10.2019.
//
#include <winioctl.h>
#include <iostream>
#include <windows.h>
#include <cstdio>
#include <assert.h>

#define _countof(array) (sizeof(array) / sizeof(array[0]))
using namespace std;

namespace{

    unsigned long tempBytesWritten = 0;
    HANDLE m_fileHandle;
    char m_pszFilename[_MAX_PATH] = "";

    // Create a temporary file for benchmark
    int createFile()
    {
        WCHAR tempPath[MAX_PATH];
        GetTempPath(_countof(tempPath), tempPath);
        _bstr_t p(tempPath);
        const char* c = p;
        strcpy(m_pszFilename, c);
        strcat(m_pszFilename, "testRawFile.raw");
        cout << "Writing to " << m_pszFilename << endl;

        m_fileHandle = CreateFileA(
                m_pszFilename,
                GENERIC_WRITE,
                0,
                NULL,
                CREATE_ALWAYS,
                FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH,
                NULL);

        if (m_fileHandle == INVALID_HANDLE_VALUE)
        {
            assert( false );
        }
        return 0;
    }
}

DWORD DetectSectorSize( WCHAR * devName, PSTORAGE_ACCESS_ALIGNMENT_DESCRIPTOR pAlignmentDescriptor)
{
    DWORD                  Bytes   = 0;
    BOOL                   bReturn = FALSE;
    DWORD                  Error   = NO_ERROR;
    STORAGE_PROPERTY_QUERY Query;

    ZeroMemory(&Query, sizeof(Query));

    HANDLE  hFile = CreateFileW( devName,
                                 STANDARD_RIGHTS_READ,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE,
                                 NULL,
                                 OPEN_EXISTING,
                                 FILE_ATTRIBUTE_NORMAL,
                                 NULL);

    if (hFile==INVALID_HANDLE_VALUE) {
        wprintf(L"  hFile==INVALID_HANDLE_VALUE. GetLastError() returns %lu.\n", Error=GetLastError());
        return Error;
    }

    Query.QueryType  = PropertyStandardQuery;
    Query.PropertyId = StorageAccessAlignmentProperty;

    bReturn = DeviceIoControl( hFile,
                               IOCTL_STORAGE_QUERY_PROPERTY,
                               &Query,
                               sizeof(STORAGE_PROPERTY_QUERY),
                               pAlignmentDescriptor,
                               sizeof(STORAGE_ACCESS_ALIGNMENT_DESCRIPTOR),
                               &Bytes,
                               NULL);

    if (bReturn == FALSE) {
        wprintf(L"  bReturn==FALSE. GetLastError() returns %lu.\n", Error=GetLastError());
    }

    CloseHandle(hFile);
    return Error;

}


int main()
{
    unsigned int dataSize = 2000;

    DWORD                               Error     = NO_ERROR;
    STORAGE_ACCESS_ALIGNMENT_DESCRIPTOR Alignment = {0};
    //  WCHAR                               szDisk[]  = L"\\\\.\\PhysicalDrive0";
    WCHAR                               szDisk[]  = L"\\\\.\\C:";

    Error = DetectSectorSize(szDisk, &Alignment);
    if (Error) {
        wprintf(L"Error %lu encountered while querying alignment.\n", Error);
        return Error;
    }

    wprintf(L"Disk %s Properties\n", (WCHAR*) szDisk);

    if (Alignment.BytesPerLogicalSector < Alignment.BytesPerPhysicalSector) {
        wprintf(L"  Emulated sector size is %lu bytes.\n", Alignment.BytesPerLogicalSector);
    }

    wprintf(L"  Physical sector size is %lu bytes.\n", Alignment.BytesPerPhysicalSector);

    dataSize = ((unsigned int)(dataSize + Alignment.BytesPerPhysicalSector - 1)/Alignment.BytesPerPhysicalSector) * Alignment.BytesPerPhysicalSector;

    // Allocate buffer for file
    auto *buffer = new unsigned char[dataSize];

    // Create file to write to
    if ( createFile() != 0 )
    {
        printf("There was error creating the files... press Enter to exit.");
        getchar();
        return -1;
    }

    const BOOL bSuccess  = WriteFile(m_fileHandle, buffer, dataSize, &tempBytesWritten, NULL );

    if (!bSuccess)
    {
        cout << "Write failed with error " << GetLastError() << endl;
    }

    // clean up and remove file
    CloseHandle(m_fileHandle);
    wchar_t wtext[_MAX_PATH];
    mbstowcs(wtext, m_pszFilename, strlen(m_pszFilename)+1);
    DeleteFile(wtext);

    return 0;
}
