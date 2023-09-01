#pragma once
#include <Windows.h>

#define RECORD_SIZE					(sizeof(Element) + hashTableHandle->MaxKeyLength + hashTableHandle->MaxPayloadLength)
#define ERROR_CREATE_FILE			"Ошибка создания файла"
#define ERROR_CREATE_CONFIG_FILE	"Ошибка создания файла конфигураций"
#define ERROR_WRITE_CONFIG_FILE		"Ошибка записи в файл конфигураций"
#define ERROR_OPEN_FILE				"Ошибка открытия файла"
#define ERROR_CREATE_FILE_MAPPING	"Ошибка создания объекта файлового отображения"
#define ERROR_MAP_VIEW				"Ошибка отображения файла в память"
#define ERROR_CREATE_MUTEX			"Ошибка создания мьютекса"
#define ERROR_CREATE_SNAP_THREAD	"Ошибка создания SnapThread"
#define ERROR_DATA_NOT_FOUND		"Ошибка нахождения элемента"
#define ERROR_TABLE_SIZE			"Ошибка размера таблицы"
#define ERROR_KEY_SIZE				"Ошибка размера ключа"
#define ERROR_PAYLOAD_SIZE			"Ошибка размера данных"
#define ERROR_UNIQUE_ELEMENT		"Ошибка уникальности добавляемого элемента"
#define ERROR_FLUSH_VIEW			"Ошибка записи содержимого отображенного в память файла обратно в файл"
#define ERROR_CREATE_SNAPSHOT		"Ошибка создания файла Snapshot"

namespace HT    // HT API
{
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

	struct HTHANDLE    // блок управления HT
	{
		HTHANDLE();
		HTHANDLE(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const wchar_t FileName[512]);
		int     Capacity;               // емкость хранилища в количестве элементов 
		int		CurrentCapacity;
		int     SecSnapshotInterval;    // переодичность сохранения в сек. 
		int     MaxKeyLength;           // максимальная длина ключа
		int     MaxPayloadLength;       // максимальная длина данных
		wchar_t    FileName[512];       // имя файла 
		HANDLE  File;                   // File HANDLE != 0, если файл открыт
		HANDLE  FileMapping;            // Mapping File HANDLE != 0, если mapping создан  
		LPVOID  Addr;                   // Addr != NULL, если mapview выполнен  
		char    LastErrorMessage[512];  // сообщение об последней ошибке или 0x00  
		time_t  lastsnaptime;           // дата последнего snap'a (time())  
		HANDLE	Mutex;
		HANDLE	SnapThread;
	};

	struct Element   // элемент 
	{
		Element();
		Element(const void* key, int keylength);                                             // for Get
		Element(const void* key, int keylength, const void* payload, int  payloadlength);    // for Insert
		Element(Element* oldelement, const void* newpayload, int  newpayloadlength);         // for update
		void*	key;                 // значение ключа 
		int		keylength;           // размер ключа
		void*	payload;             // данные 
		int     payloadlength;       // размер данных
	};

	HTHANDLE* Create   //  создать HT             
	(
		int	  Capacity,					   // емкость хранилища
		int   SecSnapshotInterval,		   // переодичность сохранения в сек.
		int   MaxKeyLength,                // максимальный размер ключа
		int   MaxPayloadLength,            // максимальный размер данных
		const wchar_t  FileName[512]          // имя файла 
	); 	// != NULL успешное завершение  

	HTHANDLE* Open     //  открыть HT             
	(
		const wchar_t    FileName[512]         // имя файла 
	); 	// != NULL успешное завершение  

	BOOL Snap         // выполнить Snapshot
	(
		HTHANDLE* hthandle           // управление HT (File, FileMapping)
	);

	BOOL Close        // Snap и закрыть HT  и  очистить HTHANDLE
	(
		const HTHANDLE* hthandle           // управление HT (File, FileMapping)
	);	//  == TRUE успешное завершение   

	BOOL Insert      // добавить элемент в хранилище
	(
		HTHANDLE* hthandle,            // управление HT
		Element* element              // элемент
	);	//  == TRUE успешное завершение 

	BOOL Delete      // удалить элемент в хранилище
	(
		HTHANDLE* hthandle,            // управление HT (ключ)
		Element* element              // элемент 
	);	//  == TRUE успешное завершение 

	Element* Get     //  читать элемент в хранилище
	(
		const HTHANDLE* hthandle,            // управление HT
		const Element* element              // элемент 
	); 	//  != NULL успешное завершение 


	BOOL Update     //  именить элемент в хранилище
	(
		const HTHANDLE* hthandle,            // управление HT
		const Element* oldelement,          // старый элемент (ключ, размер ключа)
		const void* newpayload,          // новые данные  
		int             newpayloadlength     // размер новых данных
	); 	//  != NULL успешное завершение 

	char* GetLastError  // получить сообщение о последней ошибке
	(
		HTHANDLE* ht                         // управление HT
	);

	void print                               // распечатать элемент 
	(
		const Element* element              // элемент 
	);

	DWORD WINAPI SnapWithInterval			// запуск снэпшотов
	(
		LPVOID hashTableHandle
	);

	BOOL SaveParametersToFile				// сохранить параметры создания ht в файл
	(
		int Capacity, 
		int SecSnapshotInterval, 
		int MaxKeyLength, 
		int MaxPayloadLength, 
		const wchar_t FileName[512]
	);

	int HashFunction
	(
		const Element* element,
		int HTCapacity
	);
};
