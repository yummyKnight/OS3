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
map<string, DWORD> protectionOptions = {
    {"1: PAGE_EXECUTE", PAGE_EXECUTE},
    {"2: PAGE_EXECUTE_READ", PAGE_EXECUTE_READ},
    {"3: PAGE_EXECUTE_READWRITE", PAGE_EXECUTE_READWRITE},
    {"4: PAGE_EXECUTE_WRITECOPY", PAGE_EXECUTE_WRITECOPY},
    {"5: PAGE_NOACCESS", PAGE_NOACCESS},
    {"6: PAGE_READONLY", PAGE_READONLY},
    {"7: PAGE_READWRITE", PAGE_READWRITE},
    {"8: PAGE_WRITECOPY", PAGE_WRITECOPY},
};
string determineProtection(DWORD protection)
{
    for (const auto &a : protectionOptions)
    {
        if (a.second == protection)
        {
            auto tmp = a.first;
            tmp.erase(0, 3);
            return tmp;
        }
    }
}
void WrapperGetSystemInfo()
{
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    cout << "System info: " << endl;
    cout << "Processor architecture: ";
    switch (systemInfo.wProcessorArchitecture)
    {
    case PROCESSOR_ARCHITECTURE_AMD64:
        cout << "x64 (AMD or Intel)" << endl;
        break;
    case PROCESSOR_ARCHITECTURE_ARM:
        cout << "ARM" << endl;
        break;
    case PROCESSOR_ARCHITECTURE_IA64:
        cout << "Intel Itanium-based" << endl;
        break;
    case PROCESSOR_ARCHITECTURE_INTEL:
        cout << "x86" << endl;
        break;
    case PROCESSOR_ARCHITECTURE_UNKNOWN:
        cout << "Unknown architecture." << endl;
        break;
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
void WrapperGlobalMemoryStatus()
{
    MEMORYSTATUS memorystatus;
    GlobalMemoryStatus(&memorystatus);
    cout << "A number between 0 and 100 that specifies the approximate percentage of physical memory that is in use (0 indicates no memory use and 100 indicates full memory use)." << endl;
    cout << memorystatus.dwMemoryLoad << endl;
    cout << "The amount of physical memory currently available, in bytes. This is the amount of physical memory that can be immediately reused without having to write its contents to disk first. It is the sum of the size of the standby, free, and zero lists." << endl;
    cout << memorystatus.dwAvailPhys << endl;
    cout << "The current size of the committed memory limit, in bytes. This is physical memory plus the size of the page file, minus a small overhead." << endl;
    cout << memorystatus.dwTotalPageFile << endl;
    cout << "The size of the user-mode portion of the virtual address space of the calling process, in bytes." << endl;
    cout << memorystatus.dwTotalVirtual << endl;
    cout << "The amount of unreserved and uncommitted memory currently in the user-mode portion of the virtual address space of the calling process, in bytes." << endl;
    cout << memorystatus.dwAvailVirtual << endl;
    //    cout << "" << endl;
    //    cout << memorystatus.dwAvailPhys  << endl;
}
void WrapperCheckMemoryPart(PVOID address)
{
    MEMORY_BASIC_INFORMATION basicInformation;
    DWORD t = VirtualQuery(address, &basicInformation, sizeof(basicInformation));
    if (!t)
    {
        cout << "Error in VirtualQuery: " << t << endl;
        return;
    }
    cout << "A pointer to the base address of the region of pages." << endl;
    cout << basicInformation.BaseAddress << endl;
    cout << "A pointer to the base address of a range of pages allocated by the VirtualAlloc function." << endl;
    cout << basicInformation.AllocationBase << endl;
    cout << "The memory protection option when the region was initially allocated. " << endl;
    for (const auto &a : protectionOptions)
    {
        if (a.second == basicInformation.AllocationProtect)
        {
            auto tmp = a.first;
            tmp.erase(0, 3);
            cout << tmp << endl;
            break;
        }
    }
    cout << "The size of the region beginning at the base address in which all pages have identical attributes, in bytes." << endl;
    cout << basicInformation.RegionSize << endl;
    cout << "The state of the pages in the region: " << endl;
    switch (basicInformation.State)
    {
    case MEM_COMMIT:
        cout << "MEM_COMMIT state" << endl;
        break;
    case MEM_FREE:
        cout << "MEM_FREE state" << endl;
        break;
    case MEM_RESERVE:
        cout << "MEM_RESERVE state" << endl;
        break;
    }

    cout << "The access protection of the pages in the region." << endl;
    cout << determineProtection(basicInformation.Protect) << endl;
    cout << "The type of pages in the region." << endl;
    switch (basicInformation.State)
    {
    case MEM_IMAGE:
        cout << "MEM_IMAGE type" << endl;
        break;
    case MEM_MAPPED:
        cout << "MEM_MAPPED type" << endl;
        break;
    case MEM_PRIVATE:
        cout << "MEM_PRIVATE type" << endl;
        break;
    default:
        cout << "NO type" << endl;
        break;
    }
}

void WrapperVirtualAllocSeparately(DWORD size, PVOID address = NULL)
{
    PVOID ptr = VirtualAlloc(address, size, MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!ptr)
    {
        cout << "Something went wrong in memory RESERVing" << endl;
    }
    cout << " Memory address, allocated with VirtualAlloc(MEM_RESERVE) :" << ptr << endl;
    cout << "Press any key to commit memory" << endl;
    system("pause");
    ptr = VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (!ptr)
    {
        cout << "Something went wrong in memory COMMITTing" << endl;
    }
    cout << " Memory address, after VirtualAlloc(MEM_COMMIT) :" << ptr << endl;
    cout << "Press any key to realese memory" << endl;
    system("pause");
    BOOL t = VirtualFree(ptr, 0, MEM_RELEASE);
    if (!t)
    {
        cout << "Something went wrong in memory RELEASing" << endl;
    }
    cout << " Memory address, after VirtualFree(MEM_RELEASE) :" << ptr << endl;
}
void WrapperVirtualAllocSimultaneously(DWORD size, PVOID address = NULL)
{
    PVOID ptr = VirtualAlloc(address, size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (!ptr)
    {
        cout << "Something went wrong in memory RESERVing" << endl;
    }
    cout << " Memory address, allocated with VirtualAlloc(MEM_RESERVE | MEM_COMMIT) :" << ptr << endl;
    cout << "Press any key to realese memory" << endl;
    system("pause");
    BOOL t = VirtualFree(ptr, 0, MEM_RELEASE);
    if (!t)
    {
        cout << "Something went wrong in memory RELEASing" << endl;
    }
    cout << " Memory address, after VirtualFree(MEM_RELEASE) :" << ptr << endl;
}
void WrapperVirtualAllocate(const string &info, PVOID address = NULL)
{
    // 0x60000000
    auto ptr = (char *)VirtualAlloc(address, info.length() * 2, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    cout << "All ok" << endl;
    if (!ptr)
    {
        cout << "Can't allocate memory: " << GetLastError() << endl;
        return;
    }
    strcpy(ptr, info.c_str());
    cout << "Info in ptr : " << ptr << endl;
    cout << "Press any key to free memory" << endl;
    system("pause");
    BOOL t = VirtualFree(ptr, 0, MEM_RELEASE);
    if (!t)
    {
        cout << "Something went wrong in memory RELEASing" << endl;
    }
}
void WrapperVirtualProtect(PVOID address, DWORD size)
{
    DWORD prev_sec;
    DWORD sec_var = 0;
    int i = 0;
    for (const auto &a : protectionOptions)
    {
        cout << a.first << endl;
    }
    cout << "Choose memory protection constant, which you what add to page." << endl;
    bool flag = true;
    do
    {
        cin >> i;
        for (const auto &a : protectionOptions)
        {
            if (a.first.at(0) == i + '0')
            {
                sec_var |= a.second;
                flag = false;
            }
        }
        if (flag)
        {
            cout << "Wrong number, Try another" << endl;
        }

    } while (flag);
    if (!VirtualProtect(address, size, sec_var, &prev_sec))
    {
        cout << "Address is free. Allocating memory..." << endl;
        auto ptr = (char *)VirtualAlloc(address, 100, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        if (!ptr)
        {
            cout << "Cant allocated memory in this address" << endl;
            return;
        }
        else
        {
            if (!VirtualProtect(ptr, size, sec_var, &prev_sec))
            {
                cout << "Error in VirtualProtect " << endl;
                auto t = GetLastError();
                cout << t << endl;
                return;
            }
            cout << "Protection constant successfully changed" << endl;
            cout << "Checking: " << endl;
            WrapperCheckMemoryPart(ptr);
            cout << "Press any key to release memory" << endl;
            system("pause");
            BOOL t = VirtualFree(ptr, 0, MEM_RELEASE);
            if (!t)
            {
                cout << "Something went wrong in memory RELEASing" << endl;
            }
        }
    }
    cout << "Protection constant successfully changed" << endl;
    cout << "Checking: " << endl;
    WrapperCheckMemoryPart(address);
}
int main()
{
    // auto* dwPointer = new DWORD;
    // *dwPointer = 2;
    // WrapperCheckMemoryPart((void *) 0x7ff6d24b0);
    // WrapperVirtualProtect(dwPointer,sizeof(DWORD));
    bool flag = false;

    do
    {
        int i;
        int address = 0;
        string s_address;
        string info;
        DWORD size = 0;
        cout << "---------------------------------------------------------" << endl;
        cout << "Enter 1 to get info about computing system\n"
                "Enter 2 to get status of virtual memory \n"
                "Enter 3 to get state about specific memory area\n"
                "Enter 4 to alloc memory and commit it separately\n"
                "Enter 5 to alloc memory and commit it simultaneously\n"
                "Enter 6 to write info to the specific region of memory\n"
                "Enter 7 to set access protection to the specific region of memory\n"
             << endl;
        cout << "---------------------------------------------------------" << endl;
        cin >> i;
        switch (i)
        {
        case 1:
            WrapperGetSystemInfo();
            break;
        case 2:
            WrapperGlobalMemoryStatus();
            break;
        case 3:
            cout << "Enter address (from 0x000'00000000 to 0x7FF'FFFFFFFF):" << endl;
            cin >> s_address;
            address = stoi(s_address, 0, 16);
            WrapperCheckMemoryPart((void *)address);
            break;
        case 4:
            cout << "Enter size:" << endl;
            cin >> size;
            cout << "Enter address (from 0x000'00000000 to 0x7FF'FFFFFFFF)"
                    "(or NULL to allow the program to allocate memory itself):"
                 << endl;
            cin >> s_address;
            try
            {
                /* code */
                address = stoi(s_address, 0, 16);
            }
            catch (const std::invalid_argument &e)
            {
                address = NULL;
            }

            WrapperVirtualAllocSeparately(size, (void *)address);
            break;
        case 5:
            cout << "Enter size:" << endl;
            cin >> size;
            cout << "Enter address (from 0x000'00000000 to 0x7FF'FFFFFFFF)"
                    "(or NULL to allow the program to allocate memory itself):"
                 << endl;
            cin >> s_address;
            try
            {
                /* code */
                address = stoi(s_address, 0, 16);
            }
            catch (const std::invalid_argument &e)
            {
                address = NULL;
            }

            WrapperVirtualAllocSimultaneously(size, (void *)address);
            break;
        case 6:
            cout << "Write info:" << endl;
            cin >> info;
            cout << "Enter address (from 0x000'00000000 to 0x7FF'FFFFFFFF)"
                    "(or NULL to allow the program to allocate memory itself):"
                 << endl;
            cin >> s_address;
            try
            {
                /* code */
                address = stoi(s_address, 0, 16);
            }
            catch (const std::invalid_argument &e)
            {
                address = NULL;
            }

            WrapperVirtualAllocate(info, (void *)address);
            break;
        case 7:
            cout << "Enter size or region:" << endl;
            cin >> size;
            cout << "Enter address (from 0x000'00000000 to 0x7FF'FFFFFFFF)"
                    "(or NULL to allow the program to allocate memory itself):"
                 << endl;
            cin >> s_address;
            try
            {
                /* code */
                address = stoi(s_address, 0, 16);
            }
            catch (const std::invalid_argument &e)
            {
                address = NULL;
            }

            WrapperVirtualProtect((void *)address, size);
            break;
        default:
            flag = true;
            break;
        }
    } while (!flag);

    //    WrapperVirtualAllocate("Vasya Pupkin", (void *)0x60000000);
    return 0;
}