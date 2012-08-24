#include <algorithm>
using namespace std;
#include <map>

#include "iconv.h"
#pragma comment(lib, "iconv.lib")
#include "Log.h"

//#define _BIT (0x00000040)	// just for 64bit
// 0x00060000 - windows 7 64bit
// 0x00000040 - windows xp

#define LR_SCORE (0x00127294) // 001873AC - PARTNER
#define LR_PG (0x001271C0)
#define LR_GR (0x001271BC)
#define LR_GD (0x001271B8)
#define LR_BD (0x001271B4)
#define LR_PR (0x001271B0)
#define LR_EXS (0x00127280)
#define LR_EXHS (0x0012F490)
#define LR_NC (0x00127278) // note count - 2D200
#define LR_MC (0x001271C4) // max combo
#define LR_MAINTITLE (0x000964D0)	// addr pointer
#define LR_SUBTITLE (0x000964D4)	// addr pointer
#define LR_TITLE (0x000B1760)	// addr pointer
#define LR_ARTIST (0x000964D8)	// addr pointer
#define LR_GENRE (0x000B1764)	// addr pointer
#define LR_DIFF (0x00096518)
#define LR_GUAGE (0x0008F840)
#define LR_GUAGENUM (0x00127200)
#define LR_AUTO (0x00096668)
#define LR_AUTOSCR (0x0008F870)
#define LR_IRTOT (0x0012FB10)
#define LR_IRNOW (0x0012FB54)
#define LR_MODE (0x00096510)

#define LR_ISMENU (0x00099C5C)
#define LR_ISPLAYING (0x000986EC)	// DF38, D43C, D434
#define LR_ISRESULT (0x0000A19F4)	// 001019B8, 001019F4, F8704

class Detector {
private:
	TCHAR
		LR2WinTitle[255],
		LR2FormatStr[255];

	HWND LR2hWnd;
	HANDLE LR2h;

	HMODULE app_hmoudle;
	size_t baseAddr;
	int baseOffset;

	void replace_str(TCHAR *org, TCHAR *find, int n);
	void replace_str(TCHAR *org, TCHAR *find, TCHAR *n);

	int _OFFSET;
	void OSDetect();
	Log *l;

	bool IsIIDXBMS(TCHAR *title);
public:
	Detector(Log *_l);

	DWORD LR2pid;
	std::map<int, int> LR2stat;
	TCHAR LR2BMSMainTitle[255];
	TCHAR LR2BMSSubTitle[255];
	TCHAR LR2BMSTitle[255];
	TCHAR LR2BMSArtist[255];
	TCHAR LR2BMSGenre[255];
	double LR2Guage;

	int getMemValInt(LPVOID addr);
	bool detectLR2();
	bool getLR2Status();
	void getLR2StatusString(TCHAR *str);

	bool isHighScore();
	bool isCleared();
	bool isResultScreen();
	bool isAutoPlaying();
	bool isPlaying();
	bool isMenu();

	bool isLR2Vaild();
	void setRecordAlways();
	void checkDiffLevel(TCHAR *title, TCHAR *diff, int totalNotCnt=0, int keymode=7);
};

/* formatString */


BOOL match(TCHAR *fname, TCHAR *filter);