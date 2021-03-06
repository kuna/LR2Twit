// LR2Twit.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "LR2Twit.h"
#include "DLLInjector.h"
#include "lang.h"

#include <shellapi.h>	/*shell icon*/
#include <process.h>	/* thread */

#include <io.h>
#include <mmsystem.h>
#pragma comment (lib,"winmm.lib")

#define MAX_LOADSTRING 100
#define CAPTURE_DELAY 3500
//#define DEBUG_LR2TWIT

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK msgDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

// language
LanguageSetting m_Lang;

// DLL Injector
DLLInjector g_DLL;

// Twitpic processor
Twitpic m_tpic;

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
	

   hInst = hInstance; // Store instance handle in our global variable

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
			if (IsDialogMessage(m_hWnd, &msg)) {
			} else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
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

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX ^ WS_MINIMIZEBOX,
      CW_USEDEFAULT, CW_USEDEFAULT, 240, 370, NULL, NULL, hInstance, NULL);
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
HBITMAP hBitmap;	// logo bitmap
HWND msgDlghWnd;
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
		{
			// for dialog-like font
			LOGFONT lf;
			GetObject (GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf); 
			HFONT hFont = CreateFont (lf.lfHeight, lf.lfWidth, 
				lf.lfEscapement, lf.lfOrientation, lf.lfWeight, 
				lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet, 
				lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality, 
				lf.lfPitchAndFamily, lf.lfFaceName); 

#define COMMON_CHKBOX (WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | WS_TABSTOP)
#define COMMON_EDIT (WS_CHILD | WS_VISIBLE | BS_TEXT | WS_BORDER | WS_TABSTOP)
#define COMMON_BUTTON (WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP)
#define COMMON_WIDTH 210

			c1_hWnd = CreateWindow(L"button", m_Lang.GetLanguageW(L"DIALOG", L"Dlg1").c_str(), COMMON_CHKBOX,
				10, 90, COMMON_WIDTH, 20, hWnd, (HMENU)ID_CHK1, hInst, NULL);
			c2_hWnd = CreateWindow(L"button", m_Lang.GetLanguageW(L"DIALOG", L"Dlg2").c_str(), COMMON_CHKBOX,
				10, 110, COMMON_WIDTH, 20, hWnd, (HMENU)ID_CHK2, hInst, NULL);
			c3_hWnd = CreateWindow(L"button", m_Lang.GetLanguageW(L"DIALOG", L"Dlg3").c_str(), COMMON_CHKBOX,
				10, 130, COMMON_WIDTH, 20, hWnd, (HMENU)ID_CHK3, hInst, NULL);
			c4_hWnd = CreateWindow(L"button", m_Lang.GetLanguageW(L"DIALOG", L"Dlg4").c_str(), COMMON_CHKBOX,
				10, 150, COMMON_WIDTH, 20, hWnd, (HMENU)ID_CHK4, hInst, NULL);
			c5_hWnd = CreateWindow(L"button", m_Lang.GetLanguageW(L"DIALOG", L"Dlg5").c_str(), COMMON_CHKBOX,
				10, 170, COMMON_WIDTH, 20, hWnd, (HMENU)ID_CHK5, hInst, NULL);
			c6_hWnd = CreateWindow(L"button", m_Lang.GetLanguageW(L"DIALOG", L"Dlg5_1").c_str(), COMMON_CHKBOX,
				10, 190, COMMON_WIDTH, 20, hWnd, (HMENU)ID_CHK6, hInst, NULL);

			e1_hWnd = CreateWindow(L"edit", L"", COMMON_EDIT,
				70, 220, COMMON_WIDTH-60, 18, hWnd, (HMENU)0, hInst, NULL);
			e2_hWnd = CreateWindow(L"edit", L"", COMMON_EDIT | ES_PASSWORD,
				70, 240, COMMON_WIDTH-60, 18, hWnd, (HMENU)0, hInst, NULL);
			b3_hWnd = CreateWindow(L"button", m_Lang.GetLanguageW(L"DIALOG", L"Dlg6").c_str(), COMMON_BUTTON,
				10, 260, COMMON_WIDTH, 20, hWnd, (HMENU)ID_TWEETAUTH, hInst, NULL);
			
			b5_hWnd = CreateWindow(L"button", m_Lang.GetLanguageW(L"DIALOG", L"Dlg10").c_str(), COMMON_BUTTON,
				10, 285, COMMON_WIDTH, 20, hWnd, (HMENU)ID_EDITWND, hInst, NULL);
			b1_hWnd = CreateWindow(L"button", m_Lang.GetLanguageW(L"DIALOG", L"Dlg7").c_str(), COMMON_BUTTON,
				10, 310, 60, 20, hWnd, (HMENU)ID_OK, hInst, NULL);
			b2_hWnd = CreateWindow(L"button", m_Lang.GetLanguageW(L"DIALOG", L"Dlg8").c_str(), COMMON_BUTTON,
				80, 310, 60, 20, hWnd, (HMENU)ID_CANCEL, hInst, NULL);
			b4_hWnd = CreateWindow(L"button", m_Lang.GetLanguageW(L"DIALOG", L"Dlg9").c_str(), COMMON_BUTTON,
				160, 310, 60, 20, hWnd, (HMENU)ID_TWIT, hInst, NULL);

			// labels
#define COMMON_LABEL (WS_CHILD | WS_VISIBLE)
			HWND lbl1 = CreateWindow(L"static", L"by @kuna_kr - " VERSIONSTRING, COMMON_LABEL,
				80, 65, 160, 20, hWnd, (HMENU)NULL, hInst, NULL);
			EnableWindow(lbl1, FALSE);
			HWND lbl2 = CreateWindow(L"static", L"ID", COMMON_LABEL,
				10, 220, 60, 20, hWnd, (HMENU)NULL, hInst, NULL);
			HWND lbl3 = CreateWindow(L"static", L"Password", COMMON_LABEL,
				10, 240, 60, 20, hWnd, (HMENU)NULL, hInst, NULL);

			// font change
			SendMessage (c1_hWnd, WM_SETFONT, (WPARAM)hFont, TRUE);
			SendMessage (c2_hWnd, WM_SETFONT, (WPARAM)hFont, TRUE);
			SendMessage (c3_hWnd, WM_SETFONT, (WPARAM)hFont, TRUE);
			SendMessage (c4_hWnd, WM_SETFONT, (WPARAM)hFont, TRUE);
			SendMessage (c5_hWnd, WM_SETFONT, (WPARAM)hFont, TRUE);
			SendMessage (c6_hWnd, WM_SETFONT, (WPARAM)hFont, TRUE);
			SendMessage (e1_hWnd, WM_SETFONT, (WPARAM)hFont, TRUE);
			SendMessage (e2_hWnd, WM_SETFONT, (WPARAM)hFont, TRUE);
			SendMessage (b1_hWnd, WM_SETFONT, (WPARAM)hFont, TRUE);
			SendMessage (b2_hWnd, WM_SETFONT, (WPARAM)hFont, TRUE);
			SendMessage (b3_hWnd, WM_SETFONT, (WPARAM)hFont, TRUE);
			SendMessage (b4_hWnd, WM_SETFONT, (WPARAM)hFont, TRUE);
			SendMessage (b5_hWnd, WM_SETFONT, (WPARAM)hFont, TRUE);
			SendMessage (lbl1, WM_SETFONT, (WPARAM)hFont, TRUE);
			SendMessage (lbl2, WM_SETFONT, (WPARAM)hFont, TRUE);
			SendMessage (lbl3, WM_SETFONT, (WPARAM)hFont, TRUE);
						
			// load logo from res
			hBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_LOGO));
			
			// make message edit window
			msgDlghWnd = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_MESSAGE),
				hWnd, msgDlgProc);

			break;
		}

	case WM_CTLCOLORSTATIC:	// for transparent ctl
		{
			HDC hdcStatic = (HDC) wParam; 
			SetTextColor(hdcStatic, RGB(0,0,0));    
			SetBkMode (hdcStatic, TRANSPARENT);

			return (LRESULT)GetStockObject(NULL_BRUSH);
		}
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
						doTwitwithPic(0);
					else
						doTwit(0);
				}
			}
			break;
		case ID_EDITWND:
			SetWindowText(msgDlghWnd, m_Lang.GetLanguageW(L"DIALOG", L"Dlg10").c_str());
			SetWindowText(GetDlgItem(msgDlghWnd, IDC_PREVIEW), L"");
			SetWindowText(GetDlgItem(msgDlghWnd, IDC_MSG), c_dect->getLR2FormatStr());

			ShowWindow(msgDlghWnd, SW_SHOW);
			break;
		case ID_TWEETAUTH:
			{
				char _id[255], _pass[255];
				GetWindowTextA(e1_hWnd, _id, 255);
				GetWindowTextA(e2_hWnd, _pass, 255);
				if (strlen(_id) == 0 || strlen(_pass) == 0) {
					MessageBox(m_hWnd, L"ID와 패스워드를 입력해 주셔야 합니다", L"LR2Twit", MB_ICONEXCLAMATION);
				} else {
					if (c_twit->getToken(_id, _pass)) {
						MessageBox(m_hWnd, m_Lang.GetLanguageW(L"DIALOG", L"AuthSucceed").c_str(), L"LR2Twit", MB_ICONINFORMATION);
					} else {
						MessageBox(m_hWnd, m_Lang.GetLanguageW(L"DIALOG", L"AuthFailed").c_str(), L"LR2Twit", MB_ICONEXCLAMATION);
					}
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
			
			// print logo
			HDC hdcMem = CreateCompatibleDC(hdc);
			SelectObject(hdcMem, hBitmap);
			BitBlt(hdc, 10, 10, 224, 64, hdcMem, 0, 0, SRCCOPY);
			DeleteDC(hdcMem);
			
			// debug code
#ifdef DEBUG_LR2TWIT
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
#endif

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
#ifdef DEBUG_LR2TWIT
		InvalidateRect(hWnd, NULL, TRUE);
#endif

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
	case WM_MOUSEMOVE:
		{
			int x=(int)(short)LOWORD(lParam);
			int y=(int)(short)HIWORD(lParam);

			if (y < 80) {
				SetCursor( LoadCursor(0 , IDC_HAND) );
			} else {
				SetCursor( LoadCursor(0 , IDC_ARROW) );
			}

			break;
		}

	case WM_LBUTTONDOWN:
		{
			int x=(int)(short)LOWORD(lParam);
			int y=(int)(short)HIWORD(lParam);

			if (y < 80) {
				// go to github page to see new version
				ShellExecuteA(NULL, "Open", "https://github.com/kuna/LR2Twit", NULL, NULL, SW_SHOW);
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

BOOL CALLBACK msgDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch(Message)
    {
        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case IDOK:
					{
						TCHAR msg[256];
						GetWindowText(GetDlgItem(hwnd, IDC_MSG), msg, 256);
						c_dect->setLR2FormatStr(msg);
						saveSettings();
						ShowWindow(hwnd, SW_HIDE);
						break;
					}
				case IDCANCEL:
					ShowWindow(hwnd, SW_HIDE);
					break;
				case IDC_MSG:
					if (HIWORD(wParam) == EN_CHANGE) {
						TCHAR msg[256];
						GetWindowText(GetDlgItem(hwnd, IDC_MSG), msg, 256);
						
						// replace example
						c_dect->replace_str(msg, L"[RANK]", L"AAA");
						c_dect->replace_str(msg, L"[GUAGE]", L"HARD");
						c_dect->replace_str(msg, L"[RESULT]", L"CLEAR");
						c_dect->replace_str(msg, L"[MAINTITLE]", L"Angelic Layer");
						c_dect->replace_str(msg, L"[SUBTITLE]", L"-ANOTHER-");
						c_dect->replace_str(msg, L"[TITLE]", L"Angelic Layer -ANOTHER-");
						c_dect->replace_str(msg, L"[ARTIST]", L"Shiki");
						c_dect->replace_str(msg, L"[GENRE]", L"Trance");
						c_dect->replace_str(msg, L"[EXS]", L"678");
						c_dect->replace_str(msg, L"[EXHS]", L"3146");
						c_dect->replace_str(msg, L"[EXMS]", L"4176");
						c_dect->replace_str(msg, L"[PG]", L"900");
						c_dect->replace_str(msg, L"[GR]", L"800");
						c_dect->replace_str(msg, L"[GD]", L"700");
						c_dect->replace_str(msg, L"[BD]", L"600");
						c_dect->replace_str(msg, L"[PR]", L"500");
						c_dect->replace_str(msg, L"[NC]", L"400");
						c_dect->replace_str(msg, L"[MC]", L"300");
						c_dect->replace_str(msg, L"[DIFF]", L"★4");
						c_dect->replace_str(msg, L"[SCORE]", L"171819");
						c_dect->replace_str(msg, L"[IRTOT]", L"4000");
						c_dect->replace_str(msg, L"[IRNOW]", L"1300");
						c_dect->replace_str(msg, L"[KEY]", L"7");
						c_dect->replace_str(msg, L"[AUTO]", L"(AUTO-SCR)");
						c_dect->replace_str(msg, L"[RATE]", L"88.99");

						SetWindowText(GetDlgItem(hwnd, IDC_PREVIEW), msg);
					}
					break;
            }
			break;
		case WM_CLOSE:
			ShowWindow(hwnd, SW_HIDE);
			break;
        default:
            return FALSE;
    }
    return TRUE;
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
	} else {
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
	str[117] = L'\0';	// max 140-21 = 119 char
	m_log.writeLogLine(L"TwitPic", str);

	std::string ret;
    int len = WideCharToMultiByte(CP_UTF8, 0, str, wcslen(str), NULL, 0, NULL, NULL);
    if (len > 0)
    {
		char *t = new char[len+2];
		WideCharToMultiByte(CP_UTF8, 0, str, wcslen(str), t, len, NULL, NULL);
		t[len] = 0;
		ret = string(t);
		delete [] t;
    }
	
	if (m_tpic.CaptureScreen())	// wait till finish
	{		
		// remove after update
		setMessage( m_Lang.GetLanguageA("HOOK", "PicTwitWait").c_str() );
		char *data;
		int len;
		//m_tpic.getRawData( m_tpic.LR2PicPath, data, &len );
		m_tpic.getRawData( m_tpic.LR2PicPath, &data, &len );
		if (len <= 0) {
			m_log.writeLogLine( "Screen tweet failed. (no Captured file)" );
		} else {
			if (c_twit->sendMediaTwit( ret, data, len )) {
				setMessage( m_Lang.GetLanguageA("HOOK", "PicTwit").c_str() );
				playAlarm();
			} else {
				m_log.writeLogLine( "Screen tweet failed." );
			}
		}
		free(data);
	} else {
		m_log.writeLogLine( "Failed to capture screen." );
	}

	remove(m_tpic.LR2PicPath);
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
	WritePrivateProfileStringW(L"LR2TWIT", L"TWIT_MESSAGE", c_dect->getLR2FormatStr(), L".\\settings.ini");
}

void loadSettings() {
	TCHAR _s[255];

	GetPrivateProfileStringW(L"LR2TWIT", L"LR2_TITLE", L"LR2 beta3 version 100201", _s, 255, L".\\settings.ini");
	lstrcpy(opt_lr2title, _s);
	GetPrivateProfileStringW(L"LR2TWIT", L"LR2_AUTOTWIT", L"1", _s, 255, L".\\settings.ini");
	opt1 = _wtoi(_s);
	GetPrivateProfileStringW(L"LR2TWIT", L"LR2_REMOVEAUTOLIMIT", L"0", _s, 255, L".\\settings.ini");
	opt7 = _wtoi(_s);
	GetPrivateProfileStringW(L"LR2TWIT", L"TWIT_MESSAGE", L"《[TITLE] ([DIFF])》 [GUAGE] [RESULT] - [RATE]%([RANK]), [MC]/[NC] Combo, [PR]+[BD] BP", _s, 255, L".\\settings.ini");
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