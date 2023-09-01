#define _CRT_NON_CONFORMING_WCSTOK
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdlib>
#include "Windows.h"

using namespace std;

BOOL insRowFileTxt(LPWSTR FileName, LPWSTR str, DWORD row);
BOOL printFileTxt(LPWSTR FileName);

int main()
{
    setlocale(LC_ALL, "Russian");

    LPWSTR fileName = (LPWSTR)L"D:\\3 kurs 2 sem\\OSI\\Lab9\\OS09_01\\OS09_01.txt";

    insRowFileTxt(fileName, (LPWSTR)L"Слемнёв 0", 0);
    insRowFileTxt(fileName, (LPWSTR)L"Слемнёв -1", -1);
    insRowFileTxt(fileName, (LPWSTR)L"Слемнёв 5", 5);
    insRowFileTxt(fileName, (LPWSTR)L"Слемнёв 7", 7);

    printFileTxt(fileName);
}

BOOL insRowFileTxt(LPWSTR FileName, LPWSTR str, DWORD row)
{
    PLARGE_INTEGER fileSize = new LARGE_INTEGER();
    int rowCount = 1;
    int position = 0;
    int positionAfter = 0;
    bool rowFound = false;

    try
    {
        HANDLE of = CreateFile(
            FileName,
            GENERIC_READ | GENERIC_WRITE,
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
            char* cstr = new char[wcslen(str) * sizeof(char)];
            wcstombs(cstr, str, strlen(cstr));
            char* buf = new char[(fileSize->QuadPart) * sizeof(char)];
            char* bufAfterIns = new char[(fileSize->QuadPart + strlen(cstr)) * sizeof(char)];
            ZeroMemory(buf, (fileSize->QuadPart) * sizeof(char));
            ZeroMemory(bufAfterIns, (fileSize->QuadPart + strlen(cstr)) * sizeof(char));
            DWORD amountByte = NULL;

            if (ReadFile(of, buf, fileSize->QuadPart, &amountByte, NULL))
            {
                buf[fileSize->QuadPart] = '\0';

                while (buf[position] != '\0')
                {
                    if (!rowFound && (rowCount == row || row == 0))
                    {
                        for (int i = 0; i < strlen(cstr); i++)
                        {
                            bufAfterIns[positionAfter++] = cstr[i];
                        }

                        bufAfterIns[positionAfter++] = '\n';
                        rowFound = true;
                        rowCount++;
                    }

                    if (buf[position] == '\n')
                    {
                        rowCount++;
                    }

                    bufAfterIns[positionAfter++] = buf[position++];
                }

                if (row == MAXDWORD)
                {
                    bufAfterIns[positionAfter++] = '\n';

                    for (int i = 0; i < strlen(cstr); i++)
                    {
                        bufAfterIns[positionAfter++] = cstr[i];
                    }

                    rowFound = true;
                    rowCount++;
                }

                bufAfterIns[positionAfter] = '\0';
            }

            if (rowFound)
            {
                if (SetFilePointer(of, 0, 0, FILE_BEGIN) == 0)
                {
                    if (WriteFile(of, bufAfterIns, strlen(bufAfterIns), &amountByte, NULL))
                    {
                        cout << "Row inserted successfully" << endl;
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
