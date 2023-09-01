#pragma once
#include <Windows.h>
#include "../OS13_COM/INTERFACE.h"
#include <string>
#define OS13HANDLE void*

namespace HTAPI    // HT API
{
	OS13HANDLE Init();									 // инициализация OS13
	void Dispose(OS13HANDLE h);                       // завершение работы с OS13                  


	HT::HTHANDLE* Create   //  создать HT             
	(
		OS13HANDLE h,							// указатель на компонент
		int	  Capacity,							// емкость хранилища
		int   SecSnapshotInterval,				// переодичность сохранения в сек.
		int   MaxKeyLength,						// максимальный размер ключа
		int   MaxPayloadLength,					// максимальный размер данных
		const wchar_t   FileName[512],          // имя файла 
		const wchar_t   HTUsersGroup[512]       // имя файла 
	);

	HT::HTHANDLE* Create   //  создать HT             
	(
		OS13HANDLE h,							// указатель на компонент
		int	  Capacity,							// емкость хранилища
		int   SecSnapshotInterval,				// переодичность сохранения в сек.
		int   MaxKeyLength,						// максимальный размер ключа
		int   MaxPayloadLength,					// максимальный размер данных
		const wchar_t   FileName[512],          // имя файла 
		const wchar_t   HTUsersGroup[512],       // имя файла 
		const wchar_t userName[512],
		const wchar_t password[512]
	);

	HT::HTHANDLE* Start     //  открыть HT             
	(
		OS13HANDLE h,							// указатель на компонент
		const wchar_t     FileName[512]         // имя файла 
	); 

	HT::HTHANDLE* Open     //  открыть HT             
	(
		OS13HANDLE h,							// указатель на компонент
		const wchar_t     FileName[512]         // имя файла 
	); 	// != NULL успешное завершение  

	HT::HTHANDLE* Open     //  открыть HT             
	(
		OS13HANDLE h,
		const wchar_t     FileName[512],			// имя файла 
		const wchar_t     userName[512],			// имя файла 
		const wchar_t     password[512]				// имя файла 
	); 	// != NULL успешное завершение  

	HT::HTHANDLE* Start     //  открыть HT             
	(
		OS13HANDLE h,
		const wchar_t     FileName[512],			// имя файла 
		const wchar_t     userName[512],			// имя файла 
		const wchar_t     password[512]				// имя файла 
	); 	// != NULL успешное завершение  

	BOOL Close        // Snap и закрыть HT  и  очистить HT::HTHANDLE
	(
		OS13HANDLE h,					   // указатель на компонент
		HT::HTHANDLE* hthandle           // управление HT (File, FileMapping)
	);	//  == TRUE успешное завершение   


	BOOL Insert      // добавить элемент в хранилище
	(
		OS13HANDLE h,					   // указатель на компонент
		HT::HTHANDLE* hthandle,            // управление HT
		HT::Element* element              // элемент
	);	//  == TRUE успешное завершение 


	BOOL Delete      // удалить элемент в хранилище
	(
		OS13HANDLE h,					   // указатель на компонент
		HT::HTHANDLE* hthandle,            // управление HT (ключ)
		HT::Element* element              // элемент 
	);	//  == TRUE успешное завершение 

	HT::Element* Get     //  читать элемент в хранилище
	(
		OS13HANDLE h,					   // указатель на компонент
		HT::HTHANDLE* hthandle,            // управление HT
		HT::Element* element              // элемент 
	); 	//  != NULL успешное завершение 


	BOOL Update     //  именить элемент в хранилище
	(
		OS13HANDLE h,					   // указатель на компонент
		HT::HTHANDLE* hthandle,            // управление HT
		HT::Element* oldelement,          // старый элемент (ключ, размер ключа)
		const char* newpayload,          // новые данные  
		int newpayloadlength     // размер новых данных
	); 	//  != NULL успешное завершение 

	std::string GetLastErr  // получить сообщение о последней ошибке
	(
		OS13HANDLE h,					   // указатель на компонент
		HT::HTHANDLE* ht                         // управление HT
	);

	HT::Element* CreateElement     //  читать элемент в хранилище
	(
		OS13HANDLE h,					   // указатель на компонент
		void* key,
		void* payload,      
		HT::Element** el      // элемент 
	); 	//  != NULL успешное завершение 

	BOOL Print  // получить сообщение о последней ошибке
	(
		OS13HANDLE h,					   // указатель на компонент
		HT::Element* oldelement                       // управление HT
	);
}