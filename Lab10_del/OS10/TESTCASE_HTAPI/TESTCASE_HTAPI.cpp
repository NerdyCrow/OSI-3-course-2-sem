#define _CRT_NON_CONFORMING_WCSTOK
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <Windows.h>
#include <string>
#include "OS10_HTAPI.h"

using namespace std;
using namespace HT;

string create_test()
{
	const wchar_t* FileName = (LPWSTR)L"D:\\3 kurs 2 sem\\OSI\\Lab10\\TESTCASE_HTAPI.ht";
	HTHANDLE* hashTable1 = Create(128, 1, 10, 20, FileName);

	if (!hashTable1)
	{
		return "пройден";
	}

	Close(hashTable1);
	return "не пройден";
}

string open_test()
{
	const wchar_t* FileName = (LPWSTR)L"D:\\3 kurs 2 sem\\OSI\\Lab10\\TESTCASE_HTAPI.ht";
	HTHANDLE* hashTable2 = Open(FileName);

	if (!hashTable2)
	{
		return "пройден";
	}

	Close(hashTable2);
	return "не пройден";
}

string insert_test(HTHANDLE* hashTable1) 
{
	string key1 = "slemnyov";
	string data1 = "victor";
	Element* el1 = new Element(key1.c_str(), key1.length(), data1.c_str(), data1.length());

	Insert(hashTable1, el1);
	Insert(hashTable1, el1);

	return strcmp(HT::GetLastError(hashTable1), ERROR_UNIQUE_ELEMENT) == 0 ? "пройден" : "не пройден";
}

string keysize_test(HTHANDLE* hashTable1) 
{
	string key1 = "victor slemnev1235";
	string data1 = "victor";
	Element* el1 = new Element(key1.c_str(), key1.length(), data1.c_str(), data1.length());

	Insert(hashTable1, el1);

	return strcmp(HT::GetLastError(hashTable1), ERROR_KEY_SIZE) == 0 ? "пройден" : "не пройден";
}

string datasize_test(HTHANDLE* hashTable1) 
{
	string key1 = "belarus";
	string data1 = "minsk is the capital of belarus. Hello world";
	Element* el1 = new Element(key1.c_str(), key1.length(), data1.c_str(), data1.length());

	Insert(hashTable1, el1);

	return strcmp(HT::GetLastError(hashTable1), ERROR_PAYLOAD_SIZE) == 0 ? "пройден" : "не пройден";
}

string tablesize_test(HTHANDLE* hashTable1) 
{
	string key;
	string data1 = "minsk";

	for (size_t i = 0; i < hashTable1->Capacity; i++)
	{
		key = to_string(i);
		Insert(hashTable1, new Element(key.c_str(), key.length(), data1.c_str(), data1.length()));
	}

	key = "default";
	Insert(hashTable1, new Element(key.c_str(), key.length(), data1.c_str(), data1.length()));

	return strcmp(HT::GetLastError(hashTable1), ERROR_TABLE_SIZE) == 0 ? "пройден" : "не пройден";
}



int main()
{
	setlocale(LC_ALL, "Russian");

	const wchar_t* FileName = (LPWSTR)L"D:\\3 kurs 2 sem\\OSI\\Lab10\\TESTCASE_HTAPI.ht";
	HTHANDLE* hashTable1 = Create(128, 1, 10, 20, FileName);

	cout << "create test: " << create_test() << endl;
	cout << "open test: " << open_test() << endl;
	cout << "insert test: " << insert_test(hashTable1) << endl;
	cout << "key size test: " << keysize_test(hashTable1) << endl;
	cout << "data size test: " << datasize_test(hashTable1) << endl;
	cout << "table size test: " << tablesize_test(hashTable1) << endl;
	

	Close(hashTable1);
	return 0;
}

