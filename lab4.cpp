//
// Created by Admin on 19.10.2019.
//
#include <windows.h>
#include <winbase.h>
#include <string>
#include <iostream>
#include <malloc.h>
#include <vector>
#include <map>
#include <bitset>
using namespace std;
map<string , DWORD > protectionOptions = {
        {"1: PAGE_EXECUTE", PAGE_EXECUTE },
        {"2: PAGE_EXECUTE_READ", PAGE_EXECUTE_READ },
        {"3: PAGE_EXECUTE_READWRITE", PAGE_EXECUTE_READWRITE },
        {"4: PAGE_EXECUTE_WRITECOPY", PAGE_EXECUTE_WRITECOPY},
        { "5: PAGE_NOACCESS", PAGE_NOACCESS},
        {"6: PAGE_READONLY", PAGE_READONLY},
        {"7: PAGE_READWRITE", PAGE_READWRITE},
        {"8: PAGE_WRITECOPY", PAGE_WRITECOPY},
};
void WrapperGetSystemInfo(){
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    cout << "System info: " << endl;
    cout << "Processor architecture: ";
    switch(systemInfo.wProcessorArchitecture){
        case PROCESSOR_ARCHITECTURE_AMD64: cout << "x64 (AMD or Intel)" << endl; break;
        case PROCESSOR_ARCHITECTURE_ARM: cout << "ARM" << endl; break;
        case PROCESSOR_ARCHITECTURE_IA64: cout << "Intel Itanium-based" << endl; break;
        case PROCESSOR_ARCHITECTURE_INTEL: cout << "x86" << endl; break;
        case PROCESSOR_ARCHITECTURE_UNKNOWN: cout << "Unknown architecture." << endl; break;
    }
    cout << "A pointer to the lowest memory address accessible to applications and dynamic-link libraries (DLLs): " << endl;
    cout << systemInfo.lpMinimumApplicationAddress << endl;
    cout << "A pointer to the highest memory address accessible to applications and DLLs. " << endl;
    cout << systemInfo.lpMaximumApplicationAddress << endl;
    cout << " Processors quantity " << endl;
    cout << bitset<32>(systemInfo.dwActiveProcessorMask).count() << endl;
    cout << "The page size and the granularity of page protection and commitment." << endl;
    cout << systemInfo.dwPageSize << endl;
    cout << "The granularity for the starting address at which virtual memory can be allocated." << endl;
    cout << systemInfo.dwAllocationGranularity << endl;
    cout << "The architecture-dependent processor revision." << endl;
    cout << systemInfo.wProcessorRevision << endl;
//    cout << "" << endl;
//    cout << systemInfo.wProcessorRevision << endl;
}
void WrapperGlobalMemoryStatus(){
    MEMORYSTATUS memorystatus;
    GlobalMemoryStatus(&memorystatus);
    cout << "A number between 0 and 100 that specifies the approximate percentage of physical memory that is in use (0 indicates no memory use and 100 indicates full memory use)." << endl;
    cout <<  memorystatus.dwMemoryLoad << endl;
    cout << "The amount of physical memory currently available, in bytes. This is the amount of physical memory that can be immediately reused without having to write its contents to disk first. It is the sum of the size of the standby, free, and zero lists." << endl;
    cout << memorystatus.dwAvailPhys << endl;
    cout << "The current size of the committed memory limit, in bytes. This is physical memory plus the size of the page file, minus a small overhead." << endl;
    cout << memorystatus.dwTotalPageFile << endl;
    cout << "The size of the user-mode portion of the virtual address space of the calling process, in bytes." << endl;
    cout << memorystatus.dwTotalVirtual << endl;
    cout << "The amount of unreserved and uncommitted memory currently in the user-mode portion of the virtual address space of the calling process, in bytes." << endl;
    cout << memorystatus.dwAvailVirtual  << endl;
//    cout << "" << endl;
//    cout << memorystatus.dwAvailPhys  << endl;


}
void WrapperCheckMemoryPart(PVOID address){
    MEMORY_BASIC_INFORMATION basicInformation;
    DWORD t = VirtualQuery(address, &basicInformation, sizeof(basicInformation));
    if (!t){
        cout << "Error: " << t << endl;
        return;
    }
    cout << "A pointer to the base address of the region of pages." << endl;
    cout << basicInformation.BaseAddress  << endl;
    cout << "A pointer to the base address of a range of pages allocated by the VirtualAlloc function." << endl;
    cout << basicInformation.AllocationBase  << endl;
    cout << "The memory protection option when the region was initially allocated. " << endl;
    for (const auto& a: protectionOptions){
        if (a.second == basicInformation.AllocationProtect){
            auto tmp = a.first;
            tmp.erase(0,3);
            cout << tmp << endl;
            break;
        }
    }
    cout << "The size of the region beginning at the base address in which all pages have identical attributes, in bytes."  << endl;
    cout << basicInformation.RegionSize  << endl;
    cout << "The state of the pages in the region: " << endl;
    switch(basicInformation.State){
        case MEM_COMMIT: cout << "MEM_COMMIT state" << endl;
            break;
        case MEM_FREE: cout << "MEM_FREE state" << endl;
            break;
        case MEM_RESERVE: cout << "MEM_RESERVE state" << endl;
            break;
    }

    cout << "The access protection of the pages in the region." << endl;
    for (const auto& a: protectionOptions){
        if (a.second == basicInformation.Protect){
            auto tmp = a.first;
            tmp.erase(0,3);
            cout << tmp << endl;
            break;
        }
    }
    cout << "The type of pages in the region." << endl;
    switch(basicInformation.State){
        case MEM_IMAGE: cout << "MEM_IMAGE type" << endl;
            break;
        case MEM_MAPPED: cout << "MEM_MAPPED type" << endl;
            break;
        case MEM_PRIVATE: cout << "MEM_PRIVATE type" << endl;
            break;
        default: cout << "NO type" << endl;
            break;
    }

}
void WrapperVirtualAllocSeparately(DWORD size, PVOID address=NULL){
    PVOID ptr = VirtualAlloc(address,size,MEM_RESERVE,PAGE_EXECUTE_READWRITE);
    if(!ptr){
        cout << "Something went wrong in memory RESERVing" << endl;
    }
    cout << " Memory address, allocated with VirtualAlloc(MEM_RESERVE) :" << ptr << endl;
    ptr = VirtualAlloc(ptr,size,MEM_COMMIT,PAGE_EXECUTE_READWRITE);
    if(!ptr){
        cout << "Something went wrong in memory COMMITTing" << endl;
    }
    cout << " Memory address, after VirtualAlloc(MEM_COMMIT) :" << ptr << endl;
    BOOL t = VirtualFree(ptr,0,MEM_RELEASE);
    if (!t){
        cout << "Something went wrong in memory RELEASing" << endl;
    }
    cout << " Memory address, after VirtualFree(MEM_RELEASE) :" << ptr << endl;
}
void WrapperVirtualAllocSimultaneously(DWORD size, PVOID address=NULL){
    PVOID ptr = VirtualAlloc(address,size,MEM_RESERVE | MEM_COMMIT,PAGE_EXECUTE_READWRITE);
    if(!ptr){
        cout << "Something went wrong in memory RESERVing" << endl;
    }
    cout << " Memory address, allocated with VirtualAlloc(MEM_RESERVE | MEM_COMMIT) :" << ptr << endl;
    BOOL t = VirtualFree(ptr,0,MEM_RELEASE);
    if (!t){
        cout << "Something went wrong in memory RELEASing" << endl;
    }
    cout << " Memory address, after VirtualFree(MEM_RELEASE) :" << ptr << endl;
}
void WrapperVirtualAllocate(const string& info ,PVOID address=NULL){
    // 0x60000000
    auto ptr = (char *)VirtualAlloc(address,info.length(),MEM_RESERVE | MEM_COMMIT,PAGE_EXECUTE_READWRITE);
    strcpy(ptr,info.c_str());
    cout << "Info in ptr : " << ptr << endl;
    BOOL t = VirtualFree(ptr,0,MEM_RELEASE);
    if (!t){
        cout << "Something went wrong in memory RELEASing" << endl;
    }
}
void WrapperVirtualProtect(PVOID address, DWORD size){
    DWORD prev_sec;
    DWORD sec_var = 0;
    int i = 0;

    do {
        for (const auto& a : protectionOptions){
            cout << a.first << endl;
        }
        cout << "Choose memory protection constants, which you what add to page. Type 0 to end" << endl;
        cin >> i;
        try {
            if(!i){
                break;
            }
            for (const auto& a : protectionOptions) {
                if(a.first.at(0) == i + '0'){
                    sec_var |= a.second;
                }
            }
        }
        catch(out_of_range &e) {
            cout << "Wrong number. Try another" << endl;
        }
    } while(true);
    if(!VirtualProtect(address,size, sec_var, &prev_sec)){
        cout << "Error in VirtualProtect " << endl;
        cout << GetLastError() << endl;
    }
    WrapperCheckMemoryPart(address);
}
int main(){
    auto ptr = VirtualAlloc(nullptr,1000,MEM_RESERVE,PAGE_READWRITE);
    cout << ptr << endl;
    auto* dwPointer = new DWORD;
    *dwPointer = 2;
    WrapperVirtualProtect(dwPointer,sizeof(DWORD));
//    WrapperVirtualAllocate("Vasya Pupkin", (void *)0x60000000);
    return 0;
}