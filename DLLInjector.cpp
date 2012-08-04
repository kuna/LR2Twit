#include "stdafx.h"
#include "DLLInjector.h"

typedef BOOL (__cdecl *fn_CaptureScreen)();

DWORD DLLInjector::GET_PID (char *win_Class, char *win_Title) {
	 HWND hWnd = FindWindowA(win_Class, win_Title);	//"LR2 SETUP"
	 if (!hWnd) return 0;
	 DWORD pID;
	 GetWindowThreadProcessId(hWnd, &pID);
	 return pID;
}

BOOL DLLInjector::DLL_Injection(int PID, char* PATH) {
	// path check
	char npath[256];
	strcpy(npath, PATH);
	if (memcmp(npath, ".\\\\", 3) == 0) {
		char str[256];
		GetModuleFileNameA(0, str, 256);
		char *p = strrchr(str, '\\');
		*p = 0;
		strcat(str, npath+2);
		strcpy(npath, str);
	}
	OutputDebugStringA(npath);
	OutputDebugStringA("\n");

	HANDLE hProcess, hThread;
	HMODULE hMod;
	LPVOID Virtual_Address;
	DWORD dwBufSize = strlen(npath) + 1;
	LPTHREAD_START_ROUTINE Funcation_Address;

	// PID�κ��� �ڵ� �� ����
	hProcess = OpenProcess(MAXIMUM_ALLOWED, FALSE, PID);

	// �ش� ���μ����� ���� �޸� ���� ������ ���ڿ� �����ŭ �޸� �Ҵ�
	Virtual_Address = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);

	// �ش� �޸𸮿� Injection �� DLL ������ �־���
	WriteProcessMemory(hProcess, Virtual_Address, (LPCVOID)npath, dwBufSize, NULL);
	hMod = GetModuleHandleA("kernel32.dll");
	Funcation_Address = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryA");

	// LoadLibraryA�� �����Ű�� ���ڴ� �Ʊ� �Ҵ��ߴ� �޸� �ּ�
	// ThreadProc�� LoadLibrary�� ����, ���� ũ�Ⱑ ���Ƽ� ����!!
	// ���⼭�� �����带 �����ϴ� ���� �ƴ϶� �����δ� LoadLibraryA �Լ��� ȣ��
	hThread = CreateRemoteThread(hProcess, NULL, 0, Funcation_Address, Virtual_Address, 0, NULL);
	WaitForSingleObject(hThread, 200);
	GetExitCodeThread(hThread, (LPDWORD)&g_hModule);

	CloseHandle(hThread);
	CloseHandle(hProcess);

	return TRUE;
}

BOOL DLLInjector::Inject(char *win_Class, char *win_Title, char *dll_Path) {
	PID = GET_PID(win_Class, win_Title);
	if (!PID) {
		//printf("Cannot find process\n");
		return FALSE;
	}

	DLL_Injection(PID, dll_Path);

	return TRUE;
}

BOOL DLLInjector::Eject() {
	if (!PID || !g_hModule) return FALSE;

	HANDLE hProcess, hThread;
	HMODULE hMod;
	LPVOID Virtual_Address;
	LPTHREAD_START_ROUTINE Funcation_Address;

	// PID�κ��� �ڵ� �� ����
	hProcess = OpenProcess(MAXIMUM_ALLOWED, FALSE, PID);

	hMod = GetModuleHandleA("kernel32.dll");
	Funcation_Address = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "FreeLibrary");

	// LoadLibraryA�� �����Ű�� ���ڴ� �Ʊ� �Ҵ��ߴ� �޸� �ּ�
	// ThreadProc�� LoadLibrary�� ����, ���� ũ�Ⱑ ���Ƽ� ����!!
	// ���⼭�� �����带 �����ϴ� ���� �ƴ϶� �����δ� LoadLibraryA �Լ��� ȣ��
	hThread = CreateRemoteThread(hProcess, NULL, 0, Funcation_Address, g_hModule, 0, NULL);
	WaitForSingleObject(hThread, 200);

	CloseHandle(hThread);
	CloseHandle(hProcess);

	g_hModule = 0;

	return TRUE;
}