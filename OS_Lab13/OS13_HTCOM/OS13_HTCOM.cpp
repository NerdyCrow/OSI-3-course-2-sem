// OS13_HTCOM.cpp : Определяет функции для статической библиотеки.
//

#include "pch.h"
#include "framework.h"
#include "OS13_HTCOM.h"
#include "../OS13_COM/INTERFACE.h"

namespace HTAPI    // HT API
{
	OS13HANDLE HTAPI::Init() {
		IUnknown* pIUnknown = NULL;
		CoInitialize(NULL);                        // инициализация библиотеки OLE32
		HRESULT hr0 = CoCreateInstance(CLSID_CA, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pIUnknown);
		if (SUCCEEDED(hr0))
		{
			return pIUnknown;
		}
		else {
			throw (int)hr0;
			return nullptr;
		}
	}

	void HTAPI::Dispose(OS13HANDLE h) {
		((IUnknown*)h)->Release();
		CoFreeUnusedLibraries();
	}

	HT::HTHANDLE* HTAPI::Create   //  создать HT             
	(
		OS13HANDLE h,					   // указатель на компонент
		int	  Capacity,					   // емкость хранилища
		int   SecSnapshotInterval,		   // переодичность сохранения в сек.
		int   MaxKeyLength,                // максимальный размер ключа
		int   MaxPayloadLength,            // максимальный размер данных
		const wchar_t  FileName[512]      // имя файла 
	) {
		HT::HTHANDLE* pht;
		if (SUCCEEDED(((IOS13*)h)->Create(
			Capacity, SecSnapshotInterval, MaxKeyLength, MaxPayloadLength, FileName, &pht))) {
			
			return pht;
		}
		else {
			return NULL;
		}
	}

	HT::HTHANDLE* HTAPI::Start    //  открыть HT             
	(
		OS13HANDLE h,					   // указатель на компонент
		const wchar_t   FileName[512]         // имя файла 
	) {
		HT::HTHANDLE* pht;
		if (SUCCEEDED(((IOS13*)h)->Start(FileName, &pht))) {
			return pht;
		}
		else {
			return NULL;
		}
	}

	HT::HTHANDLE* HTAPI::Open     //  открыть HT             
	(
		OS13HANDLE h,					   // указатель на компонент
		const wchar_t   FileName[512]         // имя файла 
	) {
		HT::HTHANDLE* pht;
		if (SUCCEEDED(((IOS13*)h)->Open(FileName, &pht))) {
			return pht;
		}
		else {
			return NULL;
		}
	}

	BOOL Close        // Snap и закрыть HT  и  очистить HT::HTHANDLE
	(
		OS13HANDLE h,					   // указатель на компонент
		HT::HTHANDLE* hthandle           // управление HT (File, FileMapping)
	) {
		if (SUCCEEDED(((IOS13*)h)->Close(&hthandle))) {
			return true;
		}
		else {
			return false;
		}
	}


	BOOL Insert      // добавить элемент в хранилище
	(
		OS13HANDLE h,					   // указатель на компонент
		HT::HTHANDLE* hthandle,            // управление HT
		HT::Element* element              // элемент
	) {
		if (SUCCEEDED(((IOS13*)h)->Insert(&hthandle, &element))) {
			return true;
		}
		else {
			return false;
		}
	}

	BOOL Delete      // удалить элемент в хранилище
	(
		OS13HANDLE h,					   // указатель на компонент
		HT::HTHANDLE* hthandle,            // управление HT (ключ)
		HT::Element* element              // элемент 
	) {
		if (SUCCEEDED(((IOS13*)h)->Delete(&hthandle, &element))) {
			return true;
		}
		else {
			return false;
		}
	}

	HT::Element* Get     //  читать элемент в хранилище
	(
		OS13HANDLE h,					   // указатель на компонент
		HT::HTHANDLE* hthandle,            // управление HT
		HT::Element* element              // элемент 
	) {
		HT::Element* result;
		if (SUCCEEDED(((IOS13*)h)->Get(&result, &hthandle, &element))) {
			return result;
		}
		else {
			return NULL;
		}
	}

	BOOL Update     //  именить элемент в хранилище
	(
		OS13HANDLE h,					   // указатель на компонент
		HT::HTHANDLE* hthandle,            // управление HT
		HT::Element* oldelement,          // старый элемент (ключ, размер ключа)
		const char* newpayload,          // новые данные  
		int             newpayloadlength     // размер новых данных
	) {
		if (SUCCEEDED(((IOS13*)h)->Update(&hthandle, &oldelement, &newpayload, &newpayloadlength))) {
			return true;
		}
		else {
			return false;
		}
	}

	std::string GetLastErr  // получить сообщение о последней ошибке
	(
		OS13HANDLE h,					   // указатель на компонент
		HT::HTHANDLE* ht                         // управление HT
	) {
		std::string err;
		if (SUCCEEDED(((IOS13*)h)->GetLastErr(err, &ht))) {
			return err;
		}
		else {
			return NULL;
		}
	}

	HT::Element* CreateElement     //  читать элемент в хранилище
	(
		OS13HANDLE h,					   // указатель на компонент
		void* key,
		void* payload,
		HT::Element** el      // элемент 
	) {
		HT::Element* result;
		if (SUCCEEDED(((IOS13*)h)->CreateElement(key,payload, &result))) {
			return result;
		}
		else {
			return NULL;
		}
	}

	BOOL Print     //  читать элемент в хранилище
	(
		OS13HANDLE h,					   // указатель на компонент
		HT::Element* element
	) {
		if (SUCCEEDED(((IOS13*)h)->Print(&element))) {
			return true;	
		}
		else {
			return false;
		}
	}
};