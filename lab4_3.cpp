//
// Created by Admin on 21.10.2019.
//
#include <windows.h>
#include <cstdio>

#define BUF_SIZE 256
TCHAR szName[]=TEXT("Local\\MyFileMappingObject");

int main()
{
   HANDLE hMapFile;
   LPCTSTR pBuf;
   hMapFile = OpenFileMapping(
                   FILE_MAP_ALL_ACCESS,   // read/write access
                   FALSE,                 // do not inherit the name
                   szName);               // name of mapping object

   if (hMapFile == NULL)
   {
      printf("Could not open file mapping object (%lu).\n", GetLastError());
      return 1;
   }

   pBuf = (LPTSTR) MapViewOfFile(hMapFile, // handle to map object
               FILE_MAP_ALL_ACCESS,  // read/write permission
               0,
               0,
               BUF_SIZE);

   if (pBuf == NULL)
   {
      printf("Could not map view of file (%lu).\n",GetLastError());

      CloseHandle(hMapFile);

      return 1;
   }
   printf("Result:\n");
   printf("%s\n", pBuf);

   UnmapViewOfFile(pBuf);

   CloseHandle(hMapFile);
   system("pause");
   return 0;
}
