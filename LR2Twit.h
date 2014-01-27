#pragma once

#include "resource.h"
#include "Prog_Detector.h"
#include "TwitProc.h"
#include "Twitpic.h"
#include "Log.h"

#define ID_NOTIFY 10000
#define ID_NOTIFY_CLICK WM_USER+1

#define ID_OK 10001
#define ID_CANCEL 10002
#define ID_TWEETAUTH 10003
#define ID_TWIT 10004
#define ID_EDITWND 10011
#define ID_CHK1	10005
#define ID_CHK2	10006
#define ID_CHK3	10007
#define ID_CHK4	10008
#define ID_CHK5	10009
#define ID_CHK6 10010


Detector *c_dect;
TwitProc *c_twit;
bool twit_Enabled;
bool twit_Twitted;

void startKeyHook();
void releaseKeyHook();

void __cdecl doTwit(PVOID pData);
void __cdecl doTwitwithPic(PVOID delaymilltime);
void doTray();	// http://swain.egloos.com/2385537
void alarmTray( const TCHAR *alarmMsg);
void changeTray(bool detect);
void delTray();
void saveSettings();
void loadSettings();
void setSettings();
HANDLE m_hMutex;
bool checkMutex();
void removeMutex();

// for shared memory
HANDLE hmm_LR2Message;
char *p_LR2Message;
BOOL setSharedMemory();
BOOL releaseSharedMemory();
void setMessage( const char *str);

HWND m_hWnd;
bool lr2_injectstartup = true;
bool lr2_game = false;
bool twit_cancel = false;

bool twit_auto = true;
bool twit_clear = true;
bool twit_high = true;
bool twit_pic = true;

/*********************
Child Windows
*********************/
HWND c1_hWnd;
HWND c2_hWnd;
HWND c3_hWnd;
HWND c4_hWnd;
HWND c5_hWnd;
HWND c6_hWnd;

HWND e1_hWnd;	// id
HWND e2_hWnd;	// pass

HWND b1_hWnd;	// ok
HWND b2_hWnd;	// cancel
HWND b3_hWnd;	// get new twitter auth
HWND b4_hWnd;	// write
HWND b5_hWnd;	// editwnd

// 수동수정은 ini 파일로!

Log m_log;

/*********************
LR2IR 로드해서 데이터 보여주는 기능 추가하기
전역후킹 추가하기 (키보드로 트윗 쓰기) (o)
DirectX OverLay 추가하기 (o)
ruv-it, TR2 등 지원하기

- 오토플레이 방지하기
메시지 커스터마이징
*********************/