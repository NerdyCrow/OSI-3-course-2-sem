#define _CRT_NON_CONFORMING_WCSTOK
#define _CRT_SECURE_NO_WARNINGS

#include "pch.h"
#include <iostream>
#include <filesystem>
#include <Windows.h>
#include <tchar.h>
#include <string>
#include "OS10_HTAPI.h"

using namespace std;

namespace HT
{
	void SetErrorMessage(HTHANDLE* ht, string msg);

	HTHANDLE::HTHANDLE() { }

	HTHANDLE::HTHANDLE(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const wchar_t FileName[512])
	{
		this->Capacity = Capacity;
		this->SecSnapshotInterval = SecSnapshotInterval;
		this->MaxKeyLength = MaxKeyLength;
		this->MaxPayloadLength = MaxPayloadLength;
		wcscpy_s(this->FileName, _countof(this->FileName), FileName);
	}

	Element::Element() { }

	Element::Element(const void* key, int keylength)
	{
		this->keylength = keylength;
		this->key = (void*)key;
	}

	Element::Element(const void* key, int keylength, const void* payload, int payloadlength)
	{
		this->keylength = keylength;
		this->key = (void*)key;
		this->payloadlength = payloadlength;
		this->payload = (void*)payload;
	}

	Element::Element(Element* oldelement, const void* newpayload, int  newpayloadlength)
	{
		this->payloadlength = newpayloadlength;
		this->payload = (void*)newpayload;
		this->keylength = oldelement->keylength;
		this->key = oldelement->key;
	}

	HTHANDLE* Create(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const wchar_t FileName[512])
	{
		HANDLE fileHandle = NULL;
		HANDLE mapHandle = NULL;
		HANDLE mutexHandle = NULL;
		DWORD hashTableSize = sizeof(HTHANDLE) + Capacity * (sizeof(Element) + MaxKeyLength + MaxPayloadLength);
		char* file = new char[512];

		try
		{
			fileHandle = CreateFile(FileName, GENERIC_READ | GENERIC_WRITE, NULL, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
			if (fileHandle == NULL || (ULONGLONG)fileHandle == ULLONG_MAX)
			{
				throw ERROR_CREATE_FILE;
			}

			mapHandle = CreateFileMapping(fileHandle, NULL, PAGE_READWRITE, 0, hashTableSize, NULL);
			if (mapHandle == NULL)
			{
				throw ERROR_CREATE_FILE_MAPPING;
			}

			LPVOID lp = MapViewOfFile(mapHandle, FILE_MAP_ALL_ACCESS, 0, 0, hashTableSize);
			if (lp == NULL)
			{
				throw ERROR_MAP_VIEW;
			}

			wcstombs(file, FileName, wcslen(FileName));

			mutexHandle = CreateMutex(NULL, FALSE, (LPCWSTR)file);
			if (mutexHandle == NULL)
			{
				throw ERROR_CREATE_MUTEX;
			}

			HTHANDLE* hashTableHandle = new(lp) HTHANDLE(Capacity, SecSnapshotInterval, MaxKeyLength, MaxPayloadLength, FileName);

			hashTableHandle->Addr = (unsigned char*)lp + sizeof(HTHANDLE);
			hashTableHandle->File = fileHandle;
			hashTableHandle->FileMapping = mapHandle;
			hashTableHandle->Mutex = mutexHandle;

			hashTableHandle->SnapThread = CreateThread(NULL, 0, SnapWithInterval, hashTableHandle, 0, NULL);
			if (hashTableHandle->SnapThread == NULL)
			{
				throw ERROR_CREATE_SNAP_THREAD;
			}

			if (!SaveParametersToFile(Capacity, SecSnapshotInterval, MaxKeyLength, MaxPayloadLength, FileName))
			{
				throw ERROR_CREATE_CONFIG_FILE;
			}

			return hashTableHandle;
		}
		catch (const char* err) {
			cout << err << endl;

			CloseHandle(fileHandle);
			CloseHandle(mapHandle);
			CloseHandle(mutexHandle);

			return NULL;
		}
	}

	HTHANDLE* Open(const wchar_t FileName[512])
	{
		HANDLE fileHandle = NULL;
		HANDLE mapHandle = NULL;
		HANDLE mutexHandle = NULL;
		char* file = new char[512];

		try
		{
			fileHandle = CreateFile(FileName, GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (fileHandle == NULL || (ULONGLONG)fileHandle == ULLONG_MAX)
			{
				throw ERROR_OPEN_FILE;
			}

			mapHandle = CreateFileMapping(fileHandle, NULL, PAGE_READWRITE, 0, 0, NULL);
			if (mapHandle == NULL)
			{
				throw ERROR_CREATE_FILE_MAPPING;
			}

			LPVOID lp = MapViewOfFile(mapHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
			if (lp == NULL)
			{
				throw ERROR_MAP_VIEW;
			}

			HTHANDLE* hashTableHandle = (HTHANDLE*)lp;

			wcstombs(file, FileName, wcslen(FileName));

			mutexHandle = CreateMutex(NULL, FALSE, (LPCWSTR)file);
			if (mutexHandle == NULL)
			{
				throw ERROR_CREATE_MUTEX;
			}

			hashTableHandle->Addr = (unsigned char*)lp + sizeof(HTHANDLE);
			hashTableHandle->File = fileHandle;
			hashTableHandle->Mutex = mutexHandle;
			hashTableHandle->FileMapping = mapHandle;

			unsigned char* pointer = (unsigned char*)hashTableHandle->Addr;
			for (int i = 0; i < hashTableHandle->Capacity; i++)
			{
				unsigned char* currentPointer = pointer + (RECORD_SIZE * i);
				Element* currentElement = (Element*)currentPointer;
				currentElement->key = currentPointer + sizeof(Element);
				currentElement->payload = currentPointer + sizeof(Element) + hashTableHandle->MaxKeyLength;
			}

			hashTableHandle->SnapThread = CreateThread(NULL, 0, SnapWithInterval, hashTableHandle, 0, NULL);
			if (hashTableHandle->SnapThread == NULL)
			{
				throw ERROR_CREATE_SNAP_THREAD;
			}

			return hashTableHandle;
		}
		catch (const char* err) {
			cout << err << endl;

			CloseHandle(fileHandle);
			CloseHandle(mapHandle);
			CloseHandle(mutexHandle);

			return NULL;
		}
	}

	BOOL Insert(HTHANDLE* hashTableHandle, Element* element)
	{
		WaitForSingleObject(hashTableHandle->Mutex, INFINITE);

		if (hashTableHandle->CurrentCapacity == hashTableHandle->Capacity)
		{
			ReleaseMutex(hashTableHandle->Mutex);
			SetErrorMessage((HTHANDLE*)hashTableHandle, ERROR_TABLE_SIZE);
			return false;
		}

		if (element->keylength > hashTableHandle->MaxKeyLength || element->keylength <= 0)
		{
			ReleaseMutex(hashTableHandle->Mutex);
			SetErrorMessage((HTHANDLE*)hashTableHandle, ERROR_KEY_SIZE);
			return false;
		}

		if (element->payloadlength > hashTableHandle->MaxPayloadLength || element->payloadlength <= 0)
		{
			ReleaseMutex(hashTableHandle->Mutex);
			SetErrorMessage((HTHANDLE*)hashTableHandle, ERROR_PAYLOAD_SIZE);
			return false;
		}

		unsigned char* pointer = (unsigned char*)hashTableHandle->Addr;
		int elementHash = HashFunction(element, hashTableHandle->Capacity);

		for (int i = 0; i < hashTableHandle->Capacity; i++)
		{
			unsigned char* currentPointer = pointer + ((elementHash + i) % hashTableHandle->Capacity * RECORD_SIZE);
			Element* currentElement = (Element*)currentPointer;

			if (currentElement->keylength == element->keylength && strcmp((char*)currentElement->key, (char*)element->key) == 0)
			{
				ReleaseMutex(hashTableHandle->Mutex);
				SetErrorMessage((HTHANDLE*)hashTableHandle, ERROR_UNIQUE_ELEMENT);
				return false;
			}

			if (currentElement->keylength == NULL)
			{
				currentElement->key = currentPointer + sizeof(Element);
				currentElement->payload = currentPointer + sizeof(Element) + hashTableHandle->MaxKeyLength;
				currentElement->keylength = element->keylength;
				currentElement->payloadlength = element->payloadlength;
				CopyMemory(currentElement->key, element->key, element->keylength);
				CopyMemory(currentElement->payload, element->payload, element->payloadlength);

				hashTableHandle->CurrentCapacity++;
				ReleaseMutex(hashTableHandle->Mutex);
				return true;
			}
		}

		ReleaseMutex(hashTableHandle->Mutex);
		return false;
	}

	Element* Get(const HTHANDLE* hashTableHandle, const Element* element)
	{
		WaitForSingleObject(hashTableHandle->Mutex, INFINITE);

		unsigned char* pointer = (unsigned char*)hashTableHandle->Addr;
		int elementHash = HashFunction(element, hashTableHandle->Capacity);

		for (int i = 0; i < hashTableHandle->Capacity; i++)
		{
			unsigned char* currentPointer = pointer + ((elementHash + i) % hashTableHandle->Capacity * RECORD_SIZE);
			Element* currentElement = (Element*)currentPointer;

			if (currentElement->key == NULL)
			{
				ReleaseMutex(hashTableHandle->Mutex);
				SetErrorMessage((HTHANDLE*)hashTableHandle, ERROR_DATA_NOT_FOUND);
				return NULL;
			}

			if (currentElement->keylength == element->keylength &&
				strcmp((char*)currentElement->key, (char*)element->key) == 0)
			{
				ReleaseMutex(hashTableHandle->Mutex);
				return currentElement;
			}
		}

		ReleaseMutex(hashTableHandle->Mutex);
		SetErrorMessage((HTHANDLE*)hashTableHandle, ERROR_DATA_NOT_FOUND);
		return NULL;
	}

	BOOL Update(const HTHANDLE* hashTableHandle, const Element* oldElement, const void* newPayload, int newPayloadLength)
	{
		WaitForSingleObject(hashTableHandle->Mutex, INFINITE);

		Element* element = Get(hashTableHandle, oldElement);
		if (element == NULL)
		{
			ReleaseMutex(hashTableHandle->Mutex);
			SetErrorMessage((HTHANDLE*)hashTableHandle, ERROR_DATA_NOT_FOUND);
			return false;
		}

		element->payloadlength = newPayloadLength;
		CopyMemory(element->payload, newPayload, newPayloadLength);

		ReleaseMutex(hashTableHandle->Mutex);

		return true;
	}

	BOOL Delete(HTHANDLE* hashTableHandle, Element* element)
	{
		WaitForSingleObject(hashTableHandle->Mutex, INFINITE);

		Element* deletedElement = Get(hashTableHandle, element);
		if (deletedElement == NULL)
		{
			ReleaseMutex(hashTableHandle->Mutex);
			SetErrorMessage((HTHANDLE*)hashTableHandle, ERROR_DATA_NOT_FOUND);
			return false;
		}

		ZeroMemory(deletedElement, RECORD_SIZE);
		hashTableHandle->CurrentCapacity--;
		ReleaseMutex(hashTableHandle->Mutex);

		return true;
	}

	BOOL Close(const HTHANDLE* hashTableHandle)
	{
		WaitForSingleObject(hashTableHandle->Mutex, INFINITE);
		TerminateThread(hashTableHandle->SnapThread, 0);

		if (CloseHandle(hashTableHandle->SnapThread) &&
			CloseHandle(hashTableHandle->FileMapping) &&
			CloseHandle(hashTableHandle->File) &&
			ReleaseMutex(hashTableHandle->Mutex) &&
			CloseHandle(hashTableHandle->Mutex) &&
			UnmapViewOfFile(hashTableHandle)
			)
		{
			return true;
		}

		return false;
	}

	BOOL Snap(HTHANDLE* hashTableHandle)
	{
		WaitForSingleObject(hashTableHandle->Mutex, INFINITE);

		DWORD htSize = sizeof(HTHANDLE) + hashTableHandle->CurrentCapacity * RECORD_SIZE;
		if (!FlushViewOfFile(hashTableHandle, htSize))
		{
			SetErrorMessage(hashTableHandle, ERROR_FLUSH_VIEW);
			ReleaseMutex(hashTableHandle->Mutex);
			return false;
		}

		time(&hashTableHandle->lastsnaptime);
		char* buffer = new char[32];
		ctime_s(buffer, 32, &hashTableHandle->lastsnaptime);

		tm* time = new tm();
		localtime_s(time, &hashTableHandle->lastsnaptime);
		strftime(buffer, 32, "%d.%m.%Y-%H.%M.%S", time);
		delete time;
		wstring snapFileName = wstring(hashTableHandle->FileName);

		const size_t cSize = strlen(buffer) + 1;
		wchar_t* wc = new wchar_t[cSize];
		mbstowcs(wc, buffer, cSize);

		snapFileName.replace(snapFileName.find('.'), 0, wc);
		delete[] buffer;

		HANDLE fileHandle = CreateFile((LPCWSTR)snapFileName.c_str(), GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (fileHandle == INVALID_HANDLE_VALUE)
		{
			SetErrorMessage(hashTableHandle, ERROR_CREATE_SNAPSHOT);
			ReleaseMutex(hashTableHandle->Mutex);
			return false;
		}

		WriteFile(fileHandle, hashTableHandle, htSize, NULL, NULL);

		ReleaseMutex(hashTableHandle->Mutex);

		return true;
	}

	char* GetLastError(HTHANDLE* ht)
	{
		return ht->LastErrorMessage;
	}

	void print(const Element* element)
	{
		printf("Key: %s, data: %s\n", (char*)element->key, (char*)element->payload);
	}

	void SetErrorMessage(HTHANDLE* ht, string msg)
	{
		strcpy_s(ht->LastErrorMessage, msg.length() + 1, msg.c_str());
	}

	DWORD WINAPI SnapWithInterval(LPVOID hashTableHandle)
	{
		HTHANDLE* hashTable = (HTHANDLE*)hashTableHandle;
		while (true)
		{
			Sleep(hashTable->SecSnapshotInterval * 1000);
			Snap(hashTable);
		}
	}

	BOOL SaveParametersToFile(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const wchar_t FileName[512])
	{
		HANDLE fileHandle = NULL;
		string strBuf = to_string(Capacity) + " " + to_string(SecSnapshotInterval) + " " + to_string(MaxKeyLength) + " "
			+ to_string(MaxPayloadLength);
		char* buf = (char*)strBuf.c_str();

		try
		{
			fileHandle = CreateFile((LPCWSTR)(wstring(FileName) + L"c").c_str(), GENERIC_READ | GENERIC_WRITE, NULL, NULL,
				CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
			if (fileHandle == NULL || (ULONGLONG)fileHandle == ULLONG_MAX)
			{
				throw ERROR_CREATE_CONFIG_FILE;
			}

			if (!WriteFile(fileHandle, buf, strlen(buf), NULL, NULL))
			{
				throw ERROR_WRITE_CONFIG_FILE;
			}

			return true;
		}
		catch (const char* err) {
			cout << err << endl;
			CloseHandle(fileHandle);
			return false;
		}
	}

	int HashFunction(const Element* element, int HTCapacity)
	{
		int hash = 0;

		for (int i = 0; i < element->keylength; i++)
		{
			hash = (hash * 31 + ((char*)element->key)[i]) % HTCapacity;
		}

		return hash;
	}
}