#define _CRT_NON_CONFORMING_WCSTOK
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <Windows.h>
#include "HT.h"

using namespace std;
using namespace HT;

int main()
{
	setlocale(LC_ALL, "Russian");

	HTHANDLE* hashTable = nullptr;

	try
	{
		const wchar_t* FileName = (LPWSTR)L"D:\\3 kurs 2 sem\\OSI\\Lab10\\OS10_01_HT.ht";
		int input;

		cout << "1:Создать файл\n2:Открыть файл\n";
		cin >> input;

		if (input==1)
		{
			hashTable = Create(128, 1, 10, 50, FileName);
			if (!hashTable)
			{
				throw "Ошибка создания файла";
			}
			
		}
		else if(input == 2)
		{
			hashTable = Open(FileName);
			if (!hashTable)
			{
				throw "Ошибка открытия файла";
			}
		}

	

		Element* el1 = new Element("slemnyov", 9, "victor", 7);
		Element* el2 = new Element("belarus", 8, "minsk", 6);

		if (!Insert(hashTable, el1))
		{
			cout << hashTable->LastErrorMessage << endl;
		}

		if (!Insert(hashTable, el2))
		{
			cout << hashTable->LastErrorMessage << endl;
		}

		Element* el1Get = Get(hashTable, el1);

		if (el1Get != NULL)
		{
			print(el1Get);
		}

		Element* el2Get = Get(hashTable, el2);

		if (el2Get != NULL)
		{
			print(el2Get);
		}

		cout << "Update:";
		if (Update(hashTable, el1, "victorupd", 10))
		{
			cout << "after update:" << endl;
			Element* newel1 = new Element("slemnyov", 9, "victorupd", 10);
			Element* newel1Get = Get(hashTable, newel1);

			if (newel1Get != NULL)
			{
				print(newel1Get);
			}
			else
			{
				cout << hashTable->LastErrorMessage;
			}
		}
		else
		{
			cout << hashTable->LastErrorMessage;
		}

		cout << "Delete:";
		if (Delete(hashTable, el2))
		{
			cout << "After delete:" << endl;
			el1Get = Get(hashTable, el1);

			if (el1Get != NULL)
			{
				print(el1Get);
			}
			else
			{
				cout << hashTable->LastErrorMessage;
			}
		}
		else
		{
			cout << hashTable->LastErrorMessage;
		}

		Snap(hashTable);
		
		Close(hashTable);
	}
	catch (const char* msg)
	{
		cout << msg << endl;

		if (hashTable != nullptr)
		{
			cout << GetLastError(hashTable);
		}
	}
}

