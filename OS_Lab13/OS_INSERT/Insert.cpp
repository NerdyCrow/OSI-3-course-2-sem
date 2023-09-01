#include <iostream>
#include "../OS13_HTCOM/OS13_HTCOM.h"
#include "../OS13_COM/INTERFACE.h"
#include <string>
#include <string>
#include <sstream>
#include <ctime>
#include <random>
#pragma warning(disable : 4996)
#define IERR(s)    std::cout<<"error "<<s<<std::endl
#define IRES(s,r)  std::cout<<s<<r<<std::endl

IOS13* pIOS13 = nullptr;

void printStr(char* str) {
	std::cout << "ERROR:\t";
	int i = 0;
	while (str[i]) {
		std::cout << str[i];
		i++;
	}
	std::cout << std::endl;
}

int wmain(int argc, wchar_t* argv[])
{
	wchar_t* fileName = nullptr;
	HT::HTHANDLE* HT;
	if (argc == 2) {
		fileName = argv[1];
	}
	else {
		printf("\nProgram need 1 parameter! argc: %d.\nDefault parameters are used", argc);
	}
	OS13HANDLE h = HTAPI::Init();
	if (h != nullptr) {

		HT = HTAPI::Open(h,fileName);

		if (HT == NULL) {
			throw "I am upset, but file not created";
		}

		char let[32];
		char* newkey;
		HT::Element* elem;

		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_int_distribution<int> key(1, 100);

		while (HT->shared->stoped == false) {
			newkey = itoa(key(mt), let, 10);
			HT::Element* el = HTAPI::CreateElement(h, newkey, newkey,&elem);

			if (!HTAPI::Insert(h, HT, el)) {
				std::cout << std::endl;
				std::cerr << HTAPI::GetLastErr(h, HT);
			}
			Sleep(3000);
		}
	}
	else {
		printf("error with open api\n");
	}
	return 0;
}