#include <string>
#include <sstream>
#include "../OS11_HTAPI/pch.h"
#include "../OS11_HTAPI/HT.h"

using namespace std;

string intToString(int number);
wchar_t* getWC(const char* c);
int main(int argc, char* argv[])
{
	try 
	{
#ifdef _WIN64
		HMODULE hmdll = LoadLibrary(L"OS11_HTAPI.dll");
#else
		HMODULE hmdll = LoadLibrary(L"../Debug/OS11_HTAPI.dll");
#endif
		if (!hmdll)
			throw "-- LoadLibrary failed";
		cout << "-- LoadLibrary success" << endl;

		auto open = (ht::HtHandle * (*)(const wchar_t*, bool)) GetProcAddress(hmdll, "open");
		auto insert = (BOOL(*)(ht::HtHandle*, const ht::Element*)) GetProcAddress(hmdll, "insert");
		auto createInsertElement = (ht::Element * (*)(const void*, int, const void*, int)) GetProcAddress(hmdll, "createInsertElement");

		wchar_t* fileName = getWC(argv[1]);
		ht::HtHandle* ht = ht::open(fileName, true);

		if (ht)
			cout << "-- open: success" << endl;
		else
			throw "-- open: error";

		while (true) {
			int numberKey = rand() % 50;
			string key = intToString(numberKey);
			cout << key << endl;

			ht::Element* element = createInsertElement(key.c_str(), key.length() + 1, "0", 2);
			if (insert(ht, element))
				cout << "-- insert: success" << endl;
			else
				cout << "-- insert: error" << endl;

			delete element;

			Sleep(1000);
		}
	}
	catch (const char* msg)
	{
		cout << GetLastError()<< endl;
		cout << msg << endl;
	}
}

string intToString(int number)
{
	stringstream convert;
	convert << number;

	return convert.str();
}
wchar_t* getWC(const char* c)
{
	wchar_t* wc = new wchar_t[strlen(c) + 1];
	mbstowcs(wc, c, strlen(c) + 1);

	return wc;
}