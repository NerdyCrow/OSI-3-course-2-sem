#include <iostream>
#include "../OS13_HTCOM/OS13_HTCOM.h"
#include <string>
#include <random>
#define IERR(s)    std::cout<<"error "<<s<<std::endl
#define IRES(s,r)  std::cout<<s<<r<<std::endl

int wmain(int argc, wchar_t* argv[])
{
	try
	{
		wchar_t* fileName = nullptr;
		HT::HTHANDLE* HT;
		if (argv[1]) {
			fileName = argv[1];
		}
		else {
			printf("\nProgram need 1 parameter! argc: %d.\nDefault parameters are used", argc);
		}
		OS13HANDLE h = HTAPI::Init();

		if (h != nullptr) {
			HT = HTAPI::Start(h, fileName);

			if (HT != NULL) {
				std::cout << "HT-Storage Created " << std::endl;
				HT->shared->stoped = false;
				while (HT->shared->stoped == false) {
					system("cls");
					std::cout << "snapshotIntervalSec: " << HT->shared->secSnapshotInterval << std::endl;
					std::cout << "capacity: " << HT->shared->capacity << std::endl;
					std::cout << "current capacity: " << HT->shared->currentCapacity << std::endl;
					std::cout << "maxKeyLength: " << HT->shared->maxKeyLength -1 << std::endl;
					std::cout << "maxPayloadLength: " << HT->shared->maxPayLoadLength -1<< std::endl;
					Sleep(HT->shared->secSnapshotInterval * 1000);
				}
				bool s = HTAPI::Close(h, HT);
			}
			else {
				throw "HT not found";
			}

			HTAPI::Dispose(h);
		}
		else {
			printf("error with open api\n");
		}
	}
	catch (const char* err)
	{
		std::cout << err << std::endl;
		exit(-1);
	}
	
	return 0;
}