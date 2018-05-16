#include "stdafx.h"
#include "CppUnitTest.h"
#include "..\Include\UThread.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UthreadsTests
{		
	TEST_CLASS(UnitTest1)
	{
	private:
		// thread function for test RunningThreadIsAliveTest
		static VOID Func1(UT_ARGUMENT arg) {
			PBOOL isAlive = (PBOOL)arg;
			*isAlive = UtAlive(UtSelf());
		}

		static VOID Func2Aux(UT_ARGUMENT arg) {
			printf("Running Thread is %p\n", UtSelf());
		}

		static VOID Func2(UT_ARGUMENT arg) {
			PBOOL isAlive = (PBOOL)arg;
			HANDLE hThread = UtCreate(Func2Aux, NULL, 8 * 4096, "Thread1");
			//Waiting for hThread termination
			//UtJoin(hThread);
			UtYield();
			*isAlive = UtAlive( hThread);
		}
	public:
		
		TEST_METHOD(RunningThreadIsAliveTest)
		{
			UtInit();
			BOOL isAlive = FALSE;
			UtCreate(Func1, &isAlive,8*4096,"Thread1");
			UtRun();

			Assert::IsTrue( isAlive  == TRUE);
			UtEnd();
		}
		TEST_METHOD(UtMultiJoinTest)
		{
			UtInit();
			BOOL isAlive = FALSE;
			HANDLE hMainThread = UtCreate(Func1, &isAlive, 8 * 4096, "MainThread");
			HANDLE array[2];
			array[0] = UtCreate(Func1, &isAlive, 8 * 4096, "WorkerThread1");
			array[1] = UtCreate(Func1, &isAlive, 8 * 4096, "WorkerThread2");
			UtRun();
			UtMultiJoin(array, 2);
			printf("Counter : %d", UtGetCount(hMainThread));
			UtEnd();
			getchar();
		}

		TEST_METHOD(DumpTest)
		{
			UtInit();
			BOOL isAlive = TRUE;
			UtCreate(Func1, &isAlive, 8 * 4096,"Thread1");
			UtCreate(Func1, &isAlive, 4 * 4096,"Thread2");
			UtCreate(Func1, &isAlive, 2 * 4096,"Thread3");
			UtRun();
			UtDump();
			UtEnd();
		}

		TEST_METHOD(AfterUtExitNotAliveTest)
		{
			UtInit();
			BOOL isAlive = TRUE;
			UtCreate(Func2, &isAlive, 8 * 4096, "Thread1");
			UtRun();

			Assert::IsTrue(isAlive == FALSE);
			UtEnd();
		}

	};
}