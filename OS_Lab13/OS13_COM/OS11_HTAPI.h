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
	//handle - ��� ��������� �� ���������, ����������� �������� ������ - �� ����������� �������, 
	//��������, ������-�� ����, ��������, �����...������������ ��� ��������� ������� ����� ������� ��� ��������� �������� � ��� ���������.

	// API HT - ����������� ��������� ��� ������� � ��-��������� 
	//          ��-��������� ������������� ��� �������� ������ � �� � ������� ����/��������
	//          ���������������� (�����������) ������ �������������� � ������� snapshot-���������� 
	//          Create - �������  � ������� HT-��������� ��� �������������   
	//          Open   - ������� HT-��������� ��� �������������
	//          Insert - ������� ������� ������
	//          Delete - ������� ������� ������    
	//          Get    - ������  ������� ������
	//          Update - �������� ������� ������
	//          Snap   - �������� snapshot
	//          Close  - ��������� Snap � ������� HT-��������� ��� �������������
	//          GetLastError - �������� ��������� � ��������� ������

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

	struct HTHANDLE    // ���� ���������� HT
	{
		OS_Lab11_HTAPI HTHANDLE();
		OS_Lab11_HTAPI HTHANDLE(
			int Capacity,
			int SecSnapshotInterval,
			int MaxKeyLength,
			int MaxPayloadLength,
			const wchar_t FileName[512]
		);
		wchar_t    FileName[512];          // ��� ����� 
		HANDLE  File;                   // File HANDLE != 0, ���� ���� ������
		HANDLE  FileMapping;            // Mapping File HANDLE != 0, ���� mapping ������  
		LPVOID  Addr;                   // Addr != NULL, ���� mapview ��������  
		HT::BetweenMemory* shared;
		volatile  bool exit = false;
		char    LastErrorMessage[512];  // ��������� �� ��������� ������ ��� 0x00  
		time_t  lastsnaptime;           // ���� ���������� snap'a (time())  

		HANDLE Mutex;
		int N;							//������� ����������
		Thread SnapshotThread;
		[[nodiscard]] static int Alignment(int sizeElement, int maxKeyLength, int maxPayLoadLength);
		[[nodiscard]] LPVOID Offset(unsigned int index) const;
		void PrintSizeHT() const;
	};

	struct Element   // ������� 
	{
		int         keylength{};           // ������ �����
		int         payloadlength{};       // ������ ������
		const void* key{};                 // �������� ����� 
		const void* payload{};             // ������ 
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


	HTHANDLE * Create   //  ������� HT             
	(
		int	  Capacity,					   // ������� ���������
		int   SecSnapshotInterval,		   // ������������� ���������� � ���.
		int   MaxKeyLength,                // ������������ ������ �����
		int   MaxPayloadLength,            // ������������ ������ ������
		const wchar_t fileName[512]          // ��� ����� 
	); 	// != NULL �������� ����������  

	HTHANDLE * Start     //  ������� HT             
	(
		const wchar_t FileName[512]         // ��� ����� 
	); 	// != NULL �������� ����������  
	HTHANDLE * Open     //  ������� HT             
	(
		const wchar_t FileName[512]         // ��� ����� 
	); 	// != NULL �������� ����������  

	BOOL Snap         // ��������� Snapshot
	(
		HTHANDLE * hthandle           // ���������� HT (File, FileMapping)
	);


	BOOL Close        // Snap � ������� HT  �  �������� HTHANDLE
	(
		HTHANDLE * hthandle           // ���������� HT (File, FileMapping)
	);	//  == TRUE �������� ����������   


	BOOL Insert      // �������� ������� � ���������
	(
		HTHANDLE * hthandle,            // ���������� HT
		Element * element              // �������
	);	//  == TRUE �������� ���������� 


	BOOL Delete      // ������� ������� � ���������
	(
		HTHANDLE * hthandle,            // ���������� HT (����)
		Element * element              // ������� 
	);	//  == TRUE �������� ���������� 

	Element * Get     //  ������ ������� � ���������
	(
		HTHANDLE * hthandle,            // ���������� HT
		Element * element              // ������� 
	); 	//  != NULL �������� ���������� 


	BOOL Update     //  ������� ������� � ���������
	(
		HTHANDLE * hthandle,            // ���������� HT
		Element * oldelement,          // ������ ������� (����, ������ �����)
		const char* newpayload,          // ����� ������  
		int       newpayloadlength     // ������ ����� ������
	); 	//  != NULL �������� ���������� 

	char* GetLastErr  // �������� ��������� � ��������� ������
	(
		HTHANDLE * ht                         // ���������� HT
	);

	void Print                               // ����������� ������� 
	(
		Element * element              // ������� 
	);
	DWORD WINAPI SnapThread(HTHANDLE*);
	unsigned int Hash(HTHANDLE*, const char*);
	void SetErrorMessage(HTHANDLE * ht, const char* message);
	BOOL CheckLengthElem(HTHANDLE * ht, Element * el);
};
