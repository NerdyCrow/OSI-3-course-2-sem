#pragma once
#include <objbase.h>
#include "OS11_HTAPI.h"

#define FNAME L"Kir14(5).OS.COM"
#define VINDX L"Kir14(5).OS.1"
#define PRGID L"Kir14(5).OS"

//// {C3E787D8-E8AC-479D-930F-C86433CE5915}
//static const GUID CLSID_CA =
//{ 0xc3e787d8, 0xe8ac, 0x479d, { 0x93, 0xf, 0xc8, 0x64, 0x33, 0xce, 0x59, 0x15 } };

// {5354A500-DD0B-4365-86EC-3031799B7C10}
static const GUID CLSID_CA_14 =
{ 0x5354a500, 0xdd0b, 0x4365, { 0x86, 0xec, 0x30, 0x31, 0x79, 0x9b, 0x7c, 0x10 } };



//// {88EDCC16-0A1A-4786-92E0-A523D3CBDFCC}
//static const GUID IID_IOS13 =
//{ 0x88edcc16, 0xa1a, 0x4786, { 0x92, 0xe0, 0xa5, 0x23, 0xd3, 0xcb, 0xdf, 0xcc } };

// {33DD0A45-EAE6-4C1E-B099-905C91B9AEC0}
static const GUID IID_IOS14 =
{ 0x33dd0a45, 0xeae6, 0x4c1e, { 0xb0, 0x99, 0x90, 0x5c, 0x91, 0xb9, 0xae, 0xc0 } };





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
		const wchar_t  HTUsersGroup[512],          // ��� ����� 
		HT::HTHANDLE** result
	)) PURE; 	// != NULL �������� ����������  
	
	STDMETHOD(Create   //  ������� HT             
	(
		int	  Capacity,					   // ������� ���������
		int   SecSnapshotInterval,		   // ������������� ���������� � ���.
		int   MaxKeyLength,                // ������������ ������ �����
		int   MaxPayloadLength,            // ������������ ������ ������
		const wchar_t  FileName[512],          // ��� ����� 
		const wchar_t  HTUsersGroup[512],          // ��� ����� 
		const wchar_t userName[512],
		const wchar_t password[512],
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

	STDMETHOD(Open     //  ������� HT             
	(
		const wchar_t    FileName[512],         // ��� ����� 
		const wchar_t    userName[512],         // ��� ����� 
		const wchar_t    password[512],         // ��� ����� 
		HT::HTHANDLE** result
	)) PURE; 	// != NULL �������� ����������  

	STDMETHOD(Start     //  ������� HT             
	(
		const wchar_t    FileName[512],         // ��� ����� 
		const wchar_t    userName[512],         // ��� ����� 
		const wchar_t    password[512],         // ��� ����� 
		HT::HTHANDLE** result
	)) PURE; 	// != NULL �������� ����������  

	//STDMETHOD(OpenExist     //  ������� HT             
	//(
	//	const wchar_t    FileName[512],         // ��� ����� 
	//	HT::HTHANDLE** result
	//)) PURE; 	// != NULL �������� ���������� 

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

