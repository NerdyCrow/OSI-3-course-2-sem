#include <windows.h>
#include <string>
#include <iostream>
#include "sddl.h"
#pragma warning(disable : 4996)

wchar_t* getWC(const char* c);

int main(int argc, char* argv[])
{
	
	if (argc == 2) {
		wchar_t* fileName = getWC(argv[1]);
		char result[20];
		int length = std::strlen(argv[1]);

		strncpy(result, argv[1], length - 3);
		result[length - 3] = '\0';
		wchar_t* eventName = getWC(strrchr(result, '/'));
		HANDLE hStopEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE,
			eventName);
		
		
			SetEvent(hStopEvent);
		
	}
	


}

wchar_t* getWC(const char* c)
{
	wchar_t* wc = new wchar_t[strlen(c) + 1];
	mbstowcs(wc, c, strlen(c) + 1);

	return wc;
}