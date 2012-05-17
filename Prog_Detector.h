#include <algorithm>
using namespace std;
#include <map>

#include "iconv.h"
#pragma comment(lib, "iconv.lib")

#define LR_SCORE 0x00127294
#define LR_PG 0x001271C0
#define LR_GR 0x001271BC
#define LR_GD 0x001271B8
#define LR_BD 0x001271B4
#define LR_PR 0x001271B0
#define LR_EXS 0x00127280
#define LR_EXHS 0x0012F490
#define LR_MC 0x0012F4B0
#define LR_TITLE 0x000964D0	// addr pointer
#define LR_DIFF 0x00096518
#define LR_GUAGE 0x0008F840
#define LR_STAT 0x0008C3FC
#define LR_GUAGENUM 0x00127200
#define LR_AUTO 0x00096668

class Detector {
private:
	TCHAR
		LR2WinTitle[255],
		LR2FormatStr[255];

	HWND LR2hWnd;
	DWORD LR2pid;
	HANDLE LR2h;

	wstring getInt2Str(int i); 
	void replace_str(TCHAR *org, TCHAR *find, TCHAR *n);
public:
	Detector();

	std::map<int, int> LR2stat;
	TCHAR LR2BMSTitle[255];
	double LR2Guage;

	int getMemValInt(LPVOID addr);
	bool detectLR2();
	bool getLR2Status();
	void getLR2StatusString(TCHAR *str);

	bool isHighScore();
	bool isCleared();
	bool isResultScreen();
	bool isAutoPlaying();
};

/* formatString */