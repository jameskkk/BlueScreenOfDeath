// BlueScreenOfDeath.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <winternl.h>
#include <iostream>
#include <cstdlib>

#pragma comment(lib, "ntdll.lib")

using namespace std;

EXTERN_C NTSTATUS NTAPI RtlAdjustPrivilege(ULONG, BOOLEAN, BOOLEAN, PBOOLEAN);
EXTERN_C NTSTATUS NTAPI NtSetInformationProcess(HANDLE, ULONG, PVOID, ULONG);
// Let user's PC go to BSOD
int RunBlueScreenOfDeath1()
{
	BOOLEAN bl;
	int isCritical = 1;
	UINT BreakOnTermination = 0x1D;

	if (!NT_SUCCESS(RtlAdjustPrivilege(20, TRUE, FALSE, &bl)))
	{
		printf("Unable to enable SeDebugPrivilege. Make sure you are running this program as administrator.");
		return -1;
	}

	// setting the BreakOnTermination = 1 for the current process
	NTSTATUS status = NtSetInformationProcess(GetCurrentProcess(), BreakOnTermination, &isCritical, sizeof(ULONG));
	if (status != 0)
	{
		printf("Error: Unable to cancel critical process status. NtSetInformationProcess failed with status %#lx\n", status);
	}

	else
	{
		printf("Successfully canceled critical process status.\n");
	}

	return 0;
}

typedef NTSTATUS(NTAPI *pdef_NtRaiseHardError)(NTSTATUS ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask OPTIONAL, PULONG_PTR Parameters, ULONG ResponseOption, PULONG Response);
typedef NTSTATUS(NTAPI *pdef_RtlAdjustPrivilege)(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);
int RunBlueScreenOfDeath2()
{
	BOOLEAN bEnabled;
	ULONG uResp;

	LPVOID lpFuncAddress = reinterpret_cast<LPVOID>(GetProcAddress(LoadLibrary(L"ntdll.dll"), "RtlAdjustPrivilege"));
	LPVOID lpFuncAddress2 = reinterpret_cast<LPVOID>(GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtRaiseHardError"));
	pdef_RtlAdjustPrivilege NtCall = reinterpret_cast<pdef_RtlAdjustPrivilege>(lpFuncAddress);
	pdef_NtRaiseHardError NtCall2 = reinterpret_cast<pdef_NtRaiseHardError>(lpFuncAddress2);
	NTSTATUS NtRet = NtCall(19, TRUE, FALSE, &bEnabled);
	NtCall2(static_cast<long>(STATUS_FLOAT_MULTIPLE_FAULTS), 0, 0, nullptr, 6, &uResp);
	cout << "BlueScreen() NtRet = " << NtRet;

	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	printf("Enter any key to blue screen of death!\n");
	system("PAUSE");
	printf("Good bye!\n");

	RunBlueScreenOfDeath1();
	//RunBlueScreenOfDeath2();

	return 0;
}

