#include <iostream>
#include <windows.h>
#include <vector>

using namespace std;

void showDriveInfo(const char &drive_name);

string getDrives() {
    int n;
    string disks_names;
    DWORD dr = GetLogicalDrives();
    // 26 - max numbers of disks
    for (unsigned int i = 0; i < 26; i++) {
        n = ((dr >> i) & 0x00000001);
        if (n == 1) {
            disks_names.push_back(char(65 + i));
//            disks_names.push_back(' ');
        }
    }
//    cout << "Available disk drives :" << endl;
//    cout << disks_names << endl;
    return disks_names;
}

void showDriveWithMenu() {
    cout << "Select one of the presented discs:" << endl;
    string drivers = getDrives();
    for (auto i : drivers) {
        cout << i << " ";
    }
    cout << endl;
    while (true) {
        string volume_name;
        cin >> volume_name;
        if (volume_name.size() == 1 && isalpha(volume_name[0]) && drivers.find(volume_name[0]) != string::npos) {
            showDriveInfo(volume_name[0]);
            break;
        } else {
            cout << "Incorrect data. Try again" << endl;
        }
    }

}

void showDriveInfo(const char &drive_name) {
    string tmp = ":\\";
    auto full_drive_name = tmp.insert(0, 1, drive_name).c_str();
    int d = GetDriveType(full_drive_name);
    cout << "Driver type:" << endl;
    if (d == DRIVE_UNKNOWN) cout << "UNKNOWN" << endl;
    if (d == DRIVE_NO_ROOT_DIR) cout << "DRIVE NO ROOT DIR" << endl;
    if (d == DRIVE_REMOVABLE) cout << "REMOVABLE" << endl;
    if (d == DRIVE_FIXED) cout << "FIXED" << endl;
    if (d == DRIVE_REMOTE) cout << "REMOTE" << endl;
    if (d == DRIVE_CDROM) cout << "CDROM" << endl;
    if (d == DRIVE_RAMDISK) cout << "RAMDISK" << endl;

    char VolumeNameBuffer[100];
    char FileSystemNameBuffer[100];
    LPDWORD FileSystemFlags = new DWORD();
    unsigned long VolumeSerialNumber;
    BOOL GetVolumeInformationFlag = GetVolumeInformationA(
            full_drive_name,
            VolumeNameBuffer,
            100,
            &VolumeSerialNumber,
            nullptr, //&MaximumComponentLength,
            FileSystemFlags, //&FileSystemFlags,
            FileSystemNameBuffer,
            100
    );

    if (GetVolumeInformationFlag) {
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
    } else cout << "Volume information is not present" << endl;

    delete FileSystemFlags;

    DWORD TotalNumberOfBytes;
    DWORD TotalNumberOfFreeBytes;

    BOOL GetDiskFreeSpaceFlag = GetDiskFreeSpaceEx(
            full_drive_name,                      // directory name
            nullptr,     // bytes available to caller
            (PULARGE_INTEGER) &TotalNumberOfBytes,     // bytes on disk
            (PULARGE_INTEGER) &TotalNumberOfFreeBytes  // free bytes on disk
    );

    if (GetDiskFreeSpaceFlag) {
        cout << "Total number of bytes on a disk that are available to the user who is "
                "associated with the calling thread = " << TotalNumberOfBytes << endl;

        cout << "Total number of free bytes on a disk = " << TotalNumberOfFreeBytes
             << "( " << double(TotalNumberOfFreeBytes) / 1024 / 1000
             << " Mb )" << endl;
    } else cout << "Not Present (GetDiskFreeSpace)" << endl;

}

int main() {
    showDriveWithMenu();
}