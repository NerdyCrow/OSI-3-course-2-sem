
#define _CRT_NON_CONFORMING_WCSTOK
#define _CRT_SECURE_NO_WARNINGS
#include "pch.h"

#include "HT.h"
#include <iostream>
#include <filesystem>
#include <Windows.h>
#include <tchar.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>
#include <Windows.h>

namespace ht
{
	HtHandle::HtHandle()
	{
		this->capacity = 0;
		this->secSnapshotInterval = 0;
		this->maxKeyLength = 0;
		this->maxPayloadLength = 0;
		ZeroMemory(this->fileName, sizeof(this->fileName));
		this->file = NULL;
		this->fileMapping = NULL;
		this->addr = NULL;
		ZeroMemory(this->lastErrorMessage, sizeof(this->lastErrorMessage));
		this->lastSnaptime = 0;

		this->count = 0;
	}

	HtHandle::HtHandle(int capacity, int secSnapshotInterval, int maxKeyLength, int maxPayloadLength, const wchar_t* fileName) : HtHandle()
	{
		this->capacity = capacity;
		this->secSnapshotInterval = secSnapshotInterval;
		this->maxKeyLength = maxKeyLength;
		this->maxPayloadLength = maxPayloadLength;
		memcpy(this->fileName, fileName, sizeof(this->fileName));
	}

	HtHandle* create(
		int	  capacity,					
		int   secSnapshotInterval,		
		int   maxKeyLength,            
		int   maxPayloadLength,			
		const wchar_t* fileName)		
	{
		HtHandle* htHandle = createHt(capacity, secSnapshotInterval, maxKeyLength, maxPayloadLength, fileName);
		if (htHandle!=NULL) { runSnapshotTimer(htHandle); }

		return htHandle;
	}

	HtHandle* createHt(
		int	  capacity,					
		int   secSnapshotInterval,		
		int   maxKeyLength,             
		int   maxPayloadLength,			
		const wchar_t* fileName)		
	{
		HANDLE hf = CreateFile(
			fileName,
			GENERIC_WRITE | GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE| FILE_SHARE_DELETE,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if (hf == INVALID_HANDLE_VALUE)
			return NULL;
			//throw "create or open file failed";

		int sizeMap = sizeof(HtHandle) + getSizeElement(maxKeyLength, maxPayloadLength) * capacity;
		HANDLE hm = CreateFileMapping(
			hf,
			NULL,
			PAGE_READWRITE,
			0, sizeMap,
			fileName);
		if (!hm)
			return NULL;
			//throw "create File Mapping failed";

		LPVOID lp = MapViewOfFile(
			hm,
			FILE_MAP_ALL_ACCESS,
			0, 0, 0);
		if (!lp)
			return NULL;

		ZeroMemory(lp, sizeMap);

		HtHandle* htHandle = new(lp) HtHandle(capacity, secSnapshotInterval, maxKeyLength, maxPayloadLength, fileName);
		htHandle->file = hf;
		htHandle->fileMapping = hm;
		htHandle->addr = lp;
		htHandle->lastSnaptime = time(NULL);
		htHandle->mutex = CreateMutex(
			NULL,
			FALSE,
			fileName);

		return htHandle;
	}

	void CALLBACK snapAsync(LPVOID prm, DWORD, DWORD)
	{
		HtHandle* htHandle = (HtHandle*)prm;
		if (snap(htHandle))
			std::cout << "-- spanshotAsync success" << std::endl;
	}

	HtHandle* open
	(
		const wchar_t* fileName,        
		bool isMapFile)					
	{
		HtHandle* htHandle;
		
			htHandle = openHtFromFile(fileName);
			if (htHandle)
				runSnapshotTimer(htHandle);
		

		return htHandle;
	}

	HtHandle* openHtFromFile(
		const wchar_t* fileName)
	{
		HANDLE hf = CreateFile(
			fileName,
			GENERIC_WRITE | GENERIC_READ,
			FILE_SHARE_READ| FILE_SHARE_WRITE|FILE_SHARE_DELETE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if (hf == INVALID_HANDLE_VALUE)
			return NULL;

		HANDLE hm = CreateFileMapping(
			hf,
			NULL,
			PAGE_READWRITE,
			0, 0,
			fileName);
		if (!hm)
			return NULL;

		LPVOID lp = MapViewOfFile(
			hm,
			FILE_MAP_ALL_ACCESS,
			0, 0, 0);
		if (!lp)
			return NULL;

		HtHandle* htHandle = (HtHandle*)lp;
		htHandle->file = hf;
		htHandle->fileMapping = hm;
		htHandle->addr = lp;
		htHandle->lastSnaptime = time(NULL);
		htHandle->mutex = CreateMutex(
			NULL,
			FALSE,
			fileName);

		return htHandle;
	}

	BOOL runSnapshotTimer(HtHandle* htHandle)
	{
		htHandle->snapshotTimer = CreateWaitableTimer(0, FALSE, 0);
		LARGE_INTEGER Li{};
		Li.QuadPart = -(SECOND * htHandle->secSnapshotInterval);
		SetWaitableTimer(htHandle->snapshotTimer, &Li, htHandle->secSnapshotInterval * 1000, snapAsync, htHandle, FALSE);

		return true;
	}

	Element* get(					 
		HtHandle* htHandle,            
		const Element* element)              
	{
		WaitForSingleObject(htHandle->mutex, INFINITE);
		int index = findIndex(htHandle, element);
		if (index < 0)
		{
			writeLastError(htHandle, "-- not found element (GET)");
			return NULL;
		}

		Element* foundElement = new Element();
		readFromMemory(htHandle, foundElement, index);
		ReleaseMutex(htHandle->mutex);

		return foundElement;
	}

	BOOL insert		
	(
		HtHandle* htHandle,            
		const Element* element)             
	{
		if (htHandle->count >= htHandle->capacity)
		{
			writeLastError(htHandle, "-- not found free memory");
			return false;
		}

		WaitForSingleObject(htHandle->mutex, INFINITE);
		int freeIndex = findFreeIndex(htHandle, element);
		if (freeIndex < 0)
			return false;

		writeToMemory(htHandle, element, freeIndex);
		incrementCount(htHandle);
		ReleaseMutex(htHandle->mutex);

		return true;
	}

	BOOL update     
	(
		HtHandle* htHandle,           
		const Element* oldElement,         
		const void* newPayload,         
		int             newPayloadLength)     
	{
		WaitForSingleObject(htHandle->mutex, INFINITE);
		int index = findIndex(htHandle, oldElement);
		if (index < 0)
		{
			writeLastError(htHandle, "-- not found element (UPDATE)");
			ReleaseMutex(htHandle->mutex);
			return false;
		}

		Element* updateElement = new Element(oldElement, newPayload, newPayloadLength);
		writeToMemory(htHandle, updateElement, index);
		ReleaseMutex(htHandle->mutex);

		return true;
	}

	BOOL removeOne      
	(
		HtHandle* htHandle,            
		const Element* element)				
	{
		WaitForSingleObject(htHandle->mutex, INFINITE);
		int index = findIndex(htHandle, element);
		if (index < 0)
		{
			writeLastError(htHandle, "-- not found element (DELETE)");
			ReleaseMutex(htHandle->mutex);
			return false;
		}

		clearMemoryByIndex(htHandle, index);
		decrementCount(htHandle);
		ReleaseMutex(htHandle->mutex);

		return true;
	}

	
	void print                              
	(
		const Element* element)            
	{
		std::cout << "Element:";
		std::cout << "key\": \"" << (char*)element->key << "\",";
		std::cout << "\"data\": \"" << (char*)element->payload << "\"," << std::endl;
	}

	BOOL close      
	(
		const HtHandle* htHandle)           
	{
		HANDLE hf = htHandle->file;
		HANDLE hfm = htHandle->fileMapping;
		HANDLE mutex = htHandle->mutex;

		if (htHandle->snapshotTimer)
			CancelWaitableTimer(htHandle->snapshotTimer);
		UnmapViewOfFile(htHandle->addr);
		CloseHandle(hfm);
		if (hf)
			CloseHandle(hf);
		if (mutex)
			CloseHandle(mutex);

		return true;
	}

	int hashFunction(const char* key, int capacity)
	{
		unsigned long i = 0;
		for (int j = 0; key[j]; j++)
			i += key[j];
		return i % capacity;
	}

	int nextHash(int currentHash, const char* key, int capacity)
	{
		return ++currentHash;
	}

	BOOL snap(HtHandle* htHandle)
	{
		WaitForSingleObject(htHandle->mutex, INFINITE);

		DWORD htSize = sizeof(HtHandle) + htHandle->capacity * (sizeof(int) * 2+htHandle->maxKeyLength + htHandle->maxPayloadLength) ;
		if (!FlushViewOfFile(htHandle, NULL))
		{
			writeLastError(htHandle, "-- snapshot error");
			ReleaseMutex(htHandle->mutex);
			return false;
		}

		time(&htHandle->lastSnaptime);
		char* buffer = new char[32];
		ctime_s(buffer, 32, &htHandle->lastSnaptime);

		tm* time = new tm();
		localtime_s(time, &htHandle->lastSnaptime);
		strftime(buffer, 32, "%d.%m.%Y-%H.%M.%S", time);
		delete time;
		std::wstring snapFileName = std::wstring(htHandle->fileName);

		const size_t cSize = strlen(buffer) + 1;
		wchar_t* wc = new wchar_t[cSize];
		std::mbstowcs(wc, buffer, cSize);
		snapFileName.replace(snapFileName.find_last_of('.'), 0, wc);
		delete[] buffer;
		
		HANDLE fileHandle = CreateFile((LPCWSTR)snapFileName.c_str(), GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (fileHandle == INVALID_HANDLE_VALUE)
		{
			writeLastError(htHandle, "-- snapshot error");
			ReleaseMutex(htHandle->mutex);
			return false;
		}

		WriteFile(fileHandle, htHandle, htSize, NULL, NULL);

		ReleaseMutex(htHandle->mutex);

		return true;
	}
	
	int findFreeIndex(
		const HtHandle* htHandle,
		const Element* element)
	{
		int index = hashFunction((char*)element->key, htHandle->capacity);
		int step = 1;

		while (true)
		{
			Element* foundElement = new Element();
			readFromMemory(htHandle, foundElement, index);

			if (foundElement->keyLength == NULL && foundElement->payloadLength == NULL)
			{
				delete foundElement;
				return index;
			}

			if (memcmp(foundElement->key, element->key, element->keyLength) == NULL)
			{
				delete foundElement;
				return -1;
			}

			index = (index + step) % htHandle->capacity;
			step++;
			delete foundElement;
		}

		return -1;
	}

	int findIndex(
		const HtHandle* htHandle,           
		const Element* element)				
	{
		int index = hashFunction((char*)element->key, htHandle->capacity);

		Element* foundElement = new Element();
		do
		{
			if (index >= htHandle->capacity)
			{
				index = -1;
				break;
			}

			readFromMemory(htHandle, foundElement, index);
			index = nextHash(index, (char*)element->key, htHandle->capacity);
		} while (
			memcmp(foundElement->key, element->key, element->keyLength) != NULL);

		delete foundElement;
		return index - 1;
	}

	BOOL writeToMemory(const HtHandle* const htHandle, const Element* const element, int index)
	{
		LPVOID lp = htHandle->addr;

		lp = (HtHandle*)lp + 1;
		lp = (byte*)lp + getSizeElement(htHandle->maxKeyLength, htHandle->maxPayloadLength) * index;

		memcpy(lp, element->key, element->keyLength);
		lp = (byte*)lp + htHandle->maxKeyLength;
		memcpy(lp, &element->keyLength, sizeof(int));
		lp = (int*)lp + 1;
		memcpy(lp, element->payload, element->payloadLength);
		lp = (byte*)lp + htHandle->maxPayloadLength;
		memcpy(lp, &element->payloadLength, sizeof(int));

		return true;
	}

	int incrementCount(HtHandle* const htHandle)
	{
		return ++htHandle->count;
	}

	Element* readFromMemory(const HtHandle* const htHandle, Element* const element, int index)
	{
		LPVOID lp = htHandle->addr;

		lp = (HtHandle*)lp + 1;
		lp = (byte*)lp + getSizeElement(htHandle->maxKeyLength, htHandle->maxPayloadLength) * index;

		element->key = lp;
		lp = (byte*)lp + htHandle->maxKeyLength;
		element->keyLength = *(int*)lp;
		lp = (int*)lp + 1;
		element->payload = lp;
		lp = (byte*)lp + htHandle->maxPayloadLength;
		element->payloadLength = *(int*)lp;

		return element;
	}

	BOOL clearMemoryByIndex(const HtHandle* const htHandle, int index)
	{
		LPVOID lp = htHandle->addr;
		int sizeElement = getSizeElement(htHandle->maxKeyLength, htHandle->maxPayloadLength);

		lp = (HtHandle*)lp + 1;
		lp = (byte*)lp + sizeElement * index;

		ZeroMemory(lp, sizeElement);

		return true;
	}

	int decrementCount(HtHandle* const htHandle)
	{
		return --htHandle->count;
	}

	const char* writeLastError(HtHandle* const htHandle, const char* msg)
	{
		memcpy(htHandle->lastErrorMessage, msg, sizeof(htHandle->lastErrorMessage));
		return htHandle->lastErrorMessage;
	}

	const char* getLastError  
	(
		const HtHandle* htHandle)				
	{
		return htHandle->lastErrorMessage;
	}
}
