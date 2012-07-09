#pragma once

#include "resource.h"
#include "Prog_Detector.h"
#include "TwitProc.h"
#include "Log.h"

#define ID_NOTIFY 1000
#define ID_NOTIFY_CLICK WM_USER+1

#define ID_OK 1001
#define ID_CANCEL 1002
#define ID_TWEETAUTH 1003
#define ID_TWIT 1004
#define ID_CHK1	1005
#define ID_CHK2	1006
#define ID_CHK3	1007
#define ID_CHK4	1008


Detector *c_dect;
TwitProc *c_twit;
bool twit_Enabled;
bool twit_Twitted;

void doTwit();
void doTray();	// http://swain.egloos.com/2385537
void changeTray(bool detect);
void delTray();
void saveSettings();
void loadSettings();
void setSettings();
// LR2.exe�� ������������ ������ �����ϴ� ��� �߰�?
HANDLE m_hMutex;
bool checkMutex();
void removeMutex();

HWND m_hWnd;
bool twit_auto = true;
bool twit_clear = true;
bool twit_high = true;

/*********************
Child Windows
*********************/
HWND c1_hWnd;
HWND c2_hWnd;
HWND c3_hWnd;
HWND c4_hWnd;

HWND e1_hWnd;	// id
HWND e2_hWnd;	// pass

HWND b1_hWnd;	// ok
HWND b2_hWnd;	// cancel
HWND b3_hWnd;	// get new twitter auth
HWND b4_hWnd;	// write

// ���������� ini ���Ϸ�!

Log m_log;

/*********************
LR2IR �ε��ؼ� ������ �����ִ� ��� �߰��ϱ�
������ŷ �߰��ϱ� (Ű����� Ʈ�� ����)
DirectX OverLay �߰��ϱ�
ruv-it, TR2 �� �����ϱ�

- �ߺ����� ������Ű�� (���ؽ�?)
- �����÷��� �����ϱ�
- �ɼ� �����
�޽��� Ŀ���͸���¡
*********************/