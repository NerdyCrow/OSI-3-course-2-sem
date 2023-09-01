#pragma once

#ifdef OS_Lab11_HTAPI_EXPORTS
#define OS_Lab11_HTAPI __declspec(dllexport)
#else
#define OS_Lab11_HTAPI __declspec(dllimport)
#endif

#include <Windows.h>
#include<iomanip>
#include <iostream>

#define CREATE_HTFILE_ERROR "Create HTFile error";
#define CREATE_FILEMAPPING_ERROR "Create FileMapping error";
#define MAPPING_VIEW_ERROR "Mapping view failed";

namespace HT    // HT API
{
	//handle - это указатель на структуру, описывающую свойства какого - то конкретного объекта, 
	//например, какого-то окна, процесса, файла...Используется для изменения свойств этого объекта или получения сведений о его свойствах.

	// API HT - программный интерфейс для доступа к НТ-хранилищу 
	//          НТ-хранилище предназначено для хранения данных в ОП в формате ключ/значение
	//          Персистестеность (сохранность) данных обеспечивается с помощью snapshot-менханизма 
	//          Create - создать  и открыть HT-хранилище для использования   
	//          Open   - открыть HT-хранилище для использования
	//          Insert - создать элемент данных
	//          Delete - удалить элемент данных    
	//          Get    - читать  элемент данных
	//          Update - изменить элемент данных
	//          Snap   - выпонить snapshot
	//          Close  - выполнить Snap и закрыть HT-хранилище для использования
	//          GetLastError - получить сообщение о последней ошибке

	struct Thread {
		DWORD ID{};
		HANDLE handle{};
	};

	struct BetweenMemory {                    // between memory process for second or more
		int currentCapacity{};                      // current capacity
		int sizeElement{};                          // size ELEMENT
		int capacity{};                             // capacity
		int maxKeyLength{};                         // max length key
		int maxPayLoadLength{};                     // max length value
		int secSnapshotInterval{};                  // period save to disk
		int sizeMap{};
		BOOL stoped;
		OS_Lab11_HTAPI BetweenMemory();                     // default constructor
		OS_Lab11_HTAPI BetweenMemory(int sizeElement, int capacity, int maxKeyLength, int maxPayLoadLength);
	};

	struct HTHANDLE    // блок управления HT
	{
		OS_Lab11_HTAPI HTHANDLE();
		OS_Lab11_HTAPI HTHANDLE(
			int Capacity,
			int SecSnapshotInterval,
			int MaxKeyLength,
			int MaxPayloadLength,
			const wchar_t FileName[512]
		);
		wchar_t    FileName[512];          // имя файла 
		HANDLE  File;                   // File HANDLE != 0, если файл открыт
		HANDLE  FileMapping;            // Mapping File HANDLE != 0, если mapping создан  
		LPVOID  Addr;                   // Addr != NULL, если mapview выполнен  
		HT::BetweenMemory* shared;
		volatile  bool exit = false;
		char    LastErrorMessage[512];  // сообщение об последней ошибке или 0x00  
		time_t  lastsnaptime;           // дата последнего snap'a (time())  

		HANDLE Mutex;
		int N;							//текущее заполнение
		Thread SnapshotThread;
		[[nodiscard]] static int Alignment(int sizeElement, int maxKeyLength, int maxPayLoadLength);
		[[nodiscard]] LPVOID Offset(unsigned int index) const;
		void PrintSizeHT() const;
	};

	struct Element   // элемент 
	{
		int         keylength{};           // рахмер ключа
		int         payloadlength{};       // размер данных
		const void* key{};                 // значение ключа 
		const void* payload{};             // данные 
		Element();
		Element(const void* key, int keylength);                                             // for Get
		Element(const void* key, int keylength, const void* payload, int  payloadlength);    // for Insert
		Element(Element* oldelement, const void* newpayload, int  newpayloadlength);         // for update

		void WriteToMap(HTHANDLE* hthandle, Element* element);
		void FixPointer(int maxKeyLength);
	};

	Element * ElementGet(const void* key, int keylength);
	Element * ElementIns(const void* key, int keylength, const void* payload, int  payloadlength);
	Element * ElementUpd(Element * oldelement, const void* newpayload, int  newpayloadlength);


	HTHANDLE * Create   //  создать HT             
	(
		int	  Capacity,					   // емкость хранилища
		int   SecSnapshotInterval,		   // переодичность сохранения в сек.
		int   MaxKeyLength,                // максимальный размер ключа
		int   MaxPayloadLength,            // максимальный размер данных
		const wchar_t fileName[512]          // имя файла 
	); 	// != NULL успешное завершение  

	HTHANDLE * Start     //  открыть HT             
	(
		const wchar_t FileName[512]         // имя файла 
	); 	// != NULL успешное завершение  
	HTHANDLE * Open     //  открыть HT             
	(
		const wchar_t FileName[512]         // имя файла 
	); 	// != NULL успешное завершение  

	BOOL Snap         // выполнить Snapshot
	(
		HTHANDLE * hthandle           // управление HT (File, FileMapping)
	);


	BOOL Close        // Snap и закрыть HT  и  очистить HTHANDLE
	(
		HTHANDLE * hthandle           // управление HT (File, FileMapping)
	);	//  == TRUE успешное завершение   


	BOOL Insert      // добавить элемент в хранилище
	(
		HTHANDLE * hthandle,            // управление HT
		Element * element              // элемент
	);	//  == TRUE успешное завершение 


	BOOL Delete      // удалить элемент в хранилище
	(
		HTHANDLE * hthandle,            // управление HT (ключ)
		Element * element              // элемент 
	);	//  == TRUE успешное завершение 

	Element * Get     //  читать элемент в хранилище
	(
		HTHANDLE * hthandle,            // управление HT
		Element * element              // элемент 
	); 	//  != NULL успешное завершение 


	BOOL Update     //  именить элемент в хранилище
	(
		HTHANDLE * hthandle,            // управление HT
		Element * oldelement,          // старый элемент (ключ, размер ключа)
		const char* newpayload,          // новые данные  
		int       newpayloadlength     // размер новых данных
	); 	//  != NULL успешное завершение 

	char* GetLastErr  // получить сообщение о последней ошибке
	(
		HTHANDLE * ht                         // управление HT
	);

	void Print                               // распечатать элемент 
	(
		Element * element              // элемент 
	);
	DWORD WINAPI SnapThread(HTHANDLE*);
	unsigned int Hash(HTHANDLE*, const char*);
	void SetErrorMessage(HTHANDLE * ht, const char* message);
	BOOL CheckLengthElem(HTHANDLE * ht, Element * el);
};
