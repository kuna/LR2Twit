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

	// PID로부터 핸들 값 얻어옴
	hProcess = OpenProcess(MAXIMUM_ALLOWED, FALSE, PID);

	// 해당 프로세스의 가상 메모리 남는 공간에 문자열 사이즈만큼 메모리 할당
	Virtual_Address = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);

	// 해당 메모리에 Injection 할 DLL 절대경로 넣어줌
	WriteProcessMemory(hProcess, Virtual_Address, (LPCVOID)PATH, dwBufSize, NULL);
	hMod = GetModuleHandleA("kernel32.dll");
	Funcation_Address = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryA");

	// LoadLibraryA를 실행시키고 인자는 아까 할당했던 메모리 주소
	// ThreadProc와 LoadLibrary의 인자, 리턴 크기가 같아서 가능!!
	// 여기서는 스레드를 생성하는 것이 아니라 실제로는 LoadLibraryA 함수를 호출
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