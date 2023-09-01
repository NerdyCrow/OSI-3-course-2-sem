#include <iostream>
#include "../OS13_HTCOM/OS13_HTCOM.h"
#include "../OS13_COM/INTERFACE.h"
#include <string>
#include <string>
#include <sstream>
#include <ctime>
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
	try
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

			HT = HTAPI::Open(h, fileName);

			if (HT == NULL) {
				throw "HT not found";
			}

			HT->shared->stoped = true;
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