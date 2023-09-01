#include <objbase.h>
#include <iostream>
#include "../OS13_HTCOM/OS13_HTCOM.h"
#include <string>
#include <iostream>
#include <Windows.h>
#include <lmcons.h>
#pragma warning(disable : 4996)
#define IERR(s)    std::cout<<"error "<<s<<std::endl
#define IRES(s,r)  std::cout<<s<<r<<std::endl


int wmain(int argc, wchar_t* argv[])
{
	try {
		int capacity = 2000, maxKeyLength = 4, maxPayloadLength = 4, snapshotIntervalSec = 2;
		wchar_t* fileName = nullptr;
		wchar_t* HTUsersGroup = nullptr;
		wchar_t* password = nullptr;
		HT::HTHANDLE* HT;



		capacity = _wtoi(argv[2]);
		snapshotIntervalSec = _wtoi(argv[3]);
		maxKeyLength = _wtoi(argv[4]);
		maxPayloadLength = _wtoi(argv[5]);
		fileName = argv[1];
		HTUsersGroup = argv[6];



		
		OS13HANDLE h = HTAPI::Init();
		if (h != nullptr) {

			HT = HTAPI::Create(h, capacity, snapshotIntervalSec, maxKeyLength, maxPayloadLength, fileName, HTUsersGroup);

			if (HT != NULL) {
				std::cout << "HT-Storage Created " << std::endl;
				std::wcout << "filename: " << HT->FileName << std::endl;
				std::cout << "snapshotIntervalSec: " << HT->shared->secSnapshotInterval << std::endl;
				std::cout << "capacity: " << HT->shared->capacity << std::endl;
				std::cout << "current capacity: " << HT->shared->currentCapacity << std::endl;
				std::cout << "maxKeyLength: " << HT->shared->maxKeyLength << std::endl;
				std::cout << "maxPayloadLength: " << HT->shared->maxPayLoadLength << std::endl;

				//system("pause");

				HT->shared->stoped = true;
				bool s = HTAPI::Close(h, HT);
			}
			else {
				throw "I am upset, but file not created";
			}
			HTAPI::Dispose(h);
			return 0;
		}
		else {
			throw "error with open api\n";
		}
		return 0;
	}

	catch (const char* msg)
	{
		std::cout << "\nERROR: " << msg << std::endl;
	}
}