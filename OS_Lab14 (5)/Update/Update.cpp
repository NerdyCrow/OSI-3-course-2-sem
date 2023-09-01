#include <iostream>
#include "../OS13_HTCOM/OS13_HTCOM.h"
#include "../OS13_COM/INTERFACE.h"
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
				throw("\nProgram need 1 parameter! argc: %d.\nDefault parameters are used", argc);
			}
		}
		OS13HANDLE h = HTAPI::Init();
		if (h != nullptr) {

			if (userName == nullptr) {
				HT = HTAPI::Open(h, fileName);
			}
			else {
				HT = HTAPI::Open(h, fileName, userName, password);
			}
			if (HT != NULL) {
				char let[32];
				char* newkey;
				HT::Element* elem;
				HT::Element* elGet;

				std::random_device rd;
				std::mt19937 mt(rd());
				std::uniform_int_distribution<int> key(1, 100);

				while (HT->shared->stoped == false) {
					newkey = itoa(key(mt), let, 10);
					HT::Element* newel = HTAPI::CreateElement(h, newkey, newkey, &elem);

					elGet = HTAPI::Get(h, HT, newel);

					if (elGet == nullptr) {
						std::cout << std::endl;
						std::cerr << HTAPI::GetLastErr(h, HT);
						Sleep(1000);
						continue;
					}

					std::cout << std::endl << "OLD element:";
					HTAPI::Print(h, elGet);

					const char* newpay = itoa(atoi((const char*)elGet->payload) + 1, let, 10);
					if (!HTAPI::Update(h, HT, elGet, newpay, strlen(newpay) + 1)) {
						std::cerr << HTAPI::GetLastErr(h, HT);
					}
					else {
						std::cout << "Update element: ";
						HTAPI::Print(h, elGet);
					}

					Sleep(3000);
				}
			}
			else {
				throw "I am upset, but file not created";
			}
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