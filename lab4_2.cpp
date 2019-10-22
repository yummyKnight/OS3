//
// Created by Admin on 21.10.2019.
//
#include <windows.h>
#include <cstdio>
#include <conio.h>
#include <tchar.h>
#include <iostream>
TCHAR szName[]=TEXT("Local\\MyFileMappingObject");
TCHAR fileName[]=TEXT("../timon.txt");
using namespace std;
int main()
{
    LPCTSTR pBuf;

    HANDLE file_handle = CreateFileA(
            fileName,                           // file name
            GENERIC_READ | GENERIC_WRITE,                  //DesiredAccess
            0,                                             // share access
            nullptr,                                       //SecurityAttributes
            OPEN_EXISTING,                                 //CreationDisposition
            FILE_ATTRIBUTE_NORMAL, //FlagsAndAttributes
            nullptr);
    if(file_handle == INVALID_HANDLE_VALUE){
        printf("Invalid handle");
    }
    HANDLE mapping_handle = CreateFileMapping(file_handle,
                                              NULL,
                                              PAGE_READWRITE,
                                              0,
                                              BUFSIZ,
                                              szName);


    if (mapping_handle == NULL)
    {
        _tprintf(TEXT("Could not create file mapping object (%d).\n"),
                 GetLastError());
        return 1;
    }
    string info;
    cout << "Enter string to write to file. (256 characters max)" << endl;
    cin >> info;
    char msg[BUFSIZ];
    ZeroMemory(msg,BUFSIZ);
    strcpy(msg, info.c_str());
    pBuf = (LPTSTR) MapViewOfFile(mapping_handle,   // handle to map object
                                  FILE_MAP_ALL_ACCESS, // read/write permission
                                  0,
                                  0,
                                  BUFSIZ);

    if (pBuf == NULL)
    {
        _tprintf(TEXT("Could not map view of file (%d).\n"),
                 GetLastError());

        CloseHandle(mapping_handle);

        return 1;
    }


    CopyMemory((PVOID)pBuf, msg, BUFSIZ);
    CloseHandle(file_handle);
    cout << "Waiting another program " << endl;
    system("pause");
    UnmapViewOfFile(pBuf);
    CloseHandle(mapping_handle);

    return 0;
}
