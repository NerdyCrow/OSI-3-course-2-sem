#define _CRT_NON_CONFORMING_WCSTOK
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib>
#include "Windows.h"

using namespace std;

BOOL printFileInfo(LPWSTR FileName);
BOOL printFileTxt(LPWSTR FileName);

int main()
{
    setlocale(LC_ALL, "Russian");

    printFileInfo((LPWSTR)L"D:\\3 kurs 2 sem\\OSI\\Lab9\\OS09_01\\OS09_01.txt");
    printFileTxt((LPWSTR)L"D:\\3 kurs 2 sem\\OSI\\Lab9\\OS09_01\\OS09_01.txt");
}

BOOL printFileInfo(LPWSTR FileName)
{
    LPBY_HANDLE_FILE_INFORMATION lpFileInformation = new BY_HANDLE_FILE_INFORMATION();
    DWORD fileType;
    LPSYSTEMTIME systemTimeCreation = new SYSTEMTIME();
    LPSYSTEMTIME systemTimeLastWrite = new SYSTEMTIME();
    PLARGE_INTEGER fileSize = new LARGE_INTEGER();

    try
    {
        HANDLE of = CreateFile(
            FileName,
            GENERIC_WRITE | GENERIC_READ,
            NULL,
            NULL,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

        if (of == INVALID_HANDLE_VALUE)
        {
            throw "Open file failed";
        }

        char* name = new char[MAX_PATH];
        int i = wcslen(FileName) - 1;
        int lenName = 0;

        while (FileName[i--] != '\\');
        {
            i++;
            while (FileName[i++] != '\0')
            {
                name[lenName++] = FileName[i];
            }
        }

        cout << "Имя файла: " << name << endl;
        cout << "Тип: ";

        if (fileType = GetFileType(of))
        {
            switch (fileType)
            {
            case FILE_TYPE_UNKNOWN: cout << "Unknown file type\n"; break;
            case FILE_TYPE_DISK: cout << "Disk file\n"; break;
            case FILE_TYPE_CHAR: cout << "Printe, console file\n"; break;
            case FILE_TYPE_PIPE: cout << "Socket, named pipe or anonymous pipe\n"; break;
            case FILE_TYPE_REMOTE: cout << "UNDEFINED\n"; break;
            }
        }
        else
        {
            throw "GetFileType failed";
        }

        if (GetFileSizeEx(of, fileSize))
        {
            cout << "Размер: " << fileSize->QuadPart << " bytes" << endl;
        }
        else
        {
            throw "GetFileSizeEx failed";
        }

        if (GetFileInformationByHandle(of, lpFileInformation))
        {
            if (FileTimeToSystemTime(&lpFileInformation->ftCreationTime, systemTimeCreation)
                && FileTimeToSystemTime(&lpFileInformation->ftLastAccessTime, systemTimeLastWrite))
            {
                cout << "Дата и время создания: " << systemTimeCreation->wDay << "."
                    << systemTimeCreation->wMonth << "."
                    << systemTimeCreation->wYear << " "
                    << systemTimeCreation->wHour + 3 << ":"
                    << systemTimeCreation->wMinute << ":"
                    << systemTimeCreation->wSecond << endl;
                cout << "Дата и время обновления: " << systemTimeLastWrite->wDay << "."
                    << systemTimeLastWrite->wMonth << "."
                    << systemTimeLastWrite->wYear << " "
                    << systemTimeLastWrite->wHour + 3 << ":"
                    << systemTimeLastWrite->wMinute << ":"
                    << systemTimeLastWrite->wSecond << endl;
            }
            else
            {
                throw "FileTimeToSystemTime failed";
            }
        }
        else
        {
            throw "GetFileInformationByHandle failed";
        }

        CloseHandle(of);
    }
    catch (char* err)
    {
        cout << "--- Error:\n" << err << "\n";
        return false;
    }

    return true;
}

BOOL printFileTxt(LPWSTR FileName)
{
    PLARGE_INTEGER fileSize = new LARGE_INTEGER();

    try
    {
        HANDLE of = CreateFile(
            FileName,
            GENERIC_READ,
            NULL,
            NULL,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

        if (of == INVALID_HANDLE_VALUE)
        {
            throw "Open file failed";
        }

        if (GetFileSizeEx(of, fileSize))
        {
            char* buf = new char[fileSize->QuadPart];
            ZeroMemory(buf, sizeof(buf));
            DWORD n = NULL;

            if (ReadFile(of, buf, fileSize->QuadPart, &n, NULL))
            {
                buf[fileSize->QuadPart] = '\0';
                cout << "Содержимое:\n" << buf << "\n";
            }
        }
        else
        {
            throw "GetFileSizeEx failed";
        }

        CloseHandle(of);
    }
    catch (char* err)
    {
        cout << "--- Error:\n" << err << "\n";
        return false;
    }

    return true;
}