#ifdef _WIN64
#pragma comment(lib, "../x64/debug/OS10_HTAPI.lib")
#else
#pragma comment(lib, "../debug/OS10_HTAPI.lib")
#endif

#include "../OS10_HTAPI/pch.h"
#include "../OS10_HTAPI/HT.h"

using namespace std;

int main()
{
	ht::HtHandle* ht1 = nullptr;
	ht::HtHandle* ht2 = nullptr;

	#pragma region ht1

	try
	{
		cout <<  "ht1" << endl;
		ht1 = ht::create(1000, 3, 10, 256, L"./files/HTspace.ht");
		if (ht1)
			cout << "-- create: success" << endl;
		else
			throw "-- create: error";


		if (ht::insert(ht1, new ht::Element("svn", 4, "slemnev", 8)))
			cout << "\n-- insert: success" << endl;
		else
			throw "-- insert: error";
		ht::Element* hte = ht::get(ht1, new ht::Element("svn", 4));
		if (hte)
			cout << "-- get: success" << endl;
		else
			throw "-- get: error";
		ht::print(hte);
	}
	catch (const char* msg)
	{
		cout << msg << endl;

		if (ht1 != nullptr)
			cout << ht::getLastError(ht1) << endl;
	}

	#pragma endregion

	#pragma region ht2

	try
	{
		cout << "\n ht2 " << endl;
		ht2 = ht::open(L"./files/HTspace.ht", true);
		if (ht2)
			cout << "-- open: success" << endl;
		else
			throw "-- open: error";


		if (ht::insert(ht2, new ht::Element("svn2", 5, "slemnev2", 9)))
			cout << "\n-- insert: success" << endl;
		else
			throw "-- insert: error";
		ht::Element* hte2 = ht::get(ht2, new ht::Element("svn2", 5));
		if (hte2)
			cout << "-- get: success" << endl;
		else
			throw "-- get: error";

		ht::print(hte2);
	}
	catch (const char* msg)
	{
		cout << msg <<GetLastError()<< endl;

		if (ht1 != nullptr)
			cout << ht::getLastError(ht1) << endl;
	}

	#pragma endregion

	try
	{
		
		if (ht2 != nullptr)
			if (ht::close(ht2))
				cout << "-- close: success (ht2)" << endl;
			else
				throw "-- close: error (ht2)";
		if (ht1 != nullptr)
			if (ht::close(ht1))
				cout << "-- close: success (ht1)" << endl;
			else
				throw "-- close: error (ht1)";

	}
	catch (const char* msg)
	{
		cout << msg << endl;
	}
}