#include <objbase.h>
#include <iostream>
#include "../OS13_HTCOM/OS13_HTCOM.h"
#include <string>
#pragma warning(disable : 4996)
#define IERR(s)    std::cout<<"error "<<s<<std::endl
#define IRES(s,r)  std::cout<<s<<r<<std::endl


int wmain(int argc, wchar_t* argv[])
{
	int capacity = 2000, maxKeyLength = 4, maxPayloadLength = 4, snapshotIntervalSec = 2;
	wchar_t* fileName = nullptr;
	HT::HTHANDLE* HT;
	std::wstring HTUsersGroup(L"HT");
	if (argc == 6) {
		capacity = _wtoi(argv[2]);
		snapshotIntervalSec = _wtoi(argv[3]);
		maxKeyLength = _wtoi(argv[4]);
		maxPayloadLength = _wtoi(argv[5]);
		fileName = argv[1];
	}
	else {
		printf("\nProgram need 5 parameters! argc: %d.\nDefault parameters are used", argc);
	}

	OS13HANDLE h = HTAPI::Init();
	if (h != nullptr) {

		HT = HTAPI::Create(h, capacity, snapshotIntervalSec, maxKeyLength, maxPayloadLength, fileName);

		if (HT != NULL) {
			std::cout << "HT-Storage Created " << std::endl;
			std::wcout << "filename: " << HT->FileName << std::endl;
			std::cout << "snapshotIntervalSec: " << HT->shared->secSnapshotInterval << std::endl;
			std::cout << "capacity: " << HT->shared->capacity << std::endl;
			std::cout << "current capacity: " << HT->shared->currentCapacity << std::endl;
			std::cout << "maxKeyLength: " << HT->shared->maxKeyLength << std::endl;
			std::cout << "maxPayloadLength: " << HT->shared->maxPayLoadLength << std::endl;

			system("pause");
			bool s = HTAPI::Close(h, HT);

		}
		else {
			printf("\n I am upset, but file not created");
		}
		HTAPI::Dispose(h);
	}
	else {
		printf("error with open api\n");
	}
	return 0;
}