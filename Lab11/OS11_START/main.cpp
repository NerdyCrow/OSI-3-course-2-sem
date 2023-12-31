
#include <conio.h>
#include "../OS11_HTAPI/pch.h"
#include "../OS11_HTAPI/HT.h"

using namespace std;

wchar_t* getWC(const char* c);

int main(int argc, char* argv[])
{
	try {

#ifdef _WIN64
		HMODULE hmdll = LoadLibrary(L"OS11_HTAPI.dll");
#else
		HMODULE hmdll = LoadLibrary(L"../Debug/OS11_HTAPI.dll");
#endif
		if (!hmdll)
			throw "-- LoadLibrary failed";
		cout << "-- LoadLibrary success" << endl;
		if (argc != 2) {
			throw "incorrect number of parameters";
		}

		auto open = (ht::HtHandle * (*)(const wchar_t[], bool))GetProcAddress(hmdll, "open");
		auto close = (BOOL(*)(ht::HtHandle*))GetProcAddress(hmdll, "close");
		FARPROC add_numbers_func = GetProcAddress(hmdll, "open");

		ht::HtHandle* ht = nullptr;

		wchar_t* fileName = getWC(argv[1]);
		ht = open(fileName, false);
		if (ht)
		{
			cout << "HT-Storage Start" << endl;
			wcout << "filename: " << ht->fileName << endl;
			cout << "secSnapshotInterval: " << ht->secSnapshotInterval << endl;
			cout << "capacity: " << ht->capacity << endl;
			cout << "maxKeyLength: " << ht->maxKeyLength << endl;
			cout << "maxPayloadLength: " << ht->maxPayloadLength << endl;

			while (!kbhit())
				SleepEx(0, TRUE);

			close(ht);
		}
		else
			cout << "-- open: error" << endl;
	}
	catch (const char* msg)
	{
		
		cout << msg << endl;
	}
}

wchar_t* getWC(const char* c)
{
	wchar_t* wc = new wchar_t[strlen(c) + 1];
	mbstowcs(wc, c, strlen(c) + 1);

	return wc;
}
