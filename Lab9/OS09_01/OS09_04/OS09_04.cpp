#define _CRT_NON_CONFORMING_WCSTOK
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib>
#include "Windows.h"

using namespace std;

BOOL printWatchRowFileTxt(LPWSTR FileName, DWORD mlsec);
int CheckFirstCount(LPWSTR FileName);

int fCount = 0;
int main()
{
    setlocale(LC_ALL, "Russian");

    LPWSTR fileName = (LPWSTR)L"D:\\3 kurs 2 sem\\OSI\\Lab9\\OS09_01\\OS09_01.txt";
   

    printWatchRowFileTxt(fileName, 100000);
}


BOOL printWatchRowFileTxt(LPWSTR FileName, DWORD mlsec)
{
    PLARGE_INTEGER fileSize = new LARGE_INTEGER();
    LPWSTR path = new wchar_t[MAX_PATH + 1];
    LPWSTR cFileName = new wchar_t[wcslen(FileName) * sizeof(char) + 1];
    fCount = CheckFirstCount(FileName);
    int i = 0;
    int lenName = 0;

    while (true)
    {
        path[lenName++] = FileName[i++];
        if (FileName[i] == '\0')
        {
            while (path[--lenName] != '\\')
            {
                path[lenName] = '\0';
            }
            path[lenName] = '\0';
            break;
        }
    }
    try
    {
        HANDLE notif = FindFirstChangeNotification(path, false, FILE_NOTIFY_CHANGE_SIZE);
        clock_t t1 = clock();
        DWORD dwWaitStatus;

        while (true)
        {
            dwWaitStatus = WaitForSingleObject(notif, mlsec - (clock() - t1));

            if (FindNextChangeNotification(notif) && !dwWaitStatus)
            {
                int position = 0;
                int rowCount = 1;
                HANDLE of = CreateFile(
                    FileName,
                    GENERIC_READ,
                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                    NULL,
                    OPEN_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL,
                    NULL);

                if (of == INVALID_HANDLE_VALUE)
                {
                    CloseHandle(of);
                    throw "Open file failed";
                }

                if (GetFileSizeEx(of, fileSize))
                {
                    char* buf = new char[(fileSize->QuadPart + 1) * sizeof(char)];
                    ZeroMemory(buf, (fileSize->QuadPart + 1) * sizeof(char));
                    DWORD n = NULL;

                    if (ReadFile(of, buf, fileSize->QuadPart, &n, NULL))
                    {
                        while (buf[position++] != '\0')
                        {
                            if (buf[position] == '\n')
                            {
                                rowCount++;
                            }
                        }
                    }
                }
                if (fCount != rowCount) {
                    cout << "Кол - во строк : " << rowCount << endl;
                    fCount = rowCount;
                }

                CloseHandle(of);
            }
            else
            {
                break;
            }

            if (clock() - t1 > mlsec)
            {
                break;
            }
        }

        CloseHandle(notif);
    }
    catch (const char* err)
    {
        cout << "--- Error:\n" << err << "\n";
        return false;
    }

    return true;
}
int CheckFirstCount(LPWSTR FileName) {
    PLARGE_INTEGER fSize = new LARGE_INTEGER();
    int pos = 0;
    int rCount = 1;
    HANDLE of = CreateFile(
        FileName,
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (of == INVALID_HANDLE_VALUE)
    {
        CloseHandle(of);
        throw "Open file failed";
    }

    if (GetFileSizeEx(of, fSize))
    {
        char* buf = new char[(fSize->QuadPart + 1) * sizeof(char)];
        ZeroMemory(buf, (fSize->QuadPart + 1) * sizeof(char));
        DWORD n = NULL;

        if (ReadFile(of, buf, fSize->QuadPart, &n, NULL))
        {
            while (buf[pos++] != '\0')
            {
                if (buf[pos] == '\n')
                {
                    rCount++;
                }
            }
        }
    }
    return rCount;

}
