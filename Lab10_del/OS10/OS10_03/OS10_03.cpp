#define _CRT_NON_CONFORMING_WCSTOK
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <Windows.h>
#pragma comment ( lib, "OS10_HTAPI.lib" )

#include "OS10_HTAPI.h"

using namespace std;
using namespace HT;

int main()
{
	setlocale(LC_ALL, "Russian");

	HTHANDLE* hashTable1 = nullptr;
	HTHANDLE* hashTable2 = nullptr;

	try
	{
		const wchar_t* FileName1 = (LPWSTR)L"D:\\3 kurs 2 sem\\OSI\\Lab10\\OS10_03_1_HT.ht";
		const wchar_t* FileName2 = (LPWSTR)L"D:\\3 kurs 2 sem\\OSI\\Lab10\\OS10_03_2_HT.ht";
		
		
		
			hashTable1 = Open(FileName1);
			if (!hashTable1)
			{
				hashTable1 = Create(128, 1, 10, 50, FileName1);
				if (!hashTable1) {
					throw "Ошибка создания файла";
				}
			}
		

			hashTable2 = Open(FileName2);
			if (!hashTable2)
			{
				hashTable2 = Create(128, 1, 10, 50, FileName2);
				if (!hashTable2) {
					throw "Ошибка создания файла";
				}
			}

		string key1 = "slemnyov";
		string data1 = "victor";
		string key2 = "belarus";
		string data2 = "minsk";

		Element* el1 = new Element(key1.c_str(), key1.length(), data1.c_str(), data1.length());
		Element* el2 = new Element(key2.c_str(), key2.length(), data2.c_str(), data2.length());

		if (!Insert(hashTable1, el1))
		{
			cout << hashTable1->LastErrorMessage << endl;
		}

		if (!Insert(hashTable2, el2))
		{
			cout << hashTable2->LastErrorMessage << endl;
		}

		if (Get(hashTable1, el1) != NULL)
		{
			cout << "HashTable1:";
			print(el1);
		
		}

		/*if (Get(hashTable1, el2) != NULL)
		{
			cout << "HashTable1:";
			print(el2);
		}*/

		/*if (Get(hashTable2, el1) != NULL)
		{
			cout << "HashTable2:";
			print(el1);
		}*/

		if (Get(hashTable2, el2) != NULL)
		{
			cout << "HashTable2:";
			print(el2);
		}
		
		Close(hashTable1);
		Close(hashTable2);
	}
	catch (const char* msg)
	{
		cout << msg << endl;

		if (hashTable1 != nullptr)
		{
			cout << GetLastError(hashTable1);
		}

		if (hashTable2 != nullptr)
		{
			cout << GetLastError(hashTable2);
		}
	}
}

