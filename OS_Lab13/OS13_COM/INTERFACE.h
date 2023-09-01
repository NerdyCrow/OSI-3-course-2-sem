#pragma once
#include <objbase.h>
#include "OS11_HTAPI.h"

#define FNAME L"Lera13.OS.COM"
#define VINDX L"Lera13.OS.1"
#define PRGID L"Lera13.OS"

// {C3E787D8-E8AC-479D-930F-C86433CE5915}
static const GUID CLSID_CA =
{ 0xc3e787d8, 0xe8ac, 0x479d, { 0x93, 0xf, 0xc8, 0x64, 0x33, 0xce, 0x59, 0x15 } };

// {88EDCC16-0A1A-4786-92E0-A523D3CBDFCC}
static const GUID IID_IOS13 =
{ 0x88edcc16, 0xa1a, 0x4786, { 0x92, 0xe0, 0xa5, 0x23, 0xd3, 0xcb, 0xdf, 0xcc } };


interface IOS13 :IUnknown {
    STDMETHOD(Add(const double, const double, double*)) PURE;
	STDMETHOD(Sub(const double, const double, double*)) PURE;

	STDMETHOD(Create   //  ������� HT             
	(
		int	  Capacity,					   // ������� ���������
		int   SecSnapshotInterval,		   // ������������� ���������� � ���.
		int   MaxKeyLength,                // ������������ ������ �����
		int   MaxPayloadLength,            // ������������ ������ ������
		const wchar_t  FileName[512],          // ��� ����� 
		HT::HTHANDLE** result
	)) PURE; 	// != NULL �������� ����������  

	STDMETHOD(Start     //  ������� HT             
	(
		const wchar_t FileName[512],         // ��� ����� 
		HT::HTHANDLE** result
	)) PURE; 	// != NULL �������� ���������� 

	STDMETHOD(Open     //  ������� HT             
	(
		const wchar_t    FileName[512],         // ��� ����� 
		HT::HTHANDLE** result
	)) PURE; 	// != NULL �������� ���������� 

	STDMETHOD(Close     //  ������� HT             
	(
		HT::HTHANDLE** hthandle           // ���������� HT (File, FileMapping)
	)) PURE; 	// != NULL �������� ���������� 

	STDMETHOD(Snap     //  ������� HT             
	(
		HT::HTHANDLE** hthandle
	)) PURE; 	// != NULL �������� ���������� 

	STDMETHOD(Insert     //  ������� HT             
	(
		HT::HTHANDLE** hthandle,            // ���������� HT
		HT::Element** element              // �������
	)) PURE; 	// != NULL �������� ����������

	STDMETHOD(Get     //  ������� HT             
	(
		HT::Element** result,
		HT::HTHANDLE** hthandle,            // ���������� HT
		HT::Element** element              // ������� 
	)) PURE; 	// != NULL �������� ����������

	STDMETHOD(Delete     //  ������� HT             
	(
		HT::HTHANDLE** hthandle,            // ���������� HT (����)
		HT::Element** element              // ������� 
	)) PURE; 	// != NULL �������� ����������

	STDMETHOD(Update     //  ������� HT             
	(
		HT::HTHANDLE** hthandle,            // ���������� HT
		HT::Element** oldelement,          // ������ ������� (����, ������ �����)
		const char** newpayload,          // ����� ������  
		int*   newpayloadlength     // ������ ����� ������
	)) PURE; 	// != NULL �������� ����������

	STDMETHOD(GetLastErr     //  ������� HT             
	(
		std::string& error,
		HT::HTHANDLE** ht                         // ���������� HT
	)) PURE; 	// != NULL �������� ����������

	STDMETHOD(CreateElement
	(
		void* key,
		void* payload,
		HT::Element** el			                  // ���������� HT
	)) PURE; 	// != NULL �������� ����������

	STDMETHOD(Print     //  ������� HT             
	(
		HT::Element** element              // ������� 
	)) PURE; 	// != NULL �������� ����������
};

