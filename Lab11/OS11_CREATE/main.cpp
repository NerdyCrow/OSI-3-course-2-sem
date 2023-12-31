

#include "../OS11_HTAPI/pch.h"
#include "../OS11_HTAPI/HT.h"

using namespace std;
//create(1000, 3, 10, 256, L"./files/HTspace.ht");
wchar_t* getWC(const char* c);

int main(int argc, char* argv[])
{
	ht::HtHandle* ht = nullptr;
	try {
0
#ifdef _WIN64
		HMODULE hmdll = LoadLibrary(L"OS11_HTAPI.dll");
#else
		HMODULE hmdll = LoadLibrary(L"../Debug/OS11_HTAPI.dll");
#endif
		if (!hmdll)
			throw "-- LoadLibrary failed";
		cout << "-- LoadLibrary success" << endl;

		auto create = (ht::HtHandle * (*)(int, int, int, int, const wchar_t[]))GetProcAddress(hmdll, "create");
		auto close = (BOOL(*)(ht::HtHandle*))GetProcAddress(hmdll, "close");
		
		if (atoi(argv[1])<0|| atoi(argv[2])<0|| atoi(argv[3])<0|| atoi(argv[4])<0)
		{
			throw "Invalid arguments";
		}
		else if (argc != 6) {
			throw "incorrect number of parameters";
		}
		
		ht = create(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), getWC(argv[5]));
		if (ht)
		{
			cout << "HT-Storage Created" << endl;
			wcout << "filename: " << ht->fileName << endl;
			cout << "secSnapshotInterval: " << ht->secSnapshotInterval << endl;
			cout << "capacity: " << ht->capacity << endl;
			cout << "maxKeyLength: " << ht->maxKeyLength << endl;
			cout << "maxPayloadLength: " << ht->maxPayloadLength << endl;

			close(ht);
		}
		else
			cout << "-- create: error" << endl;
	}
	catch (const char* msg) {
		cout << msg << endl;
	}
}

wchar_t* getWC(const char* c)
{
	wchar_t* wc = new wchar_t[strlen(c) + 1];
	mbstowcs(wc, c, strlen(c) + 1);

	return wc;
}
