#include <iostream>
#include "../OS13_HTCOM/OS13_HTCOM.h"
#include <string>
#include <random>
#define IERR(s)    std::cout<<"error "<<s<<std::endl
#define IRES(s,r)  std::cout<<s<<r<<std::endl


int wmain(int argc, wchar_t* argv[])
{
	try {
		wchar_t* fileName = nullptr;
		wchar_t* userName = nullptr;
		wchar_t* password = nullptr;
		HT::HTHANDLE* HT;
		if (argv[1] && argv[2] && argv[3]) {
			fileName = argv[1];
			userName = argv[2];
			password = argv[3];
		}
		else {
			if (argv[1]) {
				fileName = argv[1];
			}
			else {
				throw ("\nProgram need 1 parameter! argc: %d.\nDefault parameters are used", argc);
			}
		}

		OS13HANDLE h = HTAPI::Init();

		if (h != nullptr) {

			if (userName == nullptr) {
				HT = HTAPI::Start(h, fileName);
			}
			else {
				HT = HTAPI::Start(h, fileName, userName, password);
			}
		

			if (HT != NULL) {
				std::cout << "HT-Storage Created " << std::endl;
				HT->shared->stoped = false;
				while (HT->shared->stoped == false) {
					system("cls");
					std::cout << "snapshotIntervalSec: " << HT->shared->secSnapshotInterval << std::endl;
					std::cout << "capacity: " << HT->shared->capacity << std::endl;
					std::cout << "current capacity: " << HT->shared->currentCapacity << std::endl;
					std::cout << "maxKeyLength: " << HT->shared->maxKeyLength << std::endl;
					std::cout << "maxPayloadLength: " << HT->shared->maxPayLoadLength << std::endl;
					Sleep(HT->shared->secSnapshotInterval * 1000);
				}
				bool s = HTAPI::Close(h, HT);
			}
			else {
				throw "I am upset, but file not created";
			}

			HTAPI::Dispose(h);
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