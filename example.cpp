#include <iostream>
#include <windows.h>
#include <cstdio>
#include <tchar.h>
using namespace std;

VOID WINAPI FileWrittenCallback(DWORD dwErrorCode, DWORD dwBytesTransferred, LPOVERLAPPED lpOverlapped) {
    if (dwErrorCode != 0) {
        fprintf(stdout, "CompletionRoutine: Unable to write to file! Error: %u, AddrOverlapped: %p\n", dwErrorCode, lpOverlapped);
    }
    else {
        fprintf(stdout, "CompletionRoutine: Transferred: %u Bytes, AddrOverlapped: %p\n", dwBytesTransferred, lpOverlapped);
    }
}

int main()
{
    HANDLE hFile = CreateFile(_T("../tram.txt"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    if (INVALID_HANDLE_VALUE == hFile) {
        fprintf(stdout, "Could not create file! Error %u\n", GetLastError());
        return -1;
    }

    OVERLAPPED oFile;
    oFile.Offset = 0xFFFFFFFF;
    oFile.OffsetHigh = 0xFFFFFFFF;

    string text = _T("Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy\r\n");
    if (!WriteFileEx(hFile, text.c_str(), text.length()* 2, &oFile, (LPOVERLAPPED_COMPLETION_ROUTINE)FileWrittenCallback)) {
        fprintf(stdout, "Unable to write to file! Error %u\n", GetLastError());
    }
    CloseHandle(hFile);
    return 0;
}