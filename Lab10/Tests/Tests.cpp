#include "pch.h"
#include "CppUnitTest.h"

#pragma once
#ifdef _WIN64
#pragma comment(lib, "../x64/debug/OS10_HTAPI.lib")
#else
#pragma comment(lib, "../debug/OS10_HTAPI.lib")
#endif

#include "../OS10_HTAPI/pch.h"
#include "../OS10_HTAPI/HT.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{
	TEST_CLASS(Tests)
	{
	public:
		
		TEST_METHOD(CreateTest)
		{
			ht::HtHandle* htt = ht::create(1000, 3, 10, 256, L"./f/test1.ht");

			
			Assert::IsNull(htt);
		

		};
		TEST_METHOD(RemoveTest) {
			ht::HtHandle* ht = ht::create(1000, 3, 10, 256, L"./test2.ht");

			ht::Element* element = new ht::Element("test2", 6, "test2", 6);

			ht::insert(ht, element);
			ht::removeOne(ht, element);
			Assert::AreNotEqual(ht->lastErrorMessage, "-- not found element (DELETE)");
			ht::close(ht);
		}
		
		
		TEST_METHOD(NonExistingDeleting) {
			ht::HtHandle* ht = ht::create(1000, 3, 10, 256, L"./test4.ht");

			ht::Element* element = new ht::Element("test3", 6, "test3", 6);

			ht::insert(ht, element);
			ht::removeOne(ht, element);
			ht::removeOne(ht, element);
			Assert::AreEqual(ht->lastErrorMessage, "-- not found element (DELETE)");
			ht::close(ht);				
		}

		TEST_METHOD(NonExistingGet) {
			ht::HtHandle* ht = ht::create(1000, 3, 10, 256, L"./test5.ht");

			ht::Element* hte = ht::get(ht, new ht::Element("svn", 4));
			
			Assert::AreEqual(ht->lastErrorMessage, "-- not found element (GET)");
			ht::close(ht);
		}
		TEST_METHOD(OverCapacity) {
			ht::HtHandle* ht = ht::create(1, 3, 10, 256, L"./test7.ht");

			ht::Element* element = new ht::Element("test3", 6, "test3", 6);

			ht::insert(ht, element);
			ht::insert(ht, element);
			Assert::AreEqual(ht->lastErrorMessage, "-- not found free memory");
			ht::close(ht);

		}
	};
}
