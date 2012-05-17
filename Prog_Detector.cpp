#include "stdafx.h"
#include "Prog_Detector.h"

/*
bool eucjp_to_cp949(const string& in, wstring& out)
{
	iconv_t cd;
	bool ret = false;
	const char* pszIn;
	char* pszOut, *pos;
	size_t inLen, outLen;
	pszOut=NULL;
	
	cd = iconv_open("UCS-2","SHIFT_JIS");
	if(cd == (iconv_t)(-1))
	{
		OutputDebugString(L"error!\n");
		return false; 
	}
	
	inLen = in.length();
	pszIn = in.c_str();
	
	outLen = (inLen+1) * 2;
	pos = pszOut = (char*)calloc(outLen, sizeof(char));
	if(!pszOut) {
		OutputDebugString(L"error - no size\n");
		goto clean;
	}
	
	if(iconv(cd, &pszIn, &inLen, &pos, &outLen)==-1) {
		OutputDebugString(L"error - convert fail\n");
		goto clean;
	}
	out = wstring((TCHAR*)pszOut);
	ret = true;
clean:
	iconv_close(cd);
	if(pszOut) free(pszOut);
	return ret;
}*/

bool eucjp_to_cp949(const string& in, string& out)
{
	iconv_t cd;
	bool ret = false;
	const char* pszIn;
	char* pszOut, *pos;
	size_t inLen, outLen;
	pszOut=NULL;
	
	cd = iconv_open("EUC-KR","CP932");
	if(cd == (iconv_t)(-1))
	{
		OutputDebugString(L"error!\n");
		return false; 
	}
	
	inLen = in.length();
	pszIn = in.c_str();
	
	outLen = (inLen+1) * 2;
	pos = pszOut = (char*)calloc(outLen, sizeof(char));
	if(!pszOut) {
		OutputDebugString(L"error - no size\n");
		goto clean;
	}
	
	if(iconv(cd, &pszIn, &inLen, &pos, &outLen)==-1) {
		OutputDebugString(L"error - convert fail\n");
		goto clean;
	}
	out = pszOut;
	ret = true;
clean:
	iconv_close(cd);
	if(pszOut) free(pszOut);
	return ret;
}

Detector::Detector() {
	lstrcpy(LR2WinTitle, L"LR2 beta3 version 100201");
	lstrcpy(LR2FormatStr, L"\"%s\"곡을 %s 게이지로(%d) %d점 %s하였습니다. - %.2f%%(%s) - %d/%d/%d/%d/%d, MC:%d #BMS #LR");

	memset(LR2BMSTitle, 0, sizeof(LR2BMSTitle));

/*	LR2mm["Score"] = 0x00127294;
	LR2mm["PGr"] = 0x0012F48C;
	LR2mm["Gr"] = 0x0012F488;
	LR2mm["Gd"] = 0x0012F484;
	LR2mm["Bd"] = 0x0012F480;
	LR2mm["Pr"] = 0x0012F47C;
	LR2mm["EXScore"] = 0x00127280;
	LR2mm["EXHighScore"] = 0x0012F490;
	LR2mm["MaxCombo"] = 0x0012F4B0;
	LR2mm["Title"] = 0x0193FFB8;
	LR2mm["Diff"] = 0x00096518;
	LR2mm["Guage"] = 0x0008F840;*/
}

bool Detector::detectLR2() {
	LR2hWnd = FindWindow(LR2WinTitle, LR2WinTitle);
	if (!LR2hWnd) return false;

	DWORD TID = GetWindowThreadProcessId(LR2hWnd, &LR2pid);
	LR2h = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, LR2pid);

	return true;
}

int Detector::getMemValInt(LPVOID addr) {
	int _i;
	SIZE_T rl;

	ReadProcessMemory(LR2h, addr, &_i, sizeof(int), &rl);

	return _i;
}

bool Detector::getLR2Status() {
	if (!LR2hWnd) return false;
	if (!IsWindow(LR2hWnd)) return false;

	LR2stat[LR_SCORE] = getMemValInt((LPVOID)LR_SCORE);
	LR2stat[LR_PG] = getMemValInt((LPVOID)LR_PG);
	LR2stat[LR_GR] = getMemValInt((LPVOID)LR_GR);
	LR2stat[LR_GD] = getMemValInt((LPVOID)LR_GD);
	LR2stat[LR_BD] = getMemValInt((LPVOID)LR_BD);
	LR2stat[LR_PR] = getMemValInt((LPVOID)LR_PR);
	LR2stat[LR_DIFF] = getMemValInt((LPVOID)LR_DIFF);
	LR2stat[LR_STAT] = getMemValInt((LPVOID)LR_STAT);
	LR2stat[LR_GUAGE] = getMemValInt((LPVOID)LR_GUAGE);
	LR2stat[LR_EXS] = getMemValInt((LPVOID)LR_EXS);
	LR2stat[LR_EXHS] = getMemValInt((LPVOID)LR_EXHS);
	LR2stat[LR_MC] = getMemValInt((LPVOID)LR_MC);

	int _i, _addr;
	SIZE_T rl;
	char _str[255];
	_addr = getMemValInt((LPVOID)LR_TITLE);
	ReadProcessMemory(LR2h, (LPVOID)_addr, _str, sizeof(_str), &rl);
	wstring nstr;
	eucjp_to_cp949(string(_str), nstr);
	OutputDebugStringA(string(_str).append("\n").c_str());
	OutputDebugStringW(wstring(nstr).append(L"\n").c_str());

	lstrcpy(LR2BMSTitle, nstr.c_str());

	ReadProcessMemory(LR2h, (LPVOID)LR_GUAGENUM, &LR2Guage, sizeof(LR2Guage), &rl);

	return true;
}

void Detector::getLR2StatusString(TCHAR *str)
{
	TCHAR s0[20], s1[255], s2[100], s3[100];

	lstrcpy(s1, LR2BMSTitle);

	if (LR2Guage < 80) {
		lstrcpy(s0, L"플레이");
		lstrcpy(s2, L"FAIL");
	} else {
		lstrcpy(s0, L"클리어");
		switch (LR2stat[LR_GUAGE]) {
			case 3:
				lstrcpy(s2, L"EASY");
				break;
			case 1:
				lstrcpy(s2, L"HARD");
				break;
			case 0:
				lstrcpy(s2, L"GROOVE");
				break;
			default:
				lstrcpy(s2, L"/몰라/");
		}
	}


	float rate = LR2stat[LR_SCORE] / 200000.0f * 100;
	if (rate > 88.88) {
		lstrcpy(s3, L"AAA");
	} else if (rate > 77.77) {
		lstrcpy(s3, L"AA");
	} else if (rate > 66.66) {
		lstrcpy(s3, L"A");
	} else if (rate > 55.55) {
		lstrcpy(s3, L"B");
	} else if (rate > 44.44) {
		lstrcpy(s3, L"C");
	} else if (rate > 33.33) {
		lstrcpy(s3, L"E");
	} else if (rate > 22.22) {
		lstrcpy(s3, L"F");
	} else if (rate > 11.11) {
		lstrcpy(s3, L"F");
	} else {
		lstrcpy(s3, L"F");
	}

	swprintf(str, LR2FormatStr, s1, s2, LR2stat[LR_DIFF], LR2stat[LR_SCORE], s0, rate, s3,
		LR2stat[LR_PG],LR2stat[LR_GR],LR2stat[LR_GD],LR2stat[LR_BD],LR2stat[LR_PR],LR2stat[LR_MC] );
	return;
}