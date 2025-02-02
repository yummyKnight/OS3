#include <iostream>
#include <windows.h>
#include <vector>
#include <map>
#include <sstream>
#include <iomanip>

using namespace std;

void showDriveInfo(const char &drive_name);

void attrDecoder(DWORD attr)
{
    const map<DWORD, string> fileAttributeConstants = {
        {FILE_ATTRIBUTE_ARCHIVE, "File  is an archive file or directory."},
        {FILE_ATTRIBUTE_COMPRESSED, "File or directory is compressed."
                                    "All of the data in the file is compressed."},
        {FILE_ATTRIBUTE_DEVICE, "This value is reserved for system use."},
        {FILE_ATTRIBUTE_DIRECTORY, "The handle that identifies a directory."},
        {FILE_ATTRIBUTE_ENCRYPTED, "The file is encrypted. "
                                   "For a file, all data streams in the file are encrypted."},
        {FILE_ATTRIBUTE_NORMAL, "The file that does not have attributes set."},
        {FILE_ATTRIBUTE_NOT_CONTENT_INDEXED, "The file is not to be indexed by the content indexing service."},
        {FILE_ATTRIBUTE_READONLY, "The file is read-only."},
        {FILE_ATTRIBUTE_REPARSE_POINT, "The file has an associated reparse point, or a file that is a symbolic link."},
        {FILE_ATTRIBUTE_SPARSE_FILE, "The file is a sparse file."},
        {FILE_ATTRIBUTE_SYSTEM, "The file that the operating system uses a part of, or uses exclusively."},
        {FILE_ATTRIBUTE_TEMPORARY, "A file that is being used for temporary storage."},
    };

    cout << "The file has following attributes: " << endl;
    for (auto &constant : fileAttributeConstants)
    {
        if ((constant.first & attr) == constant.first)
        {
            cout << constant.second << endl;
        }
    }
}

string SysTimeToStr(const SYSTEMTIME *systime)
{

    ostringstream ossMessage;
    ossMessage << systime->wYear << "-"
               << setw(2) << setfill('0') << systime->wMonth << "-"
               << setw(2) << setfill('0') << systime->wDay << " "
               << setw(2) << setfill('0') << systime->wHour << ":"
               << setw(2) << setfill('0') << systime->wMinute << ":"
               << setw(2) << setfill('0') << systime->wSecond << "."
               << setw(3) << setfill('0') << systime->wMilliseconds;

    return ossMessage.str();
}

string getDrives()
{
    int n;
    string disks_names;
    DWORD dr = GetLogicalDrives();
    // 26 - max numbers of disks
    for (unsigned int i = 0; i < 26; i++)
    {
        n = ((dr >> i) & 0x00000001);
        if (n == 1)
        {
            disks_names.push_back(char(65 + i));
            //            disks_names.push_back(' ');
        }
    }
    //    cout << "Available disk drives :" << endl;
    //    cout << disks_names << endl;
    return disks_names;
}

void getDrivesM()
{

    cout << "Available disk drives :" << endl;
    for (auto a : getDrives())
    {
        cout << a << " ";
    }
    cout << endl;
}
void showDriveInfoM()
{
    cout << "Select one of the presented discs:" << endl;
    string drivers = getDrives();
    for (auto i : drivers)
    {
        cout << i << " ";
    }
    cout << endl;
    while (true)
    {
        string volume_name;
        cin >> volume_name;
        if (volume_name.size() == 1 && isalpha(volume_name[0]) && drivers.find(volume_name[0]) != string::npos)
        {
            showDriveInfo(volume_name[0]);
            break;
        }
        else
        {
            cout << "Incorrect data. Try again" << endl;
        }
    }
}

void showDriveInfo(const char &drive_name)
{
    string tmp = ":\\";
    auto full_drive_name = tmp.insert(0, 1, drive_name).c_str();
    int d = GetDriveType(full_drive_name);
    cout << "Driver type:" << endl;
    if (d == DRIVE_UNKNOWN)
        cout << "UNKNOWN" << endl;
    if (d == DRIVE_NO_ROOT_DIR)
        cout << "DRIVE NO ROOT DIR" << endl;
    if (d == DRIVE_REMOVABLE)
        cout << "REMOVABLE" << endl;
    if (d == DRIVE_FIXED)
        cout << "FIXED" << endl;
    if (d == DRIVE_REMOTE)
        cout << "REMOTE" << endl;
    if (d == DRIVE_CDROM)
        cout << "CDROM" << endl;
    if (d == DRIVE_RAMDISK)
        cout << "RAMDISK" << endl;

    char VolumeNameBuffer[100];
    char FileSystemNameBuffer[100];
    LPDWORD FileSystemFlags = new DWORD();
    unsigned long VolumeSerialNumber;
    BOOL GetVolumeInformationFlag = GetVolumeInformationA(
        full_drive_name,
        VolumeNameBuffer,
        100,
        &VolumeSerialNumber,
        nullptr,         //&MaximumComponentLength,
        FileSystemFlags, //&FileSystemFlags,
        FileSystemNameBuffer,
        100);

    if (GetVolumeInformationFlag)
    {
        cout << "Volume Name is " << VolumeNameBuffer << endl;
        cout << "Volume Serial Number is " << VolumeSerialNumber << endl;
        cout << "File System is " << FileSystemNameBuffer << endl;
        cout << "Flags associated with the specified file system:" << endl;

        if ((*FileSystemFlags & FILE_CASE_PRESERVED_NAMES) == FILE_CASE_PRESERVED_NAMES)
            cout << "   The specified volume supports preserved case of file names when it places a name on disk."
                 << endl;

        if ((*FileSystemFlags & FILE_CASE_SENSITIVE_SEARCH) == FILE_CASE_SENSITIVE_SEARCH)
            cout << "   The specified volume supports case-sensitive file names." << endl;

        if ((*FileSystemFlags & FILE_FILE_COMPRESSION) == FILE_FILE_COMPRESSION)
            cout << "   The specified volume supports file-based compression." << endl;

        if ((*FileSystemFlags & FILE_NAMED_STREAMS) == FILE_NAMED_STREAMS)
            cout << "   The specified volume supports named streams." << endl;

        if ((*FileSystemFlags & FILE_PERSISTENT_ACLS) == FILE_PERSISTENT_ACLS)
            cout << "   The specified volume preserves and enforces access control lists (ACL)." << endl;

        if ((*FileSystemFlags & FILE_READ_ONLY_VOLUME) == FILE_READ_ONLY_VOLUME)
            cout << "   The specified volume is read-only." << endl;

        if ((*FileSystemFlags & FILE_SEQUENTIAL_WRITE_ONCE) == FILE_SEQUENTIAL_WRITE_ONCE)
            cout << "   The specified volume supports a single sequential write." << endl;

        if ((*FileSystemFlags & FILE_SUPPORTS_ENCRYPTION) == FILE_SUPPORTS_ENCRYPTION)
            cout << "   The specified volume supports the Encrypted File System (EFS)." << endl;

        if ((*FileSystemFlags & FILE_SUPPORTS_EXTENDED_ATTRIBUTES) == FILE_SUPPORTS_EXTENDED_ATTRIBUTES)
            cout << "   The specified volume supports extended attributes." << endl;

        if ((*FileSystemFlags & FILE_SUPPORTS_HARD_LINKS) == FILE_SUPPORTS_HARD_LINKS)
            cout << "   The specified volume supports hard links. " << endl;

        if ((*FileSystemFlags & FILE_SUPPORTS_OBJECT_IDS) == FILE_SUPPORTS_OBJECT_IDS)
            cout << "   The specified volume supports object identifiers." << endl;

        if ((*FileSystemFlags & FILE_VOLUME_IS_COMPRESSED) == FILE_VOLUME_IS_COMPRESSED)
            cout << "   FILE_CASE_PRESERVED_NAMES" << endl;

        if ((*FileSystemFlags & FILE_SUPPORTS_OPEN_BY_FILE_ID) == FILE_SUPPORTS_OPEN_BY_FILE_ID)
            cout << "   The file system supports open by FileID." << endl;

        if ((*FileSystemFlags & FILE_SUPPORTS_REPARSE_POINTS) == FILE_SUPPORTS_REPARSE_POINTS)
            cout << "   The specified volume supports reparse points." << endl;

        if ((*FileSystemFlags & FILE_SUPPORTS_SPARSE_FILES) == FILE_SUPPORTS_SPARSE_FILES)
            cout << "   The specified volume supports sparse files." << endl;

        if ((*FileSystemFlags & FILE_SUPPORTS_TRANSACTIONS) == FILE_SUPPORTS_TRANSACTIONS)
            cout << "   The specified volume supports transactions." << endl;

        if ((*FileSystemFlags & FILE_SUPPORTS_USN_JOURNAL) == FILE_SUPPORTS_USN_JOURNAL)
            cout << "   The specified volume supports update sequence number (USN) journals." << endl;

        if ((*FileSystemFlags & FILE_UNICODE_ON_DISK) == FILE_UNICODE_ON_DISK)
            cout << "   The specified volume supports Unicode in file names as they appear on disk." << endl;

        if ((*FileSystemFlags & FILE_VOLUME_IS_COMPRESSED) == FILE_VOLUME_IS_COMPRESSED)
            cout << "   The specified volume is a compressed volume" << endl;

        if ((*FileSystemFlags & FILE_VOLUME_QUOTAS) == FILE_VOLUME_QUOTAS)
            cout << "   The specified volume supports disk quotas." << endl;
    }
    else
        cout << "Volume information is not present" << endl;

    delete FileSystemFlags;

    ULARGE_INTEGER
        TotalNumberOfBytes,
        TotalNumberOfFreeBytes;

    BOOL GetDiskFreeSpaceFlag = GetDiskFreeSpaceEx(
        full_drive_name,                         // directory name
        nullptr,                         // bytes available to caller
        &TotalNumberOfBytes,    // bytes on disk
        &TotalNumberOfFreeBytes // free bytes on disk
    );
    //    Dont work properly
    if (GetDiskFreeSpaceFlag)
    {
        cout << "Total number of bytes on a disk that are available to the user who is "
                "associated with the calling thread = "
             << TotalNumberOfBytes.QuadPart << "( " << double(TotalNumberOfBytes.QuadPart) / 1024 / 1024
             << " Mb )" << endl;

        cout << "Total number of free bytes on a disk = " << TotalNumberOfFreeBytes.QuadPart
             << "( " << double(TotalNumberOfFreeBytes.QuadPart) / 1024 / 1024
             << " Mb )" << endl;
    }
    else
        cout << "Not Present (GetDiskFreeSpace)" << endl;
}

void createDir(const string &path)
{
    LPCSTR PathName = path.c_str();
    BOOL t = CreateDirectoryA(
        PathName,
        nullptr //  If lpSecurityAttributes is NULL, the directory gets a default security descriptor
    );
    if (!t)
    {
        cout << "Error while creating folder in " + path << endl;
    }
}

HANDLE createFileWrapper(const string &file_name)
{

    HANDLE new_handle = CreateFileA(
        file_name.c_str(),            // file name
        GENERIC_READ | GENERIC_WRITE, //DesiredAccess
        0,                            // share access
        nullptr,                      //SecurityAttributes
        CREATE_ALWAYS,                //CreationDisposition
        FILE_ATTRIBUTE_NORMAL,        //FlagsAndAttributes
        nullptr);
    cout << "Created new file in " << file_name << endl;
    cout << "Handle address is " << new_handle << endl;
    return new_handle;
}

void removeDir(const string &path)
{
    LPCSTR PathName = path.c_str();
    BOOL t = RemoveDirectoryA(
        PathName //  If lpSecurityAttributes is NULL, the directory gets a default security descriptor
    );
    if (!t)
    {
        cout << "Error while deleting folder in " + path << endl;
    }
}

void copyFileWrapper(const string &src_name, const string &dst_name)
{
    char choice;
    BOOL t = CopyFile(src_name.c_str(), dst_name.c_str(), TRUE); // func overwrites file if it already exist
    if (GetLastError() == 183)
    {
        cout << "File with that name already exist in destination directory. Print 'y' to overwrite it" << endl;
        cin >> choice;
        if (choice == 'y')
        {
            t = CopyFile(src_name.c_str(), dst_name.c_str(), FALSE);
        }
    }
    if (!t)
    {
        cout << "Error while copying file from " << src_name << " to " << dst_name << endl;
    }
}

void getFileAttr(const string &file)
{
    /* always return INVALID_FILE_ATTRIBUTES. idk why.
     *
     *
     * */

    DWORD t = GetFileAttributes(file.c_str());
    if (t == INVALID_FILE_ATTRIBUTES)
    {
        cout << "Error while getting attributes from file " << file << endl;
    }
    attrDecoder(t);
    //    else {
    //cout << "0x" << hex << t << endl;

    //    }
}

void setFileAttr(const string &file)
{
    /* All attr:
 * FILE_ATTRIBUTE_ARCHIVE
FILE_ATTRIBUTE_HIDDEN
FILE_ATTRIBUTE_NORMAL
FILE_ATTRIBUTE_NOT_CONTENT_INDEXED
FILE_ATTRIBUTE_OFFLINE
FILE_ATTRIBUTE_READONLY
FILE_ATTRIBUTE_SYSTEM
FILE_ATTRIBUTE_TEMPORARY

 * */
    BOOL t = SetFileAttributes(file.c_str(), FILE_ATTRIBUTE_READONLY);
    if (t == INVALID_FILE_ATTRIBUTES)
    {
        cout << "Error while setting attributes to file " << file << endl;
    }
}

void moveFileWrapper(const string &src_name, const string &dst_name)
{
    char choice;
    BOOL t = MoveFile(src_name.c_str(), dst_name.c_str()); // func overwrites file if it already exist
    if (GetLastError() == 183)
    {
        cout << "File with that name already exist in destination directory. Print 'y' to overwrite it" << endl;
        cin >> choice;
        if (choice == 'y')
        {
            t = MoveFileEx(src_name.c_str(), dst_name.c_str(), MOVEFILE_REPLACE_EXISTING);
        }
    }
    if (!t)
    {
        cout << "Error while moving file from " << src_name << " to " << dst_name << endl;
    }
}

void setFileTimeWrapper(const HANDLE &file_handle)
{
    FILETIME filetime;
    auto sys_time = new SYSTEMTIME();
    GetSystemTime(sys_time);
    SystemTimeToFileTime(sys_time, &filetime);
    BOOL t = SetFileTime(file_handle, nullptr, &filetime, &filetime);
    if (!t)
    {
        cout << "Error while setting file time " << endl;
    }
    delete sys_time;
}

void getFileInfoByHandle(const HANDLE &file_handle)
{

    auto info = new BY_HANDLE_FILE_INFORMATION();
    BOOL t = GetFileInformationByHandle(file_handle, info);
    if (!t)
    {
        cerr << "Error while getting file info from handle " << GetLastError() << endl;
    }
    else
    {

        auto systime_create = new SYSTEMTIME();
        auto systime_access = new SYSTEMTIME();
        auto systime_write = new SYSTEMTIME();

        attrDecoder(info->dwFileAttributes);

        FileTimeToSystemTime(&info->ftCreationTime, systime_create);
        cout << "Creation time = " << SysTimeToStr(systime_create) << endl;

        FileTimeToSystemTime(&info->ftLastAccessTime, systime_access);
        cout << "Last access time = " << SysTimeToStr(systime_access) << endl;

        FileTimeToSystemTime(&info->ftLastWriteTime, systime_write);
        cout << "Last write time = " << SysTimeToStr(systime_write) << endl;

        cout << "Volume serial number is " << info->dwVolumeSerialNumber << endl;
        delete systime_create;
        delete systime_create;
        delete systime_create;
        delete info;
    }
}

int main()
{
    //getFileAttr("../baka.txt");

    setlocale(LC_CTYPE, "rus");
    bool work = true;
    while (work)
    {
        cout << "------------------------------------------------------------------------------------------" << endl;
        cout << "Available actions:" << endl;
        cout << " Enter 1 to list the drives." << endl;
        cout << " Enter 2 to display detailed information about a specific drive." << endl;
        cout << " Enter 3 to create the directory." << endl;
        cout << " Enter 4 to delete the directory." << endl;
        cout << " Enter 5 to create the file." << endl;
        cout << " Enter 6 to copy the file." << endl;
        cout << " Enter 7 to move the file." << endl;
        cout << " Enter 8 to display the file attributes." << endl;
        cout << " Enter 9 to set the file attributes." << endl;
        cout << " Enter 10 to get information about the file by its handle." << endl;
        cout << " Enter 11, update the time the file was accessed and the time the file was last written to." << endl;
        cout << "To exit press enter any other digit" << endl;
        cout << "------------------------------------------------------------------------------------------" << endl;
        unsigned i;
        string path1;
        string path2;
        HANDLE tmp_handle;
        HANDLE vasta_h = CreateFileA("test.txt", GENERIC_WRITE | GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        cin >> i;
        switch (i)
        {
        case 1:
            getDrivesM();
            break;
        case 2:
            showDriveInfoM();
            break;
        case 3:
            cout << "Enter the full path to the directory: " << endl;
            cin >> path1;
            createDir(path1);
            break;
        case 4:
            cout << "Enter the full path to the directory: " << endl;
            cin >> path1;
            removeDir(path1);
            break;
        case 5:
            cout << "Enter the full path to the file: " << endl;
            cin >> path1;
            tmp_handle = createFileWrapper(path1);
            cout << "handle: " << tmp_handle << endl;
            // CloseHandle(tmp_handle);
            break;
        case 6:
            cout << "Enter the full path to the file you want to copy: " << endl;
            cin >> path1;
            cout << "Enter the path where you want to copy the file(if necessary, its new name): " << endl;
            cin >> path2;
            copyFileWrapper(path1, path2);
            break;
        case 7:
            cout << "Enter the full path to the file you want to move: " << endl;
            cin >> path1;
            cout << "Enter the path where you want to move the file(if necessary, its new name): " << endl;
            cin >> path2;
            moveFileWrapper(path1, path2);
            break;
        case 8:
            cout << "Enter the full path to the file for which you want to display attributes: " << endl;
            cin >> path1;
            getFileAttr(path1);
            break;
        case 9:
            cout << "Enter the full path to the file to which you want to add the attribute: " << endl;
            cin >> path1;
            setFileAttr(path1);
            break;
        case 10:
            cout << "Enter the handle of the file for which you want to display the information: " << endl;
            cin >> tmp_handle;
            getFileInfoByHandle(tmp_handle);
            break;
        case 11:
            cout << "Enter the handle of the file for which you want to mark the time of creation and last access to the current: " << endl;
            cout << vasta_h << endl;
            setFileTimeWrapper(vasta_h);
            break;
        default:
            work = false;
            break;
        }
    }
    return 0;
    //    cout << GetLastError() << endl;
}