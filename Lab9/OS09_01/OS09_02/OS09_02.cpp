#define _CRT_NON_CONFORMING_WCSTOK
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib>
#include "Windows.h"

using namespace std;

BOOL delRowFileTxt(LPWSTR FileName, DWORD row);
BOOL printFileTxt(LPWSTR FileName);

int main()
{
    setlocale(LC_ALL, "Russian");

    LPWSTR fileName = (LPWSTR)L"D:\\3 kurs 2 sem\\OSI\\Lab9\\OS09_01\\OS09_01.txt";

    delRowFileTxt(fileName, 1);
    delRowFileTxt(fileName, 3);
    delRowFileTxt(fileName, 8);
    delRowFileTxt(fileName, 10);

    printFileTxt(fileName);
}

BOOL delRowFileTxt(LPWSTR FileName, DWORD row)
{
    PLARGE_INTEGER fileSize = new LARGE_INTEGER();
    int rowCount = 1;
    int position = 0;
    int positionAfter = 0;
    bool rowFound = false;

    try
    {
        if (row <= 0)
        {
            throw "Invalid number of raw";
        }

        HANDLE of = CreateFile(
            FileName,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ,
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
            char* buf = new char[fileSize->QuadPart];
            char* bufAfterDel = new char[fileSize->QuadPart];
            ZeroMemory(buf, fileSize->QuadPart);
            ZeroMemory(bufAfterDel, fileSize->QuadPart);
            DWORD amountByte = NULL;

            if (ReadFile(of, buf, fileSize->QuadPart, &amountByte, NULL))
            {
                buf[fileSize->QuadPart] = '\0';

                while (buf[position] != '\0')
                {
                    if (buf[position] == '\n')
                    {
                        rowCount++;
                    }

                    if (rowCount == row)
                    {
                        rowFound = true;
                        rowCount++;
                        position++;

                        while (buf[position] != '\n' && buf[position] != '\0')
                        {
                            position++;
                        }

                        if (row == 1)
                        {
                            position++;
                        }
                    }

                    bufAfterDel[positionAfter++] = buf[position++];
                }

                bufAfterDel[positionAfter] = '\0';
            }

            if (rowFound)
            {
                if (SetFilePointer(of, 0, 0, FILE_BEGIN) == 0)
                {
                    if (WriteFile(of, bufAfterDel, strlen(bufAfterDel), &amountByte, NULL))
                    {
                        cout << "Row deleted successfully" << endl;
                    }
                    else
                    {
                        CloseHandle(of);
                        throw "WriteFile failed";
                    }

                    if (!SetEndOfFile(of))
                    {
                        CloseHandle(of);
                        throw "SetEndOfFile failed";
                    }
                }
                else
                {
                    CloseHandle(of);
                    throw "SetFilePointer failed";
                }
            }
            else
            {
                CloseHandle(of);
                throw "Row isn't found";
            }
        }
        else
        {
            CloseHandle(of);
            throw "GetFileSizeEx failed";
        }
        CloseHandle(of);
    }
    catch (const char* err)
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
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
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
            char* buf = new char[fileSize->QuadPart];
            ZeroMemory(buf, sizeof(buf));
            DWORD amountByte = NULL;

            if (ReadFile(of, buf, fileSize->QuadPart, &amountByte, NULL))
            {
                buf[fileSize->QuadPart] = '\0';
                cout << buf << "\n";
            }
            else
            {
                CloseHandle(of);
                throw "ReadFile failed";
            }
        }
        else
        {
            CloseHandle(of);
            throw "GetFileSizeEx failed";
        }

        CloseHandle(of);
    }
    catch (const char* err)
    {
        cout << "--- Error:\n" << err << "\n";
        return false;
    }

    return true;
}
