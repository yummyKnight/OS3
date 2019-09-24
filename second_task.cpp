//
// Created by Admin on 22.09.2019.
//
#include <windows.h>
#include <string>
#include <iostream>

using namespace std;

VOID WINAPI FileWrittenCallback(DWORD dwErrorCode, DWORD dwBytesTransferred, LPOVERLAPPED lpOverlapped) {
    if (dwErrorCode != 0) {
        cout << "CompletionRoutine: Unable to write to file! Error: %u, AddrOverlapped: %p\n" << dwErrorCode
             << lpOverlapped << endl;
    } else {
        cout << "CompletionRoutine: Transferred: %u Bytes, AddrOverlapped: %p\n" << dwBytesTransferred << lpOverlapped
             << endl;
    }
}

HANDLE createFileWrapper(const string &file_name) {

    HANDLE new_handle = CreateFileA(
            file_name.c_str(),              // file name
            GENERIC_READ | GENERIC_WRITE,   //DesiredAccess
            0,                              // share access
            nullptr,                        //SecurityAttributes
            CREATE_ALWAYS,                  //CreationDisposition
            FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,          //FlagsAndAttributes
            nullptr
    );
    cout << "Created new file in " << file_name << endl;
    cout << "Handle address is " << new_handle << endl;
    return new_handle;
}

void readFileWrapper(/*const HANDLE handle, string disk_name*/) {
    DWORD SectorsPerCluster;
    DWORD BytesPerSector;
    BOOL t = GetDiskFreeSpace(
            "h:\\",
            &SectorsPerCluster, // <--
            &BytesPerSector, // <--
            nullptr,
            nullptr
    );
    if (!t) {
        cout << "Error file getting cluster size" << endl;
    }
    cout << SectorsPerCluster * BytesPerSector << endl;
    cout << "Enter file name to read text from it" << endl;
    string file_name;
    cin >> file_name;
    auto info = new char[300];
    HANDLE new_handle = CreateFileA(
            file_name.c_str(),              // file name
            GENERIC_READ,   //DesiredAccess
            0,                              // share access
            nullptr,                        //SecurityAttributes
            OPEN_EXISTING,                  //CreationDisposition
            FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,          //FlagsAndAttributes
            nullptr
    );
    OVERLAPPED overlapped = OVERLAPPED();
    t = ReadFileEx(new_handle, info, SectorsPerCluster * BytesPerSector, &overlapped, nullptr);
    if (!t) {
        cout << "Error file reading file" << endl;
    }
    cout << "File info:" << endl;
    cout << info << endl;

    CloseHandle(new_handle);
    delete[](info);

}

void writeFileWrapper(/*const HANDLE handle, string disk_name*/) {
    DWORD SectorsPerCluster;
    DWORD BytesPerSector;
    BOOL t = GetDiskFreeSpace(
            "h:\\",
            &SectorsPerCluster, // <--
            &BytesPerSector, // <--
            nullptr,
            nullptr
    );
    if (!t) {
        cout << "Error file getting cluster size" << endl;
    }
    cout << SectorsPerCluster * BytesPerSector << endl;
    cout << "Enter file name to write text to it" << endl;
    string file_name;
    cin >> file_name;
    string info;
    HANDLE new_handle = CreateFileA(
            file_name.c_str(),              // file name
            GENERIC_READ | GENERIC_WRITE,   //DesiredAccess
            0,                              // share access
            nullptr,                        //SecurityAttributes
            OPEN_EXISTING,                  //CreationDisposition
            FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,          //FlagsAndAttributes
            nullptr
    );
    OVERLAPPED overlapped;
//    Append info to file
//    overlapped.Offset = 0xFFFFFFFF;
//    overlapped.OffsetHigh = 0xFFFFFFFF;
//
    cout << "Enter info to write:" << endl;
    cin >> info;
    t = WriteFileEx(new_handle, info.c_str(), info.length() * 2, &overlapped,
                    (LPOVERLAPPED_COMPLETION_ROUTINE) FileWrittenCallback);
    if (!t) {
        cout << "Error file writing file" << endl;
    } else {
        cout << "Info wrote in file" << endl;
    }

    CloseHandle(new_handle);
}

int main() {
    writeFileWrapper();
    //readFileWrapper();
}