#pragma once
#include <Windows.h>
#include "../OS13_COM/INTERFACE.h"
#include <string>
#define OS13HANDLE void*

namespace HTAPI    // HT API
{
	OS13HANDLE Init();									 // ������������� OS13
	void Dispose(OS13HANDLE h);                       // ���������� ������ � OS13                  


	HT::HTHANDLE* Create   //  ������� HT             
	(
		OS13HANDLE h,							// ��������� �� ���������
		int	  Capacity,							// ������� ���������
		int   SecSnapshotInterval,				// ������������� ���������� � ���.
		int   MaxKeyLength,						// ������������ ������ �����
		int   MaxPayloadLength,					// ������������ ������ ������
		const wchar_t   FileName[512],          // ��� ����� 
		const wchar_t   HTUsersGroup[512]       // ��� ����� 
	);

	HT::HTHANDLE* Create   //  ������� HT             
	(
		OS13HANDLE h,							// ��������� �� ���������
		int	  Capacity,							// ������� ���������
		int   SecSnapshotInterval,				// ������������� ���������� � ���.
		int   MaxKeyLength,						// ������������ ������ �����
		int   MaxPayloadLength,					// ������������ ������ ������
		const wchar_t   FileName[512],          // ��� ����� 
		const wchar_t   HTUsersGroup[512],       // ��� ����� 
		const wchar_t userName[512],
		const wchar_t password[512]
	);

	HT::HTHANDLE* Start     //  ������� HT             
	(
		OS13HANDLE h,							// ��������� �� ���������
		const wchar_t     FileName[512]         // ��� ����� 
	); 

	HT::HTHANDLE* Open     //  ������� HT             
	(
		OS13HANDLE h,							// ��������� �� ���������
		const wchar_t     FileName[512]         // ��� ����� 
	); 	// != NULL �������� ����������  

	HT::HTHANDLE* Open     //  ������� HT             
	(
		OS13HANDLE h,
		const wchar_t     FileName[512],			// ��� ����� 
		const wchar_t     userName[512],			// ��� ����� 
		const wchar_t     password[512]				// ��� ����� 
	); 	// != NULL �������� ����������  

	HT::HTHANDLE* Start     //  ������� HT             
	(
		OS13HANDLE h,
		const wchar_t     FileName[512],			// ��� ����� 
		const wchar_t     userName[512],			// ��� ����� 
		const wchar_t     password[512]				// ��� ����� 
	); 	// != NULL �������� ����������  

	BOOL Close        // Snap � ������� HT  �  �������� HT::HTHANDLE
	(
		OS13HANDLE h,					   // ��������� �� ���������
		HT::HTHANDLE* hthandle           // ���������� HT (File, FileMapping)
	);	//  == TRUE �������� ����������   


	BOOL Insert      // �������� ������� � ���������
	(
		OS13HANDLE h,					   // ��������� �� ���������
		HT::HTHANDLE* hthandle,            // ���������� HT
		HT::Element* element              // �������
	);	//  == TRUE �������� ���������� 


	BOOL Delete      // ������� ������� � ���������
	(
		OS13HANDLE h,					   // ��������� �� ���������
		HT::HTHANDLE* hthandle,            // ���������� HT (����)
		HT::Element* element              // ������� 
	);	//  == TRUE �������� ���������� 

	HT::Element* Get     //  ������ ������� � ���������
	(
		OS13HANDLE h,					   // ��������� �� ���������
		HT::HTHANDLE* hthandle,            // ���������� HT
		HT::Element* element              // ������� 
	); 	//  != NULL �������� ���������� 


	BOOL Update     //  ������� ������� � ���������
	(
		OS13HANDLE h,					   // ��������� �� ���������
		HT::HTHANDLE* hthandle,            // ���������� HT
		HT::Element* oldelement,          // ������ ������� (����, ������ �����)
		const char* newpayload,          // ����� ������  
		int newpayloadlength     // ������ ����� ������
	); 	//  != NULL �������� ���������� 

	std::string GetLastErr  // �������� ��������� � ��������� ������
	(
		OS13HANDLE h,					   // ��������� �� ���������
		HT::HTHANDLE* ht                         // ���������� HT
	);

	HT::Element* CreateElement     //  ������ ������� � ���������
	(
		OS13HANDLE h,					   // ��������� �� ���������
		void* key,
		void* payload,      
		HT::Element** el      // ������� 
	); 	//  != NULL �������� ���������� 

	BOOL Print  // �������� ��������� � ��������� ������
	(
		OS13HANDLE h,					   // ��������� �� ���������
		HT::Element* oldelement                       // ���������� HT
	);
}