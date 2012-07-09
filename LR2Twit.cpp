// LR2Twit.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "LR2Twit.h"

#include <atlcore.h> /*string.wstring*/
#include <shellapi.h>	/*shell icon*/

#include <io.h>
#include <mmsystem.h>
#pragma comment (lib,"winmm.lib")

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

// for lowlevel Hooking
HHOOK hHook;
LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode == HC_ACTION) {
		if (opt4 && c_dect->isResultScreen() && ((EVENTMSG*)lParam)->message == 'T' && wParam == WM_KEYUP) {
			doTwit();
		}
	}

	return CallNextHookEx(hHook, nCode, wParam, lParam);
}

void playAlarm() {
	if (access("alarm.wav", 0) == -1) return;
	PlaySoundA("alarm.wav", NULL, SND_ASYNC);
}

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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LR2_D));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDI_LR2_D);
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
      CW_USEDEFAULT, CW_USEDEFAULT, 240, 250, NULL, NULL, hInstance, NULL);
	m_hWnd = hWnd;

   if (!hWnd)
   {
      return FALSE;
   }

   // Detail proc before Screen
   if (!checkMutex()) {
	   PostQuitMessage(0);
	   return FALSE;
   }

   if (!m_log.InitLog(L".\\log.txt"))
   {
	   MessageBox(hWnd, L"로그 생성에 실패하였습니다.", L"", NULL);
	   PostQuitMessage(0);
	   return FALSE;
   }
   m_log.writeLogLine(L"---------------------------------");

   loadSettings();
   c_dect = new Detector(&m_log);
   c_twit = new TwitProc(&m_log);
   if (!c_twit->loadToken()) {
	   MessageBox(hWnd, L"OAuth 인증 토큰이 없습니다!\n설정창을 통하여 인증을 받아 주세요.", L"", NULL);
   }
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

   m_log.writeDate();
   m_log.writeLogLine(L"Program successfully launched");

   hHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)HookProc,
	   GetModuleHandle(NULL), NULL);
   if (hHook) {
	   m_log.writeLogLine(L"Successfully Hooked - WH_KEYBOARD_LL");
   } else {
	   m_log.writeLogLine(L"Hooking Failed - WH_KEYBOARD_LL");
   }

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
bool b_detected = false;
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
			10, 10, 200, 20, hWnd, (HMENU)ID_CHK1, hInst, NULL);
		c2_hWnd = CreateWindow(L"button", L"Only Tweet when Cleared", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			10, 30, 200, 20, hWnd, (HMENU)ID_CHK2, hInst, NULL);
		c3_hWnd = CreateWindow(L"button", L"Only Tweet when New Record", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			10, 50, 200, 20, hWnd, (HMENU)ID_CHK3, hInst, NULL);
		c4_hWnd = CreateWindow(L"button", L"Tweet when press \'T\' Key", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			10, 70, 200, 20, hWnd, (HMENU)ID_CHK4, hInst, NULL);

		e1_hWnd = CreateWindow(L"edit", L"", WS_CHILD | WS_VISIBLE | BS_TEXT | WS_BORDER,
			10, 100, 200, 20, hWnd, (HMENU)0, hInst, NULL);
		e2_hWnd = CreateWindow(L"edit", L"", WS_CHILD | WS_VISIBLE | BS_TEXT | WS_BORDER | ES_PASSWORD,
			10, 120, 200, 20, hWnd, (HMENU)0, hInst, NULL);
		b3_hWnd = CreateWindow(L"button", L"Auth Tweet Account", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			10, 140, 200, 20, hWnd, (HMENU)ID_TWEETAUTH, hInst, NULL);

		b1_hWnd = CreateWindow(L"button", L"OK", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			10, 170, 60, 20, hWnd, (HMENU)ID_OK, hInst, NULL);
		b2_hWnd = CreateWindow(L"button", L"Cancel", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			80, 170, 60, 20, hWnd, (HMENU)ID_CANCEL, hInst, NULL);
		b4_hWnd = CreateWindow(L"button", L"Twit", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			160, 170, 40, 20, hWnd, (HMENU)ID_TWIT, hInst, NULL);

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
			setSettings();
			ShowWindow(m_hWnd, SW_SHOW);
			break;
		case ID_MENU_EXIT:
			delTray();
			m_log.endLog();
			PostQuitMessage(0);
			break;

			/* button */
		case ID_OK:
			saveSettings();
			ShowWindow(m_hWnd, SW_HIDE);
			break;
		case ID_CANCEL:
			ShowWindow(m_hWnd, SW_HIDE);
			break;
		case ID_TWIT:
			if (!c_dect->isCleared()) {
				MessageBox(m_hWnd, L"클리어 화면에서만 사용 가능한 기능입니다.\nOnly can be used at result screen status.", L"", NULL);
			} else {
				if (c_dect->isAutoPlaying()) {
					MessageBox(m_hWnd, L"AutoPlay에서는 사용하실 수 없습니다.", L"", NULL);
					break;
				}
				TCHAR str[255];
				c_dect->getLR2StatusString(str);
				int r = MessageBox(m_hWnd, wstring(str).append(L"\n\n위 내용으로 트윗하시겠습니까?\nTwit this message?").c_str(), L"", MB_YESNO);
				if (r == IDYES) {
					doTwit();
				}
			}
			break;
		case ID_TWEETAUTH:
			{
				char _id[255], _pass[255];
				GetWindowTextA(e1_hWnd, _id, 255);
				GetWindowTextA(e2_hWnd, _pass, 255);
				if (c_twit->getToken(_id, _pass)) {
					MessageBox(m_hWnd, L"인증에 성공했습니다!\nAuthentication Succeed.", L"", NULL);
				} else {
					MessageBox(m_hWnd, L"인증에 실패했습니다! 계정을 확인해 주세요.\nAuthentication Failed. Check out your account", L"", NULL);
				}
				break;
			}
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
		removeMutex();
		UnhookWindowsHookEx(hHook);
		PostQuitMessage(0);
		break;
	case WM_TIMER:
		if (!c_dect->getLR2Status()) {
			changeTray(false);
			c_dect->detectLR2();
			b_detected = false;
			break;
		} else {
			if (!b_detected) {
				playAlarm();
				b_detected = true;
			}
			changeTray(true);
		}

		InvalidateRect(hWnd, NULL, TRUE);
		if (c_dect->isResultScreen() && !twit_Enabled &&
			!(twit_high && !c_dect->isHighScore()) &&
			!(twit_clear && !c_dect->isCleared())) {	// these two should be false
			twit_Enabled = true;
			twit_Twitted = false;
		}
		if (!c_dect->isResultScreen() && twit_Enabled) {
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
			TCHAR str[255];
			c_dect->getLR2StatusString(str);
			//MessageBox(hWnd, str, L"", MB_OK);
			OutputDebugString(wstring(str).append(L"\n").c_str());
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
	// for cancel!
	if (c_dect->LR2stat[LR_SCORE] == 0) return;
	if (c_dect->isAutoPlaying()) return;

	TCHAR str[255];
	c_dect->getLR2StatusString(str);
	str[140] = L'\0';	// max 140 char
	m_log.writeLogLine(L"[Twit]", str);
	//MessageBox(hWnd, str, L"", MB_OK);

	char* pTemp = NULL;
	int iLen = ::WideCharToMultiByte(CP_ACP, 0, str, -1, pTemp, 0, NULL, NULL);
	pTemp = new char[iLen+1];
	::WideCharToMultiByte(CP_ACP, 0, str, -1, pTemp, iLen, NULL, NULL);

	string astr = string(pTemp);
	delete [] pTemp;

	OutputDebugStringA(astr.c_str());
	if (c_twit->sendTwit( astr )) {
		playAlarm();
	}
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
    lstrcpy(nid.szTip, L"LR2Twit"); 
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
		lstrcpy(data.szTip, L"LR2Twit (detected)"); 
	} else {
		data.hIcon = LoadIcon( hInst, MAKEINTRESOURCE(IDI_LR2_D) );
		lstrcpy(data.szTip, L"LR2Twit (undetected)"); 
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
	opt1 = (SendMessage(c1_hWnd,BM_GETCHECK,0,0) == BST_CHECKED);
	opt2 = (SendMessage(c2_hWnd,BM_GETCHECK,0,0) == BST_CHECKED);
	opt3 = (SendMessage(c3_hWnd,BM_GETCHECK,0,0) == BST_CHECKED);

	twit_auto = opt1;
	twit_clear = opt2;
	twit_high = opt3;


	TCHAR _s[255];
	_itow(opt1, _s, 10);
	WritePrivateProfileStringW(L"LR2TWIT", L"LR2_AUTOTWIT", _s, L".\\settings.ini" );
	_itow(opt2, _s, 10);
	WritePrivateProfileStringW(L"LR2TWIT", L"TWIT_CLEAR", _s, L".\\settings.ini" );
	_itow(opt3, _s, 10);
	WritePrivateProfileStringW(L"LR2TWIT", L"TWIT_HIGHSCORE", _s, L".\\settings.ini" );
	_itow(opt4, _s, 10);
	WritePrivateProfileStringW(L"LR2TWIT", L"TWIT_SHORTCUTKEY", _s, L".\\settings.ini" );
}

void loadSettings() {
	TCHAR _s[255];

	GetPrivateProfileStringW(L"LR2TWIT", L"LR2_TITLE", L"LR2 beta3 version 100201", _s, 255, L".\\settings.ini");
	OutputDebugString(_s);
	lstrcpy(opt_lr2title, _s);
	GetPrivateProfileStringW(L"LR2TWIT", L"LR2_AUTOTWIT", L"1", _s, 255, L".\\settings.ini");
	opt1 = _wtoi(_s);
	GetPrivateProfileStringW(L"LR2TWIT", L"TWIT_MESSAGE", L"<[TITLE] ([RANK]) [GUAGE] [RESULT]!![AUTO] [RATE]%([RANK]) - [EXS]/[EXMS] (PG[PG]/GR[GR]/GD[GD]/PR[PR]/BD[BD], [MC] Combo) #LR2", _s, 255, L".\\settings.ini");
	lstrcpy(opt_message, _s);
	OutputDebugString(_s);
	GetPrivateProfileStringW(L"LR2TWIT", L"TWIT_ENCODING", L"CP949", _s, 255, L".\\settings.ini");
	wcstombs(opt_encode, _s, 255);
	GetPrivateProfileStringW(L"LR2TWIT", L"TWIT_CLEAR", L"1", _s, 255, L".\\settings.ini");
	opt2 = _wtoi(_s);
	GetPrivateProfileStringW(L"LR2TWIT", L"TWIT_HIGHSCORE", L"1", _s, 255, L".\\settings.ini");
	opt3 = _wtoi(_s);
	GetPrivateProfileStringW(L"LR2TWIT", L"TWIT_SHORTCUTKEY", L"1", _s, 255, L".\\settings.ini");
	opt4 = _wtoi(_s);

	twit_auto = opt1;
	twit_clear = opt2;
	twit_high = opt3;
}

void setSettings() {
	SendMessage(c1_hWnd,BM_SETCHECK,opt1?BST_CHECKED:BST_UNCHECKED,0);
	SendMessage(c2_hWnd,BM_SETCHECK,opt2?BST_CHECKED:BST_UNCHECKED,0);
	SendMessage(c3_hWnd,BM_SETCHECK,opt3?BST_CHECKED:BST_UNCHECKED,0);
	SendMessage(c4_hWnd,BM_SETCHECK,opt4?BST_CHECKED:BST_UNCHECKED,0);
}

bool checkMutex()
{
	m_hMutex = CreateMutex(0, TRUE, L"LR2TWIT");
	DWORD dwErr = GetLastError();

	if (m_hMutex == NULL) return false;

	if (dwErr == ERROR_ALREADY_EXISTS) {
		//ReleaseMutex(m_hMutex);
		//CloseHandle(m_hMutex);
		return false;
	}

	return true;
}

void removeMutex()
{
	ReleaseMutex(m_hMutex);
	CloseHandle(m_hMutex);
	m_hMutex = 0;
}
