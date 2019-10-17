//
// Created by Admin on 22.09.2019.
//
#include <windows.h>
#include <winbase.h>
#include <string>
#include <iostream>
#include <malloc.h>
#include <vector>
#include <chrono>
#include <fstream>

using namespace std;
DWORD cluser_size = 4096;
char **info;
DWORD global_i = 0;
DWORD overlapped_operations_count = 16;
VOID WINAPI FileWrittenCallback(DWORD dwErrorCode, DWORD dwBytesTransferred, LPOVERLAPPED lpOverlapped) {
    if (dwErrorCode != 0) {
        cout << "CompletionRoutine: Unable to write to file! Error:" << dwErrorCode << " amount of read bytes: "
             << dwBytesTransferred << ", AddrOverlapped: " << lpOverlapped << endl;
    } else {
//        cout << "CompletionRoutine: Write: " << dwBytesTransferred << " Bytes, AddrOverlapped:" << lpOverlapped << endl;
    }
}

VOID WINAPI CopyCallback(DWORD dwErrorCode, DWORD dwBytesTransferred, LPOVERLAPPED lpOverlapped) {
    if (dwErrorCode != 0) {
        cout << "CompletionRoutine: Unable to write to file! Error:" << dwErrorCode << ", AddrOverlapped: "
             << lpOverlapped << endl;
    } else {
//        cout << "CompletionRoutine: Read: " << dwBytesTransferred << " Bytes, AddrOverlapped: " << lpOverlapped << endl;
        OVERLAPPED overlapped;
        overlapped.Offset = lpOverlapped->Offset;
        overlapped.OffsetHigh = lpOverlapped->OffsetHigh;
        DWORD t = WriteFileEx(lpOverlapped->hEvent, info[global_i], cluser_size, &overlapped,
                              (LPOVERLAPPED_COMPLETION_ROUTINE) FileWrittenCallback);
//        cout << t << endl;
        global_i++;
        // DWORD t1 = WriteFileEx(dst_handle, &str, block_size , &overlapped2, (LPOVERLAPPED_COMPLETION_ROUTINE)FileWrittenCallback);
    }
}

HANDLE createFileWrapper(const string &file_name) {

    HANDLE new_handle = CreateFileA(
            file_name.c_str(),                             // file name
            GENERIC_READ | GENERIC_WRITE,                  //DesiredAccess
            0,                                             // share access
            nullptr,                                       //SecurityAttributes
            CREATE_ALWAYS,                                 //CreationDisposition
            FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED, //FlagsAndAttributes
            nullptr);
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
            &BytesPerSector,    // <--
            nullptr,
            nullptr);
    if (!t) {
        cout << "Error file getting cluster size" << endl;
    }
    cout << SectorsPerCluster * BytesPerSector << endl;
    cout << "Enter file name to read text from it" << endl;
    string file_name = "../vasya.txt";
    //    cin >> file_name;
    HANDLE new_handle = CreateFileA(
            file_name.c_str(),                             // file name
            GENERIC_READ,                                  //DesiredAccess
            0,                                             // share access
            nullptr,                                       //SecurityAttributes
            OPEN_EXISTING,                                 //CreationDisposition
            FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED, //FlagsAndAttributes
            nullptr);
    OVERLAPPED overlapped = OVERLAPPED();
    auto l = SectorsPerCluster * BytesPerSector;
    char *str = static_cast<char *>(VirtualAlloc(nullptr, l, MEM_COMMIT, PAGE_READWRITE));
    t = ReadFileEx(new_handle, str, l, &overlapped, FileWrittenCallback);
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
            &BytesPerSector,    // <--
            nullptr,
            nullptr);
    if (!t) {
        cout << "Error file getting cluster size" << endl;
    }
    cout << SectorsPerCluster * BytesPerSector << endl;
    cout << "Enter file name to write text to it" << endl;
    string file_name = "../vasya.txt";
    //    cin >> file_name;

    string info;

    HANDLE new_handle = CreateFileA(
            file_name.c_str(),                                                     // file name
            GENERIC_WRITE | GENERIC_READ,                                          //DesiredAccess
            FILE_SHARE_READ | FILE_SHARE_WRITE,                                    // share access
            nullptr,                                                               //SecurityAttributes
            OPEN_EXISTING,                                                         //CreationDisposition
            FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED | FILE_ATTRIBUTE_NORMAL, //FlagsAndAttributes
            nullptr);
    OVERLAPPED overlapped;
    //    overlapped.hEvent = new_handle;
    //    Append info to file
    overlapped.Offset = 0xFFFFFFFF;
    overlapped.OffsetHigh = 0xFFFFFFFF;
    //
    DWORD buffer;
    const unsigned SECTOR_SIZE = SectorsPerCluster * BytesPerSector;
    auto br = string("My county is super duper\r\n");
    char buf[102400];
    //    char *str = static_cast<char *>(VirtualAlloc(nullptr, br.length(), MEM_COMMIT, PAGE_READWRITE));
    ZeroMemory(&buf, br.length());

    //
    //    int counter = 0;
    //    for (auto i : br) {
    //        str[counter++] = i;
    //    }
    for (char &i : buf) {
        i = 'M';
    }

    cout << new_handle << endl;

    //    char *p = (char *) ((DWORD) (buf + SECTOR_SIZE - 1) & ~(SECTOR_SIZE - 1));
    //    string str = "My county is super duper";
    //    t = SetFilePointerEx(new_handle,)
    //    if (!t) {
    //        cout << GetLastError() << endl;
    //        cout << "Error file writing file" << endl;
    //    }
    cout << br.length() << endl;
    t = WriteFileEx(new_handle, &buf, 102400, &overlapped, (LPOVERLAPPED_COMPLETION_ROUTINE) FileWrittenCallback);
    std::cout << GetLastError() << '\n';
    //    if (GetOverlappedResult(new_handle, &overlapped, &buffer, TRUE)) {
    //        cout << "Overlapped operation successful ending. Size of buffer is " << buffer << endl;
    //    }
    if (SleepEx(INFINITE, TRUE) == WAIT_IO_COMPLETION) {
        cout << "Its fine\n";
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
int BlockCopying(const string &src, const string &dst) {
    HANDLE src_handle = CreateFileA(
            src.c_str(),                                                           // file name
            GENERIC_READ,                                                          //DesiredAccess
            FILE_SHARE_READ,                                                       // share access
            nullptr,                                                               //SecurityAttributes
            OPEN_EXISTING,                                                         //CreationDisposition
            FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED | FILE_ATTRIBUTE_NORMAL, //FlagsAndAttributes
            nullptr);
    if (src_handle == INVALID_HANDLE_VALUE) {
        cout << "Invalid src file" << endl;
        return -1;
    }
    HANDLE dst_handle = CreateFileA(
            dst.c_str(),                                                           // file name
            GENERIC_WRITE,                                                         //DesiredAccess
            FILE_SHARE_WRITE,                                                      // share access
            nullptr,                                                               //SecurityAttributes
            OPEN_EXISTING,                                                         //CreationDisposition
            FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED | FILE_ATTRIBUTE_NORMAL, //FlagsAndAttributes
            nullptr);

    if (dst_handle == INVALID_HANDLE_VALUE) {
        cout << "Invalid dst file" << endl;
        return -1;
    }

    DWORD src_size = GetFileSize(src_handle, NULL);
    const DWORD overlapped_count = src_size == cluser_size ? src_size / cluser_size : src_size / cluser_size + 1;
    vector<OVERLAPPED> overlapped_vector(overlapped_count);

    for (DWORD i = 0; i < overlapped_count; i++) {
        overlapped_vector.at(i).hEvent = dst_handle;
    }

    cout << "Src size: " << src_size << " Overlapped_count: " << overlapped_count << endl;
    info = new char *[overlapped_count];

    for (size_t i = 0; i < overlapped_count; i++) {
        info[i] = new char[cluser_size];
        ZeroMemory(info[i], cluser_size);
    }

    LARGE_INTEGER cur_pos;
    DWORD t_overlapped_operations_count;


    if (!overlapped_operations_count) {
        t_overlapped_operations_count = overlapped_count - 1;
        overlapped_operations_count = overlapped_count;
    } else {
        t_overlapped_operations_count = overlapped_operations_count - 1;
    }
    int sync_times = 0;
    auto start = chrono::high_resolution_clock::now();
    for (size_t i = 0; i < overlapped_count; i++) {
        cur_pos.QuadPart = i * cluser_size;
        overlapped_vector.at(i).Offset = cur_pos.LowPart;
        overlapped_vector.at(i).OffsetHigh = cur_pos.HighPart;
        DWORD t = ReadFileEx(src_handle, info[i], cluser_size, &overlapped_vector.at(i),
                             (LPOVERLAPPED_COMPLETION_ROUTINE) CopyCallback);
//        SleepEx(INFINITE, TRUE);
        if (i == t_overlapped_operations_count) {
            sync_times++;
            t_overlapped_operations_count += overlapped_operations_count;
            SleepEx(INFINITE, TRUE);
        }
    }
    if (sync_times * overlapped_operations_count < overlapped_count && overlapped_operations_count != 0) {
        sync_times++;
        SleepEx(INFINITE, TRUE);
    }
    auto end = chrono::high_resolution_clock::now();
    cout << "Number of operations that needed extra sync: "
         << overlapped_count - (sync_times - 1) * overlapped_operations_count << endl;
    // for (size_t i = 0; i < overlapped_count; i++)
    // {
    //     cout << info[i] << endl;
    // }
    auto time = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
    cout << "src file size (kb): " << src_size / 1024 << endl;
    cout << "dst file size (kb): " << GetFileSize(dst_handle, NULL) / 1024 << endl;
    cout << "Time (in seconds): " << time << endl;
    cout << "Speed ( kb in seconds ): " << (src_size / time / 1024) << endl;
    cout << "Amount of syncs: " << sync_times << endl;
    //    for (DWORD i = 0; i < src_size; i = i + block_size) {
    //
    //    }
    return 1;
}

int main() {
    std::ofstream ofs;
    ofs.open("../vasya.txt", std::ofstream::out | std::ofstream::trunc);
    ofs.close();
    BlockCopying("../dataset/256_000", "../vasya.txt");
    //    writeFileWrapper();
    //    readFileWrapper();
}
