#include <string>
#include <sstream>
#include "../OS11_HTAPI/pch.h"
#include "../OS11_HTAPI/HT.h"

using namespace std;

string intToString(int number);
int charToInt(char* str);
string incrementPayload(char* str);
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
		auto get = (ht::Element * (*)(const ht::HtHandle*, const ht::Element*)) GetProcAddress(hmdll, "get");
		auto getElement = (ht::Element * (*)(const void*, int)) GetProcAddress(hmdll, "createGetElement");
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
			ht::Element* getElem = getElement(key.c_str(), key.length() + 1);

			ht::Element* element = get(ht, getElem);
			if (element)
			{
				cout << "-- get: success" << endl;
				string newPayload = incrementPayload((char*)element->payload);

				if (update(ht, element, newPayload.c_str(), newPayload.length() + 1))
					cout << "-- update: success from " <<atoi(newPayload.c_str())-1<<" to "<<newPayload << endl;
				else
					cout << "-- update: error" << endl;
			}
			else
				cout << "-- get: error" << endl;

			Sleep(1000);
		}
	}
	catch (const char* msg)
	{
		cout << msg << endl;
	}
}

string intToString(int number)
{
	stringstream convert;
	convert << number;

	return convert.str();
}

int charToInt(char* str)
{
	stringstream convert;
	convert << str;
	int num;
	convert >> num;

	return num;
}

string incrementPayload(char* str)
{
	int oldNumberPayload = charToInt(str);
	int newNumberPayload = oldNumberPayload + 1;
	string newPayload = intToString(newNumberPayload);

	return newPayload;
}

wchar_t* getWC(const char* c)
{
	wchar_t* wc = new wchar_t[strlen(c) + 1];
	mbstowcs(wc, c, strlen(c) + 1);

	return wc;
}