#pragma once
#include <Windows.h>

#define RECORD_SIZE					(sizeof(Element) + hashTableHandle->MaxKeyLength + hashTableHandle->MaxPayloadLength)
#define ERROR_CREATE_FILE			"������ �������� �����"
#define ERROR_CREATE_CONFIG_FILE	"������ �������� ����� ������������"
#define ERROR_WRITE_CONFIG_FILE		"������ ������ � ���� ������������"
#define ERROR_OPEN_FILE				"������ �������� �����"
#define ERROR_CREATE_FILE_MAPPING	"������ �������� ������� ��������� �����������"
#define ERROR_MAP_VIEW				"������ ����������� ����� � ������"
#define ERROR_CREATE_MUTEX			"������ �������� ��������"
#define ERROR_CREATE_SNAP_THREAD	"������ �������� SnapThread"
#define ERROR_DATA_NOT_FOUND		"������ ���������� ��������"
#define ERROR_TABLE_SIZE			"������ ������� �������"
#define ERROR_KEY_SIZE				"������ ������� �����"
#define ERROR_PAYLOAD_SIZE			"������ ������� ������"
#define ERROR_UNIQUE_ELEMENT		"������ ������������ ������������ ��������"
#define ERROR_FLUSH_VIEW			"������ ������ ����������� ������������� � ������ ����� ������� � ����"
#define ERROR_CREATE_SNAPSHOT		"������ �������� ����� Snapshot"

namespace HT    // HT API
{
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

	struct HTHANDLE    // ���� ���������� HT
	{
		HTHANDLE();
		HTHANDLE(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const wchar_t FileName[512]);
		int     Capacity;               // ������� ��������� � ���������� ��������� 
		int		CurrentCapacity;
		int     SecSnapshotInterval;    // ������������� ���������� � ���. 
		int     MaxKeyLength;           // ������������ ����� �����
		int     MaxPayloadLength;       // ������������ ����� ������
		wchar_t    FileName[512];       // ��� ����� 
		HANDLE  File;                   // File HANDLE != 0, ���� ���� ������
		HANDLE  FileMapping;            // Mapping File HANDLE != 0, ���� mapping ������  
		LPVOID  Addr;                   // Addr != NULL, ���� mapview ��������  
		char    LastErrorMessage[512];  // ��������� �� ��������� ������ ��� 0x00  
		time_t  lastsnaptime;           // ���� ���������� snap'a (time())  
		HANDLE	Mutex;
		HANDLE	SnapThread;
	};

	struct Element   // ������� 
	{
		Element();
		Element(const void* key, int keylength);                                             // for Get
		Element(const void* key, int keylength, const void* payload, int  payloadlength);    // for Insert
		Element(Element* oldelement, const void* newpayload, int  newpayloadlength);         // for update
		void*	key;                 // �������� ����� 
		int		keylength;           // ������ �����
		void*	payload;             // ������ 
		int     payloadlength;       // ������ ������
	};

	HTHANDLE* Create   //  ������� HT             
	(
		int	  Capacity,					   // ������� ���������
		int   SecSnapshotInterval,		   // ������������� ���������� � ���.
		int   MaxKeyLength,                // ������������ ������ �����
		int   MaxPayloadLength,            // ������������ ������ ������
		const wchar_t  FileName[512]          // ��� ����� 
	); 	// != NULL �������� ����������  

	HTHANDLE* Open     //  ������� HT             
	(
		const wchar_t    FileName[512]         // ��� ����� 
	); 	// != NULL �������� ����������  

	BOOL Snap         // ��������� Snapshot
	(
		HTHANDLE* hthandle           // ���������� HT (File, FileMapping)
	);

	BOOL Close        // Snap � ������� HT  �  �������� HTHANDLE
	(
		const HTHANDLE* hthandle           // ���������� HT (File, FileMapping)
	);	//  == TRUE �������� ����������   

	BOOL Insert      // �������� ������� � ���������
	(
		HTHANDLE* hthandle,            // ���������� HT
		Element* element              // �������
	);	//  == TRUE �������� ���������� 

	BOOL Delete      // ������� ������� � ���������
	(
		HTHANDLE* hthandle,            // ���������� HT (����)
		Element* element              // ������� 
	);	//  == TRUE �������� ���������� 

	Element* Get     //  ������ ������� � ���������
	(
		const HTHANDLE* hthandle,            // ���������� HT
		const Element* element              // ������� 
	); 	//  != NULL �������� ���������� 


	BOOL Update     //  ������� ������� � ���������
	(
		const HTHANDLE* hthandle,            // ���������� HT
		const Element* oldelement,          // ������ ������� (����, ������ �����)
		const void* newpayload,          // ����� ������  
		int             newpayloadlength     // ������ ����� ������
	); 	//  != NULL �������� ���������� 

	char* GetLastError  // �������� ��������� � ��������� ������
	(
		HTHANDLE* ht                         // ���������� HT
	);

	void print                               // ����������� ������� 
	(
		const Element* element              // ������� 
	);

	DWORD WINAPI SnapWithInterval			// ������ ���������
	(
		LPVOID hashTableHandle
	);

	BOOL SaveParametersToFile				// ��������� ��������� �������� ht � ����
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
