#include "stdafx.h"
#include "DLLInjector.h"

DWORD DLLInjector::GET_PID (char *win_Class, char *win_Title) {
	 HWND hWnd = FindWindowA(win_Class, win_Title);	//"LR2 SETUP"
	 if (!hWnd) return 0;
	 DWORD pID;
	 GetWindowThreadProcessId(hWnd, &pID);
	 return pID;
}

BOOL DLLInjector::DLL_Injection(int PID, char* PATH) {
	HANDLE hProcess, hThread;
	HMODULE hMod;
	LPVOID Virtual_Address;
	DWORD dwBufSize = strlen(PATH) + 1;
	LPTHREAD_START_ROUTINE Funcation_Address;

	// PID�κ��� �ڵ� �� ����
	hProcess = OpenProcess(MAXIMUM_ALLOWED, FALSE, PID);

	// �ش� ���μ����� ���� �޸� ���� ������ ���ڿ� �����ŭ �޸� �Ҵ�
	Virtual_Address = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);

	// �ش� �޸𸮿� Injection �� DLL ������ �־���
	WriteProcessMemory(hProcess, Virtual_Address, (LPCVOID)PATH, dwBufSize, NULL);
	hMod = GetModuleHandleA("kernel32.dll");
	Funcation_Address = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryA");

	// LoadLibraryA�� �����Ű�� ���ڴ� �Ʊ� �Ҵ��ߴ� �޸� �ּ�
	// ThreadProc�� LoadLibrary�� ����, ���� ũ�Ⱑ ���Ƽ� ����!!
	// ���⼭�� �����带 �����ϴ� ���� �ƴ϶� �����δ� LoadLibraryA �Լ��� ȣ��
	hThread = CreateRemoteThread(hProcess, NULL, 0, Funcation_Address, Virtual_Address, 0, NULL);
	WaitForSingleObject(hThread, 200);

	CloseHandle(hThread);
	CloseHandle(hProcess);

	return TRUE;
}

BOOL DLLInjector::Inject(char *win_Class, char *win_Title, char *dll_Path) {

	DWORD PID = 0;
	PID = GET_PID(win_Class, win_Title);
	if (!PID) {
		//printf("Cannot find process\n");
		return FALSE;
	}

	//DLL_Injection(PID, "C:\\Users\\kuna\\Documents\\Visual Studio 2005\\Projects\\DxHookDLL\\Debug\\DXHookDLL.dll");
	DLL_Injection(PID, "D:\\[Projects]\\DLLSubClass\\Debug\\LR2DLL.dll");

	return TRUE;
}