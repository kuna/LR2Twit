// LR2Twit.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "LR2Twit.h"
#include "DLLInjector.h"
#include "lang.h"

#include <atlcore.h> /*string.wstring*/
#include <shellapi.h>	/*shell icon*/
#include <process.h>	/* thread */

#include <io.h>
#include <mmsystem.h>
#pragma comment (lib,"winmm.lib")

#define MAX_LOADSTRING 100
#define CAPTURE_DELAY 3500

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

// language
LanguageSetting m_Lang;

// DLL Injector
DLLInjector g_DLL;

// for lowlevel Hooking
HHOOK hHook;
LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode == HC_ACTION) {
		int bControlKeyDown = GetAsyncKeyState (VK_CONTROL);

		if (bControlKeyDown && GetAsyncKeyState (VK_SHIFT) && !c_dect->isPlaying()) {
			// show level
			c_dect->getLR2Status();

			TCHAR s1[256], s2[256];
			lstrcpy(s1, c_dect->LR2BMSTitle);
			c_dect->getLevel(s2);

			char msg[25];
			int iLen = ::WideCharToMultiByte(CP_ACP, 0, s2, -1, msg, 0, NULL, NULL);
			::WideCharToMultiByte(CP_ACP, 0, s2, -1, msg, iLen, NULL, NULL);

			setMessage(msg);
		}

		if (bControlKeyDown && wParam == WM_KEYUP)
		{
			if (((EVENTMSG*)lParam)->message == 'U') {
				// update file
				if (c_dect->DownloadInsaneLevel()) {
					MessageBox(0, L"Successfully download: getinsanelist.cgi", 0, 0);
					c_dect->LoadInsaneLevel();
				} else {
					MessageBox(0, L"Failed to download: getinsanelist.cgi", 0, 0);
				}
			}
		}
		if (bControlKeyDown && wParam == WM_KEYUP && opt4 && c_dect->isLR2Vaild()) {
			if (((EVENTMSG*)lParam)->message == 'T' && c_dect->isResultScreen()) {
				if (c_dect->isAutoPlaying()) {
					setMessage( m_Lang.GetLanguageA("DIALOG", "NoAutoPlay").c_str() );
				} else {
					doTwit(0);
				}
			}
			if (((EVENTMSG*)lParam)->message == 'R' && c_dect->isResultScreen()) {
				if (c_dect->isAutoPlaying()) {
					setMessage( m_Lang.GetLanguageA("DIALOG", "NoAutoPlay").c_str() );
				} else {
					doTwitwithPic(0);
				}
			}
			if (((EVENTMSG*)lParam)->message == 'Y' && c_dect->isPlaying()) {
				twit_cancel = !twit_cancel;

				if (twit_cancel) {
					setMessage( m_Lang.GetLanguageA("HOOK", "TwitCancel").c_str() );
				} else {
					setMessage( m_Lang.GetLanguageA("HOOK", "TwitRetry").c_str() );
				}
			}
			if (((EVENTMSG*)lParam)->message == 'Q') {
				c_dect->setRecordAlways();
				setMessage( m_Lang.GetLanguageA("HOOK", "RecordAlways").c_str() );
			}
			if (((EVENTMSG*)lParam)->message == 'A') {
				// REInspect
				if (g_DLL.isDLLInjected(NULL, "LR2 beta3 version 100201", L"LR2DLL.dll"))
					g_DLL.Eject();
				g_DLL.Inject(NULL, "LR2 beta3 version 100201", ".\\\\LR2DLL.dll");
				setMessage("DLL Re-Injected.");
			}
			if (((EVENTMSG*)lParam)->message == 'E') {
				// Eject
				g_DLL.Eject();
				setMessage("DLL Ejected.");
			}
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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LR2_E));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDI_LR2_E);
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
      CW_USEDEFAULT, CW_USEDEFAULT, 240, 290, NULL, NULL, hInstance, NULL);
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
	   MessageBox(hWnd, m_Lang.GetLanguageW(L"DIALOG", L"FailLog").c_str(), L"", NULL);
	   PostQuitMessage(0);
	   return FALSE;
   }
   m_log.writeLogLine(L"---------------------------------");

   loadSettings();
   c_dect = new Detector(&m_log);
   c_dect->LoadInsaneLevel();

   c_twit = new TwitProc(&m_log);
   if (!c_twit->loadToken()) {
	   MessageBox(hWnd, m_Lang.GetLanguageW(L"DIALOG", L"NoOAuthToken").c_str(), L"", NULL);
   }

   SetTimer(hWnd, 1, 1000, NULL);
   doTray();
   // Proc End

   ShowWindow(hWnd, SW_HIDE); //nCmdShow
   UpdateWindow(hWnd);

   m_log.writeDate();
   m_log.writeLogLine(L"Program successfully launched");

   setSharedMemory();
   m_log.writeLogLine(L"setSharedMemory(); Excuted");

   startKeyHook();
   g_DLL.l = &m_log;

   return TRUE;
}


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
		c1_hWnd = CreateWindow(L"button", m_Lang.GetLanguageW(L"DIALOG", L"Dlg1").c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			10, 10, 200, 20, hWnd, (HMENU)ID_CHK1, hInst, NULL);
		c2_hWnd = CreateWindow(L"button", m_Lang.GetLanguageW(L"DIALOG", L"Dlg2").c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			10, 30, 200, 20, hWnd, (HMENU)ID_CHK2, hInst, NULL);
		c3_hWnd = CreateWindow(L"button", m_Lang.GetLanguageW(L"DIALOG", L"Dlg3").c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			10, 50, 200, 20, hWnd, (HMENU)ID_CHK3, hInst, NULL);
		c4_hWnd = CreateWindow(L"button", m_Lang.GetLanguageW(L"DIALOG", L"Dlg4").c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			10, 70, 200, 20, hWnd, (HMENU)ID_CHK4, hInst, NULL);
		c5_hWnd = CreateWindow(L"button", m_Lang.GetLanguageW(L"DIALOG", L"Dlg5").c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			10, 90, 200, 20, hWnd, (HMENU)ID_CHK5, hInst, NULL);
		c6_hWnd = CreateWindow(L"button", m_Lang.GetLanguageW(L"DIALOG", L"Dlg5_1").c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			10, 110, 200, 20, hWnd, (HMENU)ID_CHK6, hInst, NULL);

		e1_hWnd = CreateWindow(L"edit", L"", WS_CHILD | WS_VISIBLE | BS_TEXT | WS_BORDER,
			10, 140, 200, 20, hWnd, (HMENU)0, hInst, NULL);
		e2_hWnd = CreateWindow(L"edit", L"", WS_CHILD | WS_VISIBLE | BS_TEXT | WS_BORDER | ES_PASSWORD,
			10, 160, 200, 20, hWnd, (HMENU)0, hInst, NULL);
		b3_hWnd = CreateWindow(L"button", m_Lang.GetLanguageW(L"DIALOG", L"Dlg6").c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			10, 180, 200, 20, hWnd, (HMENU)ID_TWEETAUTH, hInst, NULL);

		b1_hWnd = CreateWindow(L"button", m_Lang.GetLanguageW(L"DIALOG", L"Dlg7").c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			10, 210, 60, 20, hWnd, (HMENU)ID_OK, hInst, NULL);
		b2_hWnd = CreateWindow(L"button", m_Lang.GetLanguageW(L"DIALOG", L"Dlg8").c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			80, 210, 60, 20, hWnd, (HMENU)ID_CANCEL, hInst, NULL);
		b4_hWnd = CreateWindow(L"button", m_Lang.GetLanguageW(L"DIALOG", L"Dlg9").c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			160, 210, 40, 20, hWnd, (HMENU)ID_TWIT, hInst, NULL);

		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

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
			DestroyWindow(hWnd);
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
			{
				if (!c_dect->isCleared() && twit_clear) {
					MessageBox(m_hWnd, m_Lang.GetLanguageW(L"DIALOG", L"OnlyClearScreen").c_str(), L"", NULL);
					break;
				}
				if (!c_dect->isResultScreen()) {
					MessageBox(m_hWnd, m_Lang.GetLanguageW(L"DIALOG", L"OnlyResultScreen").c_str(), L"", NULL);
					break;
				}
				if (c_dect->isAutoPlaying()) {
					MessageBox(m_hWnd, m_Lang.GetLanguageW(L"DIALOG", L"NoAutoPlay").c_str(), L"", NULL);
					break;
				}
				TCHAR str[255];
				c_dect->getLR2StatusString(str);
				int r = MessageBox(m_hWnd, wstring(str).append(  m_Lang.GetLanguageW(L"DIALOG", L"TwitConfirm") ).c_str(), L"", MB_YESNO);
				if (r == IDYES) {
					if (twit_pic)
						//_beginthread(doTwitwithPic, 0, 0);
						doTwitwithPic(0);
					else
						doTwit(0);
				}
			}
			break;
		case ID_TWEETAUTH:
			{
				char _id[255], _pass[255];
				GetWindowTextA(e1_hWnd, _id, 255);
				GetWindowTextA(e2_hWnd, _pass, 255);
				if (c_twit->getToken(_id, _pass)) {
					MessageBox(m_hWnd, m_Lang.GetLanguageW(L"DIALOG", L"AuthSucceed").c_str(), L"", NULL);
				} else {
					MessageBox(m_hWnd, m_Lang.GetLanguageW(L"DIALOG", L"AuthFailed").c_str(), L"", NULL);
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
			
			// debug code
			swprintf(s, L"%d - %d/%d/%d/%d/%d", c_dect->LR2stat[LR_SCORE],
			c_dect->LR2stat[LR_PG], 
			c_dect->LR2stat[LR_GR], 
			c_dect->LR2stat[LR_GD], 
			c_dect->LR2stat[LR_PR], 
			c_dect->LR2stat[LR_BD] );
			SetRect(&rt, 10, 300, 200, 320);
			DrawText(hdc, s, lstrlen(s), &rt, DT_LEFT);
			
			swprintf(s, L"\"%s\"", c_dect->LR2BMSTitle);
			SetRect(&rt, 10, 320, 200, 340);
			DrawText(hdc, s, lstrlen(s), &rt, DT_LEFT);
			
			swprintf(s, L"GUAGE:%d", c_dect->LR2stat[LR_GUAGE]);
			SetRect(&rt, 10, 340, 200, 360);
			DrawText(hdc, s, lstrlen(s), &rt, DT_LEFT);
			
			if (twit_Enabled && !twit_Twitted) {
				lstrcpy(s, L"RESULT SCREEN - SUBMIT SCORE");
				SetRect(&rt, 10, 360, 200, 380);
				DrawText(hdc, s, lstrlen(s), &rt, DT_LEFT);
			}
			
			TCHAR s1[256], s2[256];
			lstrcpy(s1, c_dect->LR2BMSTitle);
			c_dect->getLevel(s2);
			SetRect(&rt, 10, 380, 200, 400);
			DrawText(hdc, s2, lstrlen(s2), &rt, DT_LEFT);

			EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		delTray();
		releaseKeyHook();
		g_DLL.Eject();
		m_log.endLog();
		releaseSharedMemory();
		removeMutex();
		UnhookWindowsHookEx(hHook);
		PostQuitMessage(0);
		break;
	case WM_TIMER:
		InvalidateRect(hWnd, NULL, TRUE);

		if (!c_dect->getLR2Status()) {
			changeTray(false);
			c_dect->detectLR2();
			b_detected = false;
			break;
		} else {
			if (!b_detected) {
				playAlarm();
				if (lr2_injectstartup && !g_DLL.isDLLInjected(NULL, "LR2 beta3 version 100201", L"LR2DLL.dll"))
					g_DLL.Inject(NULL, "LR2 beta3 version 100201", ".\\\\LR2DLL.dll");
				b_detected = true;

				alarmTray( m_Lang.GetLanguageW(L"HOOK", L"Tray")
					.append( m_Lang.GetLanguageW(L"HOOK", (lr2_injectstartup)?(L"Tray_InjectStartup"):(L"Tray_NoInjectStartup") ) ).c_str() );
				setMessage( m_Lang.GetLanguageA("HOOK", "Hook").c_str() );
			}
			changeTray(true);
		}

		/* when Encountered Result Screen - with conditions */
		if (c_dect->isResultScreen() && !twit_Enabled) {	
			// info message for non-auto twit
			if (twit_cancel || !twit_auto || (twit_auto && !c_dect->isCleared())) 
				if (opt4)
					setMessage( m_Lang.GetLanguageA("HOOK", "ResultScreen").c_str() );

			twit_Enabled = true;
			twit_Twitted = false;
		}
		/* when Result screen finished */
		if (!c_dect->isResultScreen() && twit_Enabled) {
			twit_cancel = false;
			twit_Enabled = false;
		}
		/* when Game screen Started */
		if (!lr2_game && c_dect->isPlaying()) {
			lr2_game = true;
			twit_cancel = false;	// 게임 도중에 캔슬 가능함

			// info message for canceling
			if (twit_auto && opt4)
				setMessage( m_Lang.GetLanguageA("HOOK", "AutoTwit").c_str() );
		}
		/* when Not Game screen */
		if (!c_dect->isPlaying()) {
			lr2_game = false;
		}

		/* Auto-Twitting Procedure */
		if (twit_auto && twit_Enabled && !twit_Twitted && !twit_cancel) {
			// check condition
			if (!(twit_high && !c_dect->isHighScore()) &&
			!(twit_clear && !c_dect->isCleared())) {
				if (twit_pic) {
					setMessage( m_Lang.GetLanguageA("HOOK", "ScreenShotWait").c_str() );
					int val = CAPTURE_DELAY;
					doTwitwithPic(&val);
					//_beginthread(doTwitwithPic, 0, &val);
				} else {
					doTwit(0);
				}

				twit_Twitted = true;
			}
		}

		break;
	case WM_LBUTTONUP:
		{
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

void __cdecl doTwit(void *) {
	// for cancel!
	if (c_dect->LR2stat[LR_SCORE] == 0) return;
	if (c_dect->isAutoPlaying()) return;

	TCHAR str[255];
	c_dect->getLR2StatusString(str);
	str[140] = L'\0';	// max 140 char
	m_log.writeLogLine(L"Twit", str);

	char* pTemp = NULL;
	int iLen = ::WideCharToMultiByte(CP_UTF8, 0, str, -1, pTemp, 0, NULL, NULL);
	pTemp = new char[iLen+1];
	::WideCharToMultiByte(CP_UTF8, 0, str, -1, pTemp, iLen, NULL, NULL);

	string astr = string(pTemp);
	delete [] pTemp;

	if (c_twit->sendTwit( astr )) {
		setMessage( m_Lang.GetLanguageA("HOOK", "NormalTwit").c_str() );
		playAlarm();
	}
}

void __cdecl doTwitwithPic(PVOID delaymilltime) {
	// for cancel!
	if (c_dect->LR2stat[LR_SCORE] == 0) return;
	if (c_dect->isAutoPlaying()) return;


	if (delaymilltime != 0) {
		DWORD sleeptime = CAPTURE_DELAY;
		Sleep(sleeptime);
	}

	// remove message
	setMessage("");

	TCHAR str[255];
	c_dect->getLR2StatusString(str);
	str[119] = L'\0';	// max 140-21 = 119 char
	m_log.writeLogLine(L"TwitPic", str);

	char* pTemp = NULL;
	int iLen = ::WideCharToMultiByte(CP_UTF8, 0, str, -1, pTemp, 0, NULL, NULL);
	pTemp = new char[iLen+1];
	::WideCharToMultiByte(CP_UTF8, 0, str, -1, pTemp, iLen, NULL, NULL);
	
	Twitpic m_tpic;
	m_tpic.set_account(c_twit->customerKey, c_twit->customerSecret,
		c_twit->accessToken, c_twit->accessTokenSecret);
	if (m_tpic.CaptureScreen())	// wait till finish
	{		// remove after update
		setMessage( m_Lang.GetLanguageA("HOOK", "PicTwitWait").c_str() );
		string res = m_tpic.upload_pic( m_tpic.LR2PicPath, pTemp );
		remove(m_tpic.LR2PicPath);
		
		setMessage( m_Lang.GetLanguageA("HOOK", "PicTwit").c_str() );
		m_log.writeLogLine( res.c_str() );
		playAlarm();
	} else {
		m_log.writeLogLine( "Failed to capture screen." );
	}
	
	delete [] pTemp;
}

void doTray() {
	NOTIFYICONDATA  nid;
    nid.cbSize = sizeof(nid);
    nid.hWnd = m_hWnd; // 메인 윈도우 핸들
    nid.uID = ID_NOTIFY;  // 아이콘 리소스 ID
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP; // 플래그 설정
    nid.hIcon = LoadIcon( hInst, MAKEINTRESOURCE(IDI_LR2_E) ); // 아이콘 로드 
	nid.uCallbackMessage = ID_NOTIFY_CLICK;

    TCHAR strTitle[256];
    lstrcpy(nid.szTip, L"LR2Twit"); 
    Shell_NotifyIcon(NIM_ADD, &nid);
    SendMessage(m_hWnd, WM_SETICON, (WPARAM)TRUE, (LPARAM)nid.hIcon);
}

void alarmTray(const TCHAR *alarmMsg) {
	NOTIFYICONDATA  nid;
    nid.cbSize = NOTIFYICONDATA_V2_SIZE;
	nid.hWnd = m_hWnd;
    nid.uID = ID_NOTIFY;
    nid.uFlags = NIF_INFO | NIF_MESSAGE;
	nid.dwInfoFlags = NIIF_INFO;
	lstrcpy(nid.szInfoTitle, L"LR2Twit");
	lstrcpy(nid.szInfo, alarmMsg);
    Shell_NotifyIcon(NIM_MODIFY, &nid);
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
	opt4 = (SendMessage(c4_hWnd,BM_GETCHECK,0,0) == BST_CHECKED);
	opt5 = (SendMessage(c5_hWnd,BM_GETCHECK,0,0) == BST_CHECKED);
	opt6 = (SendMessage(c6_hWnd,BM_GETCHECK,0,0) == BST_CHECKED);

	twit_auto = opt1;
	twit_clear = opt2;
	twit_high = opt3;
	twit_pic = opt5;


	TCHAR _s[255];
	_itow(opt1, _s, 10);
	WritePrivateProfileStringW(L"LR2TWIT", L"LR2_AUTOTWIT", _s, L".\\settings.ini" );
	_itow(opt2, _s, 10);
	WritePrivateProfileStringW(L"LR2TWIT", L"TWIT_CLEAR", _s, L".\\settings.ini" );
	_itow(opt3, _s, 10);
	WritePrivateProfileStringW(L"LR2TWIT", L"TWIT_HIGHSCORE", _s, L".\\settings.ini" );
	_itow(opt4, _s, 10);
	WritePrivateProfileStringW(L"LR2TWIT", L"TWIT_SHORTCUTKEY", _s, L".\\settings.ini" );
	_itow(opt5, _s, 10);
	WritePrivateProfileStringW(L"LR2TWIT", L"TWIT_WITHPIC", _s, L".\\settings.ini" );
	_itow(opt6, _s, 10);
	WritePrivateProfileStringW(L"LR2TWIT", L"TWIT_LR2TAG", _s, L".\\settings.ini" );
}

void loadSettings() {
	TCHAR _s[255];

	GetPrivateProfileStringW(L"LR2TWIT", L"LR2_TITLE", L"LR2 beta3 version 100201", _s, 255, L".\\settings.ini");
	lstrcpy(opt_lr2title, _s);
	GetPrivateProfileStringW(L"LR2TWIT", L"LR2_AUTOTWIT", L"1", _s, 255, L".\\settings.ini");
	opt1 = _wtoi(_s);
	GetPrivateProfileStringW(L"LR2TWIT", L"LR2_REMOVEAUTOLIMIT", L"0", _s, 255, L".\\settings.ini");
	opt7 = _wtoi(_s);
	GetPrivateProfileStringW(L"LR2TWIT", L"TWIT_MESSAGE", L"《[TITLE] ([DIFF])》 [GUAGE] [RESULT]♬ - [RATE]%([RANK]), [MC]/[NC] Combo, [PR]+[BD] BP♬", _s, 255, L".\\settings.ini");
	lstrcpy(opt_message, _s);
	GetPrivateProfileStringW(L"LR2TWIT", L"TWIT_ENCODING", L"SHIFT_JIS", _s, 255, L".\\settings.ini");
	wcstombs(opt_encode, _s, 255);
	GetPrivateProfileStringW(L"LR2TWIT", L"TWIT_DLLAUTOINJECTATSTARTUP", L"1", _s, 255, L".\\settings.ini");
	lr2_injectstartup = (wcscmp(_s, L"1") == 0);
	GetPrivateProfileStringW(L"LR2TWIT", L"TWIT_CLEAR", L"1", _s, 255, L".\\settings.ini");
	opt2 = _wtoi(_s);
	GetPrivateProfileStringW(L"LR2TWIT", L"TWIT_HIGHSCORE", L"1", _s, 255, L".\\settings.ini");
	opt3 = _wtoi(_s);
	GetPrivateProfileStringW(L"LR2TWIT", L"TWIT_SHORTCUTKEY", L"1", _s, 255, L".\\settings.ini");
	opt4 = _wtoi(_s);
	GetPrivateProfileStringW(L"LR2TWIT", L"TWIT_WITHPIC", L"1", _s, 255, L".\\settings.ini");
	opt5 = _wtoi(_s);
	GetPrivateProfileStringW(L"LR2TWIT", L"TWIT_LR2TAG", L"1", _s, 255, L".\\settings.ini");
	opt6 = _wtoi(_s);

	twit_auto = opt1;
	twit_clear = opt2;
	twit_high = opt3;
	twit_pic = opt5;
}

void setSettings() {
	SendMessage(c1_hWnd,BM_SETCHECK,opt1?BST_CHECKED:BST_UNCHECKED,0);
	SendMessage(c2_hWnd,BM_SETCHECK,opt2?BST_CHECKED:BST_UNCHECKED,0);
	SendMessage(c3_hWnd,BM_SETCHECK,opt3?BST_CHECKED:BST_UNCHECKED,0);
	SendMessage(c4_hWnd,BM_SETCHECK,opt4?BST_CHECKED:BST_UNCHECKED,0);
	SendMessage(c5_hWnd,BM_SETCHECK,opt5?BST_CHECKED:BST_UNCHECKED,0);
	SendMessage(c6_hWnd,BM_SETCHECK,opt6?BST_CHECKED:BST_UNCHECKED,0);
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

// shared memory
BOOL setSharedMemory() {
	hmm_LR2Message = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 256, L"LR2Message");
	if (!hmm_LR2Message) {
		return FALSE;
	}
	p_LR2Message = (char*)MapViewOfFile(hmm_LR2Message, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (!p_LR2Message) {
		return FALSE;
	}

	return TRUE;
}

BOOL releaseSharedMemory() {
	UnmapViewOfFile(p_LR2Message);
	CloseHandle(hmm_LR2Message);

	return TRUE;
}

// auto release는 나중에 구현
int g_Message_key = 10;
void setMessage(const char *str) {
	g_Message_key ++;
	if (g_Message_key>255) g_Message_key = 0;
	p_LR2Message[0] = (BYTE)g_Message_key;
	strcpy(p_LR2Message+1, str);
}

void startKeyHook() {
   hHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)HookProc,
	   GetModuleHandle(NULL), NULL);
   if (hHook) {
	   m_log.writeLogLine(L"Successfully Hooked - WH_KEYBOARD_LL");
   } else {
	   m_log.writeLogLine(L"Hooking Failed - WH_KEYBOARD_LL");
   }
}

void releaseKeyHook() {
	UnhookWindowsHookEx(hHook);
}