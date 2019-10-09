//
// Created by Admin on 22.09.2019.
//
#include <windows.h>
#include <string>
#include <iostream>
#include <malloc.h>

using namespace std;

VOID WINAPI FileWrittenCallback(DWORD dwErrorCode, DWORD dwBytesTransferred, LPOVERLAPPED lpOverlapped) {
    if (dwErrorCode != 0) {
        cout << "CompletionRoutine: Unable to write to file! Error:" << dwErrorCode << ", AddrOverlapped: " <<
             lpOverlapped << endl;
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
            "c:\\",
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
    auto l = SectorsPerCluster * BytesPerSector;
    char *str = static_cast<char *>(VirtualAlloc(nullptr, l * 3, MEM_COMMIT, PAGE_READWRITE));
    t = ReadFileEx(new_handle, str, l * 2, &overlapped, FileWrittenCallback);
    DWORD buffer;
    if (GetOverlappedResult(new_handle, &overlapped, &buffer, TRUE)) {
        cout << "Overlapped operation successful ending" << endl;
    }
    if (!t) {
        cout << "Error file reading file" << endl;
    }
    cout << "File info:" << endl;
    cout << string(str) << endl;

    CloseHandle(new_handle);
    if (GetLastError() == ERROR_SUCCESS) {
        cout << "All success" << endl;
    }
    CloseHandle(new_handle);

}

void writeFileWrapper(/*const HANDLE handle, string disk_name*/) {
    DWORD SectorsPerCluster;
    DWORD BytesPerSector;
    BOOL t = GetDiskFreeSpace(
            "c:\\",
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
            GENERIC_WRITE | GENERIC_READ,   //DesiredAccess
            0,                              // share access
            nullptr,                        //SecurityAttributes
            OPEN_EXISTING,                  //CreationDisposition
            FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED | FILE_ATTRIBUTE_NORMAL,          //FlagsAndAttributes
            nullptr
    );
    OVERLAPPED overlapped;
//    overlapped.hEvent = new_handle;
//    Append info to file
    overlapped.Offset = 0xFFFFFFFF;
    overlapped.OffsetHigh = 0xFFFFFFFF;
//
    DWORD buffer;
    const unsigned SECTOR_SIZE = SectorsPerCluster * BytesPerSector;
    char buf[] = "Vasya Pupkin";

//    char *str = static_cast<char *>(VirtualAlloc(nullptr, SECTOR_SIZE, MEM_COMMIT, PAGE_READWRITE));
//
//    int counter = 0;
//    for (auto i : string("My county is super duper")) {
//        str[counter++] = i;
//    }

    cout << new_handle << endl;
    char *p = (char *) ((DWORD) (buf + SECTOR_SIZE - 1) & ~(SECTOR_SIZE - 1));
//    string str = "My county is super duper";
//    t = SetFilePointerEx(new_handle,)
//    if (!t) {
//        cout << GetLastError() << endl;
//        cout << "Error file writing file" << endl;
//    }
    t = WriteFileEx(new_handle, &p, SECTOR_SIZE, &overlapped, FileWrittenCallback);
    std::cout << GetLastError() << '\n';
    if (GetOverlappedResult(new_handle, &overlapped, &buffer, TRUE)) {
        cout << "Overlapped operation successful ending" << endl;
    }

    if (!t) {
        cout << "Error file writing file" << endl;
    } else {
        cout << "Info wrote in file" << endl;
    }

    if (GetLastError() == ERROR_SUCCESS) {
        cout << "All success" << endl;
    }
    CloseHandle(new_handle);
}

// special function for async i/0


int main() {
    writeFileWrapper();

    //readFileWrapper();
}
