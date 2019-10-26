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
const bool debug = false;
DWORD cluster_size = 4096;
char **info;
DWORD global_i = 0;
DWORD overlapped_operations_count = 0;
long long write_complete = 0;
VOID WINAPI FileWrittenCallback(DWORD dwErrorCode, DWORD dwBytesTransferred, LPOVERLAPPED lpOverlapped) {
    if (dwErrorCode != 0) {
        cout << "CompletionRoutine: Unable to write to file! Error:" << dwErrorCode << " amount of read bytes: "
             << dwBytesTransferred << ", AddrOverlapped: " << lpOverlapped << endl;
    } else {
//        cout << "CompletionRoutine: Write: " << dwBytesTransferred << " Bytes, AddrOverlapped:" << lpOverlapped << endl;
    }
    write_complete++;
}

VOID WINAPI CopyCallback(DWORD dwErrorCode, DWORD dwBytesTransferred, LPOVERLAPPED lpOverlapped) {
    if (dwErrorCode != 0) {
        cout << "CompletionRoutine: Unable to read from file! Error:" << dwErrorCode << ", AddrOverlapped: "
             << lpOverlapped << endl;
    } else {
//        cout << "CompletionRoutine: Read: " << dwBytesTransferred << " Bytes, AddrOverlapped: " << lpOverlapped << endl;
        OVERLAPPED overlapped;
        overlapped.Offset = lpOverlapped->Offset;
        overlapped.OffsetHigh = lpOverlapped->OffsetHigh;

//            cout << "global i " << global_i << endl;
        WriteFileEx(lpOverlapped->hEvent, info[global_i], cluster_size, &overlapped,
                (LPOVERLAPPED_COMPLETION_ROUTINE) FileWrittenCallback);
        //if(debug) {cout << "Write t codes: " << t << endl;}
//        cout << t << endl;
        global_i++;
        // DWORD t1 = WriteFileEx(dst_handle, &str, block_size , &overlapped2, (LPOVERLAPPED_COMPLETION_ROUTINE)FileWrittenCallback);
    }
}
// special function for async i/0
void BlockCopying(const string &src, const string &dst) {

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
        return;
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
        return;
    }

    __int64 src_size = GetFileSize(src_handle, NULL);

    const DWORD overlapped_count = src_size == cluster_size ? src_size / cluster_size : src_size / cluster_size + 1;
    vector<OVERLAPPED> overlapped_vector(overlapped_count);

    for (DWORD i = 0; i < overlapped_count; i++) {
        overlapped_vector.at(i).hEvent = dst_handle;
    }
    info = (char**) calloc(overlapped_count, sizeof(char*));
    cout << "Src size: " << src_size << " Overlapped_count: " << overlapped_count << endl;
    for (size_t i = 0; i < overlapped_count; i++) {
        info[i] = (char*) calloc(cluster_size , sizeof(char*));
//        ZeroMemory(info[i], cluster_size);
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
        cur_pos.QuadPart = i * cluster_size;
        overlapped_vector.at(i).Offset = cur_pos.LowPart;
        overlapped_vector.at(i).OffsetHigh = cur_pos.HighPart;
        DWORD t = ReadFileEx(src_handle, info[i], cluster_size, &overlapped_vector.at(i),
                             (LPOVERLAPPED_COMPLETION_ROUTINE) CopyCallback);
        if(!t){
            cout << "Error while reading file" << endl;
            if (GetLastError() == ERROR_HANDLE_EOF){
                cout << "EOF!!!" << endl;
            }
        }
//        SleepEx(INFINITE, TRUE);
        if(debug){cout << " Read t codes :" << t << endl;}
        if (i == t_overlapped_operations_count) {
            sync_times++;
            t_overlapped_operations_count += overlapped_operations_count;
            SleepEx(INFINITE, TRUE);
//            cout << "Sync:" << sync_times << endl;
        }
    }
    if (sync_times * overlapped_operations_count < overlapped_count && overlapped_operations_count != 0) {
        if(debug) {cout << "Extra syncs:" << endl;}
        sync_times++;
        SleepEx(INFINITE, TRUE);
    }
    auto end = chrono::high_resolution_clock::now();
    while (write_complete != overlapped_count){
        SleepEx(100,TRUE);
    }
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
    cout << "Writes complete :" << write_complete << endl;
//    cout << info[overlapped_count - 1] << endl;
//    cout << endl;
    CloseHandle(src_handle);
    CloseHandle(dst_handle);
    for (size_t i = 0; i < overlapped_count; i++) {
        free(info[i]);
    }
    free(info);
}
void experiment(const string &src){
    std::ofstream ofs;
    ofs.open("timon.txt", std::ofstream::out | std::ofstream::trunc);
    ofs.close();
    BlockCopying(src, "timon.txt");
}
int main() {
    vector<string> v = {"src.txt"};
    vector<DWORD > v_int = {1,2,4,8,12,16,32};
//    vector<DWORD > v_int = {1};
//    cluster_size = 4096;
    system("pause");
    overlapped_operations_count = 0;
    for (auto oc : v_int) {
        overlapped_operations_count = oc;
//        for (cluster_size = 24576; cluster_size < 32769; cluster_size += 4096) {
            for (const auto& a : v) {
                cout << "Cluster size: " << cluster_size << " Number of async operation: " << oc << endl;
                global_i = 0;
                write_complete = 0;
                experiment(a);
            }
            cout << endl;
//        }
    }
    system("pause");
//    BlockCopying("../dataset/100000", "../vasya.txt");
    //    writeFileWrapper();
    //    readFileWrapper();
}
