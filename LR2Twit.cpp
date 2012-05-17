// LR2Twit.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "LR2Twit.h"

#include <atlcore.h> /*string.wstring*/
#include <shellapi.h>	/*shell icon*/


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_LR2TWIT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LR2TWIT));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LR2TWIT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_LR2TWIT);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, 240, 240, NULL, NULL, hInstance, NULL);
	m_hWnd = hWnd;

   if (!hWnd)
   {
      return FALSE;
   }

   // Detail proc before Screen
   c_dect = new Detector();
   c_twit = new TwitProc();
   c_twit->getToken("kuna_KR", "k9021927");
   //if (!c_dect->detectLR2()) {
//	   MessageBox(hWnd, L"LR2 창을 찾을 수 없습니다!", L"", NULL);
//	   DestroyWindow(hWnd);
//	   return FALSE;
  // }

   SetTimer(hWnd, 1, 1000, NULL);
   doTray();
   // Proc End

   ShowWindow(hWnd, SW_HIDE); //nCmdShow
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	RECT rt;
	TCHAR s[100];


	switch (message)
	{
	case WM_CREATE:
		c1_hWnd = CreateWindow(L"button", L"Automatic Result Tweet", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			10, 10, 200, 20, hWnd, NULL, hInst, NULL);
		c2_hWnd = CreateWindow(L"button", L"Only Tweet when Cleared", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			10, 30, 200, 20, hWnd, (HMENU)0, hInst, NULL);
		c3_hWnd = CreateWindow(L"button", L"Only Tweet when New Record", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			10, 50, 200, 20, hWnd, (HMENU)0, hInst, NULL);

		e1_hWnd = CreateWindow(L"edit", L"", WS_CHILD | WS_VISIBLE | BS_TEXT | WS_BORDER,
			10, 80, 200, 20, hWnd, (HMENU)0, hInst, NULL);
		e2_hWnd = CreateWindow(L"edit", L"", WS_CHILD | WS_VISIBLE | BS_TEXT | WS_BORDER | ES_PASSWORD,
			10, 100, 200, 20, hWnd, (HMENU)0, hInst, NULL);
		b3_hWnd = CreateWindow(L"button", L"Auth Tweet Account", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			10, 120, 200, 20, hWnd, (HMENU)0, hInst, NULL);

		b1_hWnd = CreateWindow(L"button", L"OK", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			10, 150, 80, 20, hWnd, (HMENU)0, hInst, NULL);
		b2_hWnd = CreateWindow(L"button", L"Cancel", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			100, 150, 80, 20, hWnd, (HMENU)0, hInst, NULL);

		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_MENU_SETTINGS:
			ShowWindow(m_hWnd, SW_SHOW);
			break;
		case ID_MENU_EXIT:
			delTray();
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_CLOSE:
		ShowWindow(m_hWnd, SW_HIDE);
		return FALSE;
		break;
	case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, &ps);

			// TODO: Add any drawing code here...
			swprintf(s, L"SCORE:%d", c_dect->LR2stat[LR_SCORE]);
			SetRect(&rt, 10, 210, 200, 230);
			DrawText(hdc, s, lstrlen(s), &rt, DT_LEFT);
			
			swprintf(s, L"%s", c_dect->LR2BMSTitle);
			SetRect(&rt, 10, 230, 200, 250);
			DrawText(hdc, s, lstrlen(s), &rt, DT_LEFT);
			
			swprintf(s, L"GUAGE:%d", c_dect->LR2stat[LR_GUAGE]);
			SetRect(&rt, 10, 250, 200, 270);
			DrawText(hdc, s, lstrlen(s), &rt, DT_LEFT);
			
			if (twit_Enabled && !twit_Twitted) {
				lstrcpy(s, L"RESULT SCREEN - SUBMIT SCORE");
				SetRect(&rt, 10, 280, 200, 300);
				DrawText(hdc, s, lstrlen(s), &rt, DT_LEFT);
			}
			// Drawing END

			EndPaint(hWnd, &ps);
			break;
		}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_TIMER:

		if (!c_dect->getLR2Status()) {
			changeTray(false);
			c_dect->detectLR2();
			break;
		} else {
			changeTray(true);
		}

		InvalidateRect(hWnd, NULL, TRUE);
		if (c_dect->LR2stat[LR_STAT] == 2 && !twit_Enabled) {
			twit_Enabled = true;
			twit_Twitted = false;
		}
		if (c_dect->LR2stat[LR_STAT] != 2 && twit_Enabled) {
			twit_Enabled = false;
		}

		// automatic Twit
		if (twit_auto && twit_Enabled && !twit_Twitted) {
			doTwit();
			twit_Twitted = true;
		}

		break;
	case WM_LBUTTONUP:
		{
			TCHAR str[100];
			c_dect->getLR2StatusString(str);
			MessageBox(hWnd, str, L"", MB_OK);
			if (twit_Enabled && !twit_Twitted) {
				doTwit();
				twit_Twitted = true;
			}
			break;
		}
	case ID_NOTIFY_CLICK:
		{
			if (lParam == WM_RBUTTONUP) {
				HMENU hMenu_Parent = LoadMenu( hInst, MAKEINTRESOURCE(IDR_NOTIFY) );
				HMENU hMenu = GetSubMenu(hMenu_Parent, 0);
				POINT pt;
				GetCursorPos(&pt);
				SetForegroundWindow(m_hWnd);
				TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, m_hWnd, NULL);
			}
			break;
		}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


/*****************************************
Redeclarations
*****************************************/

void doTwit() {
	
			TCHAR str[100];
			c_dect->getLR2StatusString(str);
			//MessageBox(hWnd, str, L"", MB_OK);

			char* pTemp = NULL;
			int iLen = ::WideCharToMultiByte(CP_ACP, 0, str, -1, pTemp, 0, NULL, NULL);
			pTemp = new char[iLen+1];
			::WideCharToMultiByte(CP_ACP, 0, str, -1, pTemp, iLen, NULL, NULL);

			string astr = string(pTemp);
			delete [] pTemp;

			OutputDebugStringA(astr.c_str());
			c_twit->sendTwit( astr );
}

void doTray() {
	NOTIFYICONDATA  nid;
    nid.cbSize = sizeof(nid);
    nid.hWnd = m_hWnd; // 메인 윈도우 핸들
    nid.uID = ID_NOTIFY;  // 아이콘 리소스 ID
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP; // 플래그 설정
    nid.hIcon = LoadIcon( hInst, MAKEINTRESOURCE(IDI_LR2_D) ); // 아이콘 로드 
	nid.uCallbackMessage = ID_NOTIFY_CLICK;

    TCHAR strTitle[256];
    GetWindowText(m_hWnd, strTitle, sizeof(strTitle)); // 캡션바에 출력된 문자열 얻음
    lstrcpy(nid.szTip, strTitle); 
    Shell_NotifyIcon(NIM_ADD, &nid);
    SendMessage(m_hWnd, WM_SETICON, (WPARAM)TRUE, (LPARAM)nid.hIcon);
}

void changeTray(bool detect) {
	// 변경
	NOTIFYICONDATA data;
	data.cbSize = sizeof(NOTIFYICONDATA);
	data.hWnd = m_hWnd;
	data.uID = ID_NOTIFY;
	data.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	//strcpy( data.szTip, "메세지 도착" );
	if (detect) {
		data.hIcon = LoadIcon( hInst, MAKEINTRESOURCE(IDI_LR2_E) );
	} else {
		data.hIcon = LoadIcon( hInst, MAKEINTRESOURCE(IDI_LR2_D) );
	}
	data.uCallbackMessage = ID_NOTIFY_CLICK;

	Shell_NotifyIcon( NIM_MODIFY, &data );
}

void delTray() {
	// 삭제
	NOTIFYICONDATA data;
	data.cbSize = sizeof(NOTIFYICONDATA);
	data.hWnd = m_hWnd;
	data.uID = ID_NOTIFY;
	Shell_NotifyIcon( NIM_DELETE, &data );
}

void saveSettings() {
}

void loadSettings() {/*
	char _s[255];
	GetPrivateProfileString("LR2TWIT", "LR2_TITLE", "", _s, 255, ".\\settings.ini");
	GetPrivateProfileString("LR2TWIT", "TWIT_ID", "", _s, 255, ".\\settings.ini");
	GetPrivateProfileString("LR2TWIT", "TWIT_PASS", "", _s, 255, ".\\settings.ini");
	GetPrivateProfileString("LR2TWIT", "LR2_AUTOTWIT", "", _s, 255, ".\\settings.ini");
	GetPrivateProfileString("LR2TWIT", "TWIT_MESSAGE", "", _s, 255, ".\\settings.ini");
	GetPrivateProfileString("LR2TWIT", "TWIT_ENCODING", "", _s, 255, ".\\settings.ini");
	GetPrivateProfileString("LR2TWIT", "TWIT_NOTHIGHSCORE", "1", _s, 255, ".\\settings.ini");
	GetPrivateProfileString("LR2TWIT", "TWIT_NOTCLEAR", "0", _s, 255, ".\\settings.ini");
*/}