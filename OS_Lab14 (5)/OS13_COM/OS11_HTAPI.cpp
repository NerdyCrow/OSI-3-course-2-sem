// MathLibrary.cpp : Defines the exported functions for the DLL.
#include "pch.h" // use stdafx.h in Visual Studio 2017 and earlier
#include <utility>
#include <limits.h>
#include "OS11_HTAPI.h"
#include <algorithm>
#include "SEQLOG.h"
#include <LM.h>
#pragma comment (lib,"netapi32.lib")
//const wchar_t * path = L"C:\\Users\\37525\\OneDrive\\Рабочий стол\\OS_Lab14\\x64\\Debug\\test.txt";
extern HMODULE hmodule;

#define SNAP_TIME 3

namespace HT {

	HANDLE Addr;

	DWORD WINAPI SnapThread(HTHANDLE* ht);
	wchar_t* GenerateMutexName(const wchar_t* pathToHT);
	int Alignment(int sizeElement, int maxKeyLength, int maxPayLoadLength);

	BOOL isGroupExist(LPCWSTR groupName);
	BOOL isInUserGroups(LPCWSTR userName, LPCWSTR groupName);
	BOOL isCurrentGroup(LPCWSTR groupName);
	BOOL isUserRight(LPCWSTR name, LPCWSTR pass);

	Element::Element() {
		this->key = nullptr;
		this->keylength = NULL;
		this->payload = nullptr;
		this->payloadlength = NULL;
	}
	Element::Element(const void* key, int keylength) {
		Element();
		this->key = (void*)key;
		this->keylength = keylength;
	}
	Element::Element(const void* key, int keylength, const void* payload, int  payloadlength) {
		this->key = (void*)key;
		this->keylength = keylength;
		this->payload = (void*)payload;
		this->payloadlength = payloadlength;
	}
	Element::Element(Element* oldelement, const void* newpayload, int  newpayloadlength) {
		this->key = oldelement->key;
		this->keylength = oldelement->keylength;
		this->payload = (void*)newpayload;
		this->payloadlength = newpayloadlength;
	}

	Element* ElementGet(const void* key, int keylength) {
		return new Element(key, keylength);
	}
	Element* ElementIns(const void* key, int keylength, const void* payload, int  payloadlength) {
		return new Element(key, keylength, payload, payloadlength);
	}
	Element* ElementUpd(Element* oldelement, const void* newpayload, int  newpayloadlength) {
		return new Element(oldelement, newpayload, newpayloadlength);
	}


#pragma region Work with Handle(File)
	HTHANDLE::HTHANDLE() {
		this->shared = new BetweenMemory();
		this->shared->secSnapshotInterval = 3;
		memset(this->LastErrorMessage, 0, 512);
	};
	HTHANDLE::HTHANDLE(
		int Capacity,
		int SecSnapshotInterval,
		int MaxKeyLength,
		int MaxPayloadLength,
		const wchar_t FileName[512]
	) {
		this->shared = new BetweenMemory
		(Alignment(
			sizeof(Element),
			MaxKeyLength + 1,
			MaxPayloadLength + 1
		),
			Capacity,
			MaxPayloadLength + 1,
			MaxKeyLength + 1
		);
		this->shared->secSnapshotInterval = SecSnapshotInterval;
		memset(this->LastErrorMessage, 0, 512);
	}

	BetweenMemory::BetweenMemory(int sizeElement, int capacity, int maxKeyLength, int maxPayLoadLength) {
		this->currentCapacity = 0;
		this->sizeElement = sizeElement;
		this->capacity = capacity;
		this->maxKeyLength = maxKeyLength;
		this->maxPayLoadLength = maxPayLoadLength;
		this->secSnapshotInterval = SNAP_TIME;
		this->stoped = false;
	}

	BetweenMemory::BetweenMemory() {
		this->secSnapshotInterval = SNAP_TIME;
	}

	LPVOID HTHANDLE::Offset(unsigned int index) const {
		return (void*)((char*)this->Addr + (this->shared->sizeElement * index));
	}


	int HTHANDLE::Alignment(int sizeElement, int maxKeyLength, int maxPayLoadLength) {
		int size = sizeElement + (maxKeyLength + 1) + (maxPayLoadLength + 1); // +1 for '/0'
		if (size % 4 == 0)
			return size;
		else {
			return size + abs((long)size % 4 - 4);
		}
	}

	HTHANDLE* Create(
		int	  Capacity,							// емкость хранилища
		int   SecSnapshotInterval,				// переодичность сохранения в сек.
		int   MaxKeyLength,						// максимальный размер ключа
		int   MaxPayloadLength,					// максимальный размер данных
		const wchar_t FileName[512],			// имя файла 
		const wchar_t  HTUsersGroup[512]
	) {
		HTHANDLE* ht;
		DWORD MapSize = (HT::HTHANDLE::Alignment((int)sizeof(Element), MaxKeyLength, MaxPayloadLength)) * Capacity + (int)sizeof(BetweenMemory);

		if (!isGroupExist(HTUsersGroup) || !isCurrentGroup(HTUsersGroup) || !isCurrentGroup(L"Администраторы")) {
			std::cout << "conditions are not fulfilled";
			return NULL;
		}

		try {
			HANDLE hf = CreateFile(
				(LPCWSTR)FileName,						//path
				GENERIC_WRITE | GENERIC_READ,
				0,										//режим совместного пользования
				nullptr,								// атрибуты защиты
				//CREATE_NEW,							// создание 
				OPEN_ALWAYS, 							// создание 
				FILE_ATTRIBUTE_NORMAL,					//атрибуты и флаги
				nullptr									//файл атрибутов
			);
			if (hf == INVALID_HANDLE_VALUE) return NULL;


			HANDLE hm = CreateFileMapping(//создать объект ядра 
				hf,
				NULL,
				PAGE_READWRITE,
				0, MapSize, FileName); // L"Kir");
			if (!hm)return NULL;

			LPVOID lp = MapViewOfFile(
				hm,
				FILE_MAP_ALL_ACCESS,
				0, 0, 0);
			if (!lp)return NULL;

			ZeroMemory((char*)lp, MapSize + (int)sizeof(BetweenMemory));
			auto* shared = (BetweenMemory*)lp;
			HTHANDLE* ht = new HTHANDLE(
				Capacity,
				SecSnapshotInterval,
				MaxKeyLength,
				MaxPayloadLength,
				FileName);// = "..\\OS_Lab10\\OS1.xxx" );
			*shared = *ht->shared;
			ht->shared = shared;
			memcpy(ht->shared->HTUsersGroup, HTUsersGroup, sizeof(ht->shared->HTUsersGroup));
			ht->shared->sizeMap = MapSize;
			ht->File = hf;
			ht->FileMapping = hm;
			ht->Addr = (char*)lp + sizeof(BetweenMemory);
			ht->lastsnaptime = time(NULL);
			//ht->Mutex = CreateMutex(NULL, FALSE, L"Kir");// GenerateMutexName(FileName));	//MAX_PATH LIMIT

			//ht->SnapshotThread.handle = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)SnapThread, ht, 0, &ht->SnapshotThread.ID);
			return ht;
		}
		catch (const std::runtime_error& err) {
			std::cerr << "\nError: " << err.what() << std::endl;
			return nullptr;
		}
	}

	HTHANDLE* Create(
		int	  Capacity,							// емкость хранилища
		int   SecSnapshotInterval,				// переодичность сохранения в сек.
		int   MaxKeyLength,						// максимальный размер ключа
		int   MaxPayloadLength,					// максимальный размер данных
		const wchar_t FileName[512],			// имя файла 
		const wchar_t HTUsersGroup[512],
		const wchar_t userName[512],
		const wchar_t password[512]
	) {
		HTHANDLE* ht;
		DWORD MapSize = (HT::HTHANDLE::Alignment((int)sizeof(Element), MaxKeyLength, MaxPayloadLength)) * Capacity + (int)sizeof(BetweenMemory);

	/*	if (!isUserRight(userName, password)) {
			std::cout << "Invalid username or password";
			return NULL;
		}*/
		std::cout << isGroupExist(HTUsersGroup);
		std::cout << isInUserGroups(userName, HTUsersGroup);
		std::cout << isInUserGroups(userName, L"Администраторы");
		if (!isGroupExist(HTUsersGroup) || !isInUserGroups(userName, HTUsersGroup) || !isInUserGroups(userName, L"Администраторы")) {
			std::cout << "Groups error";
			return NULL;
		}

		try {
			HANDLE hf = CreateFile(
				(LPCWSTR)FileName,						//path
				GENERIC_WRITE | GENERIC_READ,
				0,										//режим совместного пользования
				nullptr,								// атрибуты защиты
				//CREATE_NEW,							// создание 
				OPEN_ALWAYS, 							// создание 
				FILE_ATTRIBUTE_NORMAL,					//атрибуты и флаги
				nullptr									//файл атрибутов
			);
			if (hf == INVALID_HANDLE_VALUE) return NULL;


			HANDLE hm = CreateFileMapping(//создать объект ядра 
				hf,
				NULL,
				PAGE_READWRITE,
				0, MapSize, FileName); // L"Kir");
			if (!hm)return NULL;

			LPVOID lp = MapViewOfFile(
				hm,
				FILE_MAP_ALL_ACCESS,
				0, 0, 0);
			if (!lp)return NULL;

			ZeroMemory((char*)lp, MapSize + (int)sizeof(BetweenMemory));
			auto* shared = (BetweenMemory*)lp;
			HTHANDLE* ht = new HTHANDLE(
				Capacity,
				SecSnapshotInterval,
				MaxKeyLength,
				MaxPayloadLength,
				FileName);// = "..\\OS_Lab10\\OS1.xxx" );
			*shared = *ht->shared;
			ht->shared = shared;
			memcpy(ht->shared->HTUsersGroup, HTUsersGroup, sizeof(ht->shared->HTUsersGroup));
			ht->shared->sizeMap = MapSize;
			ht->File = hf;
			ht->FileMapping = hm;
			ht->Addr = (char*)lp + sizeof(BetweenMemory);
			ht->lastsnaptime = time(NULL);
			//ht->Mutex = CreateMutex(NULL, FALSE, L"Kir");// GenerateMutexName(FileName));	//MAX_PATH LIMIT

			//ht->SnapshotThread.handle = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)SnapThread, ht, 0, &ht->SnapshotThread.ID);
			return ht;
		}
		catch (const std::runtime_error& err) {
			std::cerr << "\nError: " << err.what() << std::endl;
			return nullptr;
		}
	}

	HTHANDLE* Start(const wchar_t FileName[512]) {
		auto* ht = new HTHANDLE();
		BetweenMemory* shared{};
		HANDLE hm{};
		LPVOID lp{};
		try {
			HANDLE hf = CreateFile(
				(LPCWSTR)FileName,//path
				GENERIC_WRITE | GENERIC_READ,
				NULL,						// режим совместного пользования
				NULL,						// атрибуты защиты
				OPEN_EXISTING,				// открытие
				FILE_ATTRIBUTE_NORMAL,		// атрибуты и флаги
				NULL						// файл атрибутов
			);

			hm = CreateFileMapping(				//создать объект ядра 
				hf,
				NULL,
				PAGE_READWRITE,
				0, 0, FileName);
			if (!hm)return NULL;

			lp = MapViewOfFile(
				hm,
				FILE_MAP_ALL_ACCESS,
				0, 0, 0);
			if (!lp)return NULL;

			ht->shared = (BetweenMemory*)lp;
			/*ht->Mutex = CreateMutex(NULL, FALSE, L"Kir");
			ht->SnapshotThread.handle = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)SnapThread, ht, 0, &ht->SnapshotThread.ID);*/

			if (!isCurrentGroup(ht->shared->HTUsersGroup)) {
				std::cout << "User doenst belong to the group";
				//ht->shared->stoped = true;
				//Close(ht);
				return NULL;
			}

			return ht;
		}
		catch (const std::runtime_error& err) {
			std::cerr << "\nError: " << err.what() << std::endl;
			return nullptr;
		}
	}

	HTHANDLE* Start(
		const wchar_t FileName[512],
		const wchar_t userName[512],
		const wchar_t password[512]
	) {
		if (!isUserRight(userName, password)) {
			std::cout << "Invalid username or password";
			return NULL;
		}

		auto* ht = new HTHANDLE();
		BetweenMemory* shared{};
		HANDLE hm{};
		LPVOID lp{};
		try {
			HANDLE hf = CreateFile(
				(LPCWSTR)FileName,//path
				GENERIC_WRITE | GENERIC_READ,
				NULL,						// режим совместного пользования
				NULL,						// атрибуты защиты
				OPEN_EXISTING,				// открытие
				FILE_ATTRIBUTE_NORMAL,		// атрибуты и флаги
				NULL						// файл атрибутов
			);

			hm = CreateFileMapping(				//создать объект ядра 
				hf,
				NULL,
				PAGE_READWRITE,
				0, 0, FileName);
			if (!hm)return NULL;

			lp = MapViewOfFile(
				hm,
				FILE_MAP_ALL_ACCESS,
				0, 0, 0);
			if (!lp)return NULL;

			ht->shared = (BetweenMemory*)lp;
			/*ht->Mutex = CreateMutex(NULL, FALSE, L"Kir");
			ht->SnapshotThread.handle = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)SnapThread, ht, 0, &ht->SnapshotThread.ID);*/

			if (!isInUserGroups(userName, ht->shared->HTUsersGroup)) {
				std::cout << "User doenst belong to the group";
				Close((HTHANDLE*)lp);
				return NULL;
			}

			return ht;
		}
		catch (const std::runtime_error& err) {
			std::cerr << "\nError: " << err.what() << std::endl;
			return nullptr;
		}
	}

	HTHANDLE* Open(const wchar_t FileName[512]) {
		auto* ht = new HTHANDLE();
		BetweenMemory* shared{};
		HANDLE hm{};
		LPVOID lp{};
		bool canSnap = true;
		try {
			bool canSnap = false;

			HANDLE hm = CreateFileMapping(//создать объект ядра 
				INVALID_HANDLE_VALUE,
				NULL,
				PAGE_READWRITE,
				0, sizeof(BetweenMemory), FileName);
			if (!hm) {
				std::cout << "Map is not founded\n";
				return NULL;
			}

			LPVOID lp = MapViewOfFile(
				hm,
				FILE_MAP_ALL_ACCESS,
				0, 0, 0);
			if (!lp)return NULL;

			shared = (BetweenMemory*)lp;
			if (shared->capacity == 0)
				throw std::runtime_error("Map is empty\n");

			ht->shared = shared;

			hm = CreateFileMapping(		//создать объект ядра 
				INVALID_HANDLE_VALUE,
				NULL,
				PAGE_READWRITE,
				0, ht->shared->sizeMap, FileName);
			if (!hm)return NULL;

			lp = MapViewOfFile(
				hm,
				FILE_MAP_ALL_ACCESS,
				0, 0, 0);
			if (!lp)return NULL;

			shared = (BetweenMemory*)lp;
			if (shared->capacity == 0)
				throw std::runtime_error("Map is emty\n");

			ht->shared = shared;
			ht->FileMapping = hm;
			ht->Addr = (char*)lp + sizeof(BetweenMemory);
			//ht->Mutex = CreateMutex(NULL, FALSE, L"Kir");

			if (!isCurrentGroup(ht->shared->HTUsersGroup)) {
				std::cout << "User doenst belong to the group";
				//ht->shared->stoped = true;
				//Close(ht);
				return NULL;
			}
			
			return ht;
		}
		catch (const std::runtime_error& err) {
			std::cerr << "Error: " << err.what() << std::endl;
			return nullptr;
		}
	}

	HTHANDLE* Open(
		const wchar_t FileName[512],
		const wchar_t userName[512],
		const wchar_t password[512]
	) {
		if (!isUserRight(userName, password)) {
			std::cout << "Invalid username or password";
			return NULL;
		}

		auto* ht = new HTHANDLE();
		BetweenMemory* shared{};
		HANDLE hm{};
		LPVOID lp{};
		bool canSnap = true;
		try {
			bool canSnap = false;

			HANDLE hm = CreateFileMapping(//создать объект ядра 
				INVALID_HANDLE_VALUE,
				NULL,
				PAGE_READWRITE,
				0, sizeof(BetweenMemory), FileName);
			if (!hm) {
				std::cout << "Map is not founded\n";
				return NULL;
			}

			LPVOID lp = MapViewOfFile(
				hm,
				FILE_MAP_ALL_ACCESS,
				0, 0, 0);
			if (!lp)return NULL;

			shared = (BetweenMemory*)lp;
			if (shared->capacity == 0)
				throw std::runtime_error("Map is empty\n");

			ht->shared = shared;

			hm = CreateFileMapping(		//создать объект ядра 
				INVALID_HANDLE_VALUE,
				NULL,
				PAGE_READWRITE,
				0, ht->shared->sizeMap, FileName);
			if (!hm)return NULL;

			lp = MapViewOfFile(
				hm,
				FILE_MAP_ALL_ACCESS,
				0, 0, 0);
			if (!lp)return NULL;

			shared = (BetweenMemory*)lp;
			if (shared->capacity == 0)
				throw std::runtime_error("Map is emty\n");

			ht->shared = shared;
			ht->FileMapping = hm;
			ht->Addr = (char*)lp + sizeof(BetweenMemory);
			//ht->Mutex = CreateMutex(NULL, FALSE, L"Kir");

			if (!isInUserGroups(userName, ht->shared->HTUsersGroup)) {
				std::cout << "User doenst belong to the group";
				Close((HTHANDLE*)lp);
				return NULL;
			}

			return ht;
		}
		catch (const std::runtime_error& err) {
			std::cerr << "\nError: " << err.what() << std::endl;
			return nullptr;
		}
	}

	BOOL Snap(HTHANDLE* hthandle) {
		wchar_t name[100];
		std::wstring name_file;
		time_t t1;
		tm ttm{};
		DWORD bytesWritten;
		name_file += L"SnapFiles//";
		t1 = time(&t1);
		localtime_s(&ttm, &t1);
		int data[6] = { ttm.tm_mday, ttm.tm_mon, (1900 + ttm.tm_year), ttm.tm_hour, ttm.tm_min, ttm.tm_sec };

		for (size_t i = 0; i < _countof(data); i++)
		{
			swprintf_s(name, L"%d", data[i]);
			name_file += name;
			name_file += L".";
		}
		name_file += L"xxx";

		WaitForSingleObject(hthandle->Mutex, INFINITE);

		HANDLE file = CreateFile(
			(LPCWSTR)name_file.c_str(),
			GENERIC_WRITE,
			0,
			nullptr,
			CREATE_NEW,
			0,
			nullptr);

		if (file == INVALID_HANDLE_VALUE) {
			strcpy_s(hthandle->LastErrorMessage, (char*)"Error opening source \n");
		}

		bool writeFile = WriteFile(
			file,
			hthandle->shared,
			sizeof(BetweenMemory) + (hthandle->shared->capacity * hthandle->shared->sizeElement),
			&bytesWritten,
			nullptr
		);
		ReleaseMutex(hthandle->Mutex);

		if (writeFile) {
			return TRUE;
		}
		else {
			strcpy_s(hthandle->LastErrorMessage, (char*)"Error opening source ");
			return FALSE;
		}
	}

	BOOL Close        // Snap и закрыть HT  и  очистить HTHANDLE
	(
		HTHANDLE* hthandle           // управление HT (File, FileMapping)
	) {
		while (hthandle->shared->stoped == false) {
			Sleep(2000);
		}
		hthandle->exit = true;  // variable for exit thread for snap
		WaitForSingleObject(hthandle->SnapshotThread.handle, INFINITE);

		if (!FlushViewOfFile(hthandle->shared, hthandle->shared->sizeMap))
		{
			throw std::runtime_error("Close -> FlushViewOfFile -> error");
			strcpy_s(hthandle->LastErrorMessage, (char*)"Element not found\n");
			return FALSE;
		}

		if (!CloseHandle(hthandle->FileMapping))return FALSE;
		if (!CloseHandle(hthandle->File))return FALSE;
		//if (!CloseHandle(hthandle->Mutex))return FALSE;
		return TRUE;
	}
#pragma endregion

#pragma region Work with Element
	BOOL Insert      // добавить элемент в хранилище
	(
		HTHANDLE* hthandle,            // управление HT
		Element* element              // элемент
	) {//  == TRUE успешное завершение 
		try {
			if (!CheckLengthElem(hthandle, element)) {
				throw std::runtime_error("Key or value larger than available");
				return FALSE;
			}
			WaitForSingleObject(hthandle->Mutex, INFINITE);

			if (hthandle->shared->capacity == hthandle->shared->currentCapacity) {
				throw std::runtime_error("Table is full(((");
				ReleaseMutex(hthandle->Mutex);
				return FALSE;
			}
			auto hashKey = Hash(hthandle, (const char*)element->key);

			for (auto i = 0; i != hthandle->shared->capacity + 1; i++) {
				auto* temp = (Element*)hthandle->Offset(hashKey);

				if (temp->keylength == 0) {
					hthandle->shared->currentCapacity++;
					temp->WriteToMap(hthandle, element);
					std::cout << "Element insered";
					Print(element);
					hthandle->PrintSizeHT();
					ReleaseMutex(hthandle->Mutex);
					return TRUE;
				}
				else {
					temp->FixPointer(hthandle->shared->maxKeyLength);
					if (strcmp((char*)element->key, (char*)temp->key) == 0) {
						throw std::runtime_error("Element is exist\n");
					}
				}
				hashKey = (hashKey + 1) % hthandle->shared->capacity;
			}
			ReleaseMutex(hthandle->Mutex);
			return FALSE;
		}
		catch (std::runtime_error& error) {
			ReleaseMutex(hthandle->Mutex);
			SetErrorMessage(hthandle, error.what());
			return FALSE;
		}
	}

	BOOL Delete      // удалить элемент в хранилище
	(
		HTHANDLE* hthandle,				// управление HT (ключ)
		Element* element				// элемент 
	) {
		try {
			WaitForSingleObject(hthandle->Mutex, INFINITE);
			auto* item = Get(hthandle, element);
			if (item != nullptr) {
				item->keylength = 0;
				item->key = nullptr;
				ZeroMemory(item, hthandle->shared->sizeElement);
				hthandle->shared->currentCapacity--;
				ReleaseMutex(hthandle->Mutex);
				return TRUE;
			}
			throw std::runtime_error("Sorry I not found this element");
		}
		catch (std::runtime_error& error) {
			ReleaseMutex(hthandle->Mutex);
			SetErrorMessage(hthandle, error.what());
			return FALSE;
		}
	}	//  == TRUE успешное завершение 

	Element* Get     //  читать элемент в хранилище
	(
		HTHANDLE* hthandle,				// управление HT
		Element* element				// элемент 
	) {
		try {
			if (!CheckLengthElem(hthandle, element)) {
				throw std::runtime_error("Key or value larger than available");
				return nullptr;
			}

			WaitForSingleObject(hthandle->Mutex, INFINITE);
			auto hashKey = Hash(hthandle, (const char*)element->key);
			for (size_t i = 0; i < hthandle->shared->capacity; i++)
			{
				auto* item = (Element*)hthandle->Offset(hashKey);
				item->FixPointer(hthandle->shared->maxKeyLength);
				if (item->key != nullptr && item->keylength != 0) {
					if (strcmp((char*)item->key, (char*)element->key) == 0) {
						ReleaseMutex(hthandle->Mutex);
						return item;
					}
				}
				hashKey = (hashKey + 1) % hthandle->shared->capacity;
			}
			ReleaseMutex(hthandle->Mutex);
			throw std::runtime_error("This element not founded");
			return nullptr;
		}
		catch (std::runtime_error& error) {
			ReleaseMutex(hthandle->Mutex);
			SetErrorMessage(hthandle, error.what());
			return FALSE;
		}

	} 	//  != NULL успешное завершение 


	BOOL Update     //  именить элемент в хранилище
	(
		HTHANDLE* hthandle,					// управление HT
		Element* oldelement,				// старый элемент (ключ, размер ключа)
		const char* newpayload,				// новые данные  
		int newpayloadlength				// размер новых данных
	) {
		try {
			if (!CheckLengthElem(hthandle, oldelement)) {
				throw std::runtime_error("Key or value larger than available");
				return FALSE;
			}

			WaitForSingleObject(hthandle->Mutex, INFINITE);
			auto* item = Get(hthandle, oldelement);
			if (item != nullptr) {
				strcpy_s((char*)item->payload, newpayloadlength + 1, (char*)newpayload);
				ReleaseMutex(hthandle->Mutex);
				return TRUE;
			}

			throw std::runtime_error("Sorry I not found this element");
			ReleaseMutex(hthandle->Mutex);
			return FALSE;
		}
		catch (std::runtime_error& error) {
			ReleaseMutex(hthandle->Mutex);
			SetErrorMessage(hthandle, error.what());
			return FALSE;
		}
	} 	//  != NULL успешное завершение 

	void Print(Element* element) {
		if (element->keylength == 0)
			return;
		char* key = (char*)element->key;
		std::cout << std::endl << "KEY  :  " << key << std::endl;
		char* payload = (char*)element->payload;
		std::cout << "VALUE:  " << payload << std::endl << std::endl;
	}

	unsigned int Hash(HTHANDLE* hthandle, const char* key) {
		std::string_view temp{ key };
		std::hash< std::string_view> hashing;
		return hashing(temp) % hthandle->shared->capacity;
	}

	void Element::WriteToMap(HTHANDLE* hthandler, Element* element) {
		this->keylength = element->keylength;
		this->payloadlength = element->payloadlength;
		FixPointer(hthandler->shared->maxKeyLength);
		strcpy_s((char*)this->key, this->keylength + 1, (const char*)element->key);
		strcpy_s((char*)this->payload, this->payloadlength + 1, (const char*)element->payload);
	};

	void Element::FixPointer(int maxKeyLength) {
		this->key = (char*)this + sizeof(Element);
		this->payload = (char*)this + sizeof(Element) + maxKeyLength;
	}

#pragma endregion

	char* GetLastErr(HTHANDLE* ht) {
		return ht->LastErrorMessage;
	}

	void SetErrorMessage(HTHANDLE* ht, const char* message) {
		int len = strlen(message) + 1;
		char* err = (char*)message;
		memcpy(ht->LastErrorMessage, err, len);
	}

	BOOL CheckLengthElem(HTHANDLE* ht, Element* el) {
		if (el->keylength > ht->shared->maxKeyLength ||
			el->payloadlength > ht->shared->maxPayLoadLength) {
			return FALSE;
		}
		return TRUE;
	}

	DWORD WINAPI SnapThread(HTHANDLE* hthandle) {
		HANDLE mutex = CreateMutex(nullptr, FALSE, L"SNAP");

		while (!hthandle->exit) {
			WaitForSingleObject(mutex, INFINITE);
			Sleep(hthandle->shared->secSnapshotInterval * 1000);
			Snap(hthandle);
			ReleaseMutex(mutex);
		}
		return 0;
	}

	void HTHANDLE::PrintSizeHT() const {
		std::cout << "Size: " << this->shared->capacity << "/" << this->shared->currentCapacity << std::endl;
	}

	void PrintAll(HTHANDLE* hthandle) {
		std::cout.setf(std::ios::left);
		std::cout.width(hthandle->shared->maxKeyLength);
		std::cout << "KEY";
		std::cout.setf(std::ios::left);
		std::cout.width(hthandle->shared->maxPayLoadLength);
		std::cout << "VALUE" << std::endl;

		for (int i = 0; i <= hthandle->shared->capacity; i++) {
			auto* element = (Element*)hthandle->Offset(i);
			if (element->keylength != 0) {
				element->FixPointer(hthandle->shared->maxKeyLength);
				std::cout.setf(std::ios::left);
				std::cout.width(hthandle->shared->maxKeyLength);
				std::cout << (char*)element->key;
				std::cout.width(hthandle->shared->maxPayLoadLength);
				std::cout << (char*)element->payload << std::endl;
			}
		}
	}

	BOOL  isGroupExist(LPCWSTR groupName) {
		GROUP_INFO_0* groupsInfo;
		DWORD readed = 0, total = 0;
		NetLocalGroupEnum(
			NULL,
			0,
			(LPBYTE*)&groupsInfo,
			MAX_PREFERRED_LENGTH,
			&readed,
			&total,
			NULL
		);
		bool exosts = false;
		for (int i = 0; i < readed; i++) {
			int res = lstrcmpW(groupName, groupsInfo[i].grpi0_name);
			if (res == 0) {
				exosts = true;
				break;
			}
		}
		NetApiBufferFree((LPVOID)groupsInfo);
		return exosts;
	}

	BOOL isInUserGroups(LPCWSTR userName, LPCWSTR groupName) {
		GROUP_USERS_INFO_0* groupUsersInfo;
		DWORD uc = 0, tc = 0;
		NET_API_STATUS ns = NetUserGetLocalGroups(
			NULL,
			userName,
			0,
			LG_INCLUDE_INDIRECT,
			(LPBYTE*)&groupUsersInfo,
			MAX_PREFERRED_LENGTH,
			&uc,
			&tc
		);
		bool exosts = false;
		if (ns == NERR_Success) {
			for (int i = 0; i < uc; i++) {
				int res = lstrcmpW(groupName, groupUsersInfo[i].grui0_name);
				if (res == 0) {
					exosts = true;
					break;
				}
			}
			NetApiBufferFree((LPVOID)groupUsersInfo);
		}
		return exosts;
	}

	BOOL isCurrentGroup(LPCWSTR groupName) {
		WCHAR currentUserName[512];
		DWORD lenUserName = 512;
		GetUserName(currentUserName, &lenUserName);
		std::wcout << "name: " << currentUserName << std::endl;
		return isInUserGroups(currentUserName, groupName);
	}

	BOOL  isUserRight(LPCWSTR name, LPCWSTR pass) {
		bool res;
		HANDLE hToken = 0;
		res = LogonUserW(
			name,
			L".",
			pass,
			LOGON32_LOGON_INTERACTIVE,
			LOGON32_PROVIDER_DEFAULT,
			&hToken
		);
		CloseHandle(hToken);
		return res;
	}
}