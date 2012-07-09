#include "stdafx.h"
#include "Prog_Detector.h"

/*** OS Detector ***/
#ifndef GETPROC
#define GETPROC(dll, type, name) (type)GetProcAddress(dll, name)
#endif

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
LPFN_ISWOW64PROCESS fnIsWow64Process;
BOOL IsWow64()
{
    BOOL bIsWow64 = FALSE;

    //IsWow64Process is not available on all supported versions of Windows.
    //Use GetModuleHandle to get a handle to the DLL that contains the function
    //and GetProcAddress to get a pointer to the function if available.

    fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(
        GetModuleHandle(TEXT("kernel32")),"IsWow64Process");

    if(NULL != fnIsWow64Process)
    {
        if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
        {
            //handle error
        }
    }
    return bIsWow64;
}
typedef BOOL 
(WINAPI *fnVerifyVersionInfo)(LPOSVERSIONINFOEX, DWORD, DWORDLONG);

typedef ULONGLONG 
(WINAPI *fnVerSetConditionMask)(ULONGLONG, DWORD, BYTE);

BOOL _VerifyVersionInfo(LPOSVERSIONINFOEX lpVersionInfo
                        , DWORD dwTypeMask
                        , DWORDLONG dwConditionMask
                        , BOOL *bVerified)
{
    BOOL bRet = FALSE;
    HINSTANCE hDll = NULL;
    fnVerifyVersionInfo FVerifyVersionInfo;
    fnVerSetConditionMask FVerSetConditionMask;

    hDll = LoadLibrary(L"kernel32.dll");
    if(hDll != NULL)
    {
        FVerifyVersionInfo = GETPROC(hDll
                                    , fnVerifyVersionInfo
                                    , "VerifyVersionInfoA");

        FVerSetConditionMask = GETPROC(hDll
                                    , fnVerSetConditionMask
                                    , "VerSetConditionMask");

        if(FVerifyVersionInfo != NULL && FVerSetConditionMask != NULL)
        {
            dwConditionMask = 0;
            if(dwTypeMask & VER_MAJORVERSION)
                dwConditionMask = FVerSetConditionMask(dwConditionMask
                                                    , VER_MAJORVERSION
                                                    , VER_EQUAL);

            if(dwTypeMask & VER_MINORVERSION)
                dwConditionMask = FVerSetConditionMask(dwConditionMask
                                                    , VER_MINORVERSION
                                                    , VER_EQUAL);

            if(dwTypeMask & VER_PLATFORMID)
                dwConditionMask = FVerSetConditionMask(dwConditionMask
                                                    , VER_PLATFORMID
                                                    , VER_EQUAL);

            if(dwTypeMask & VER_PRODUCT_TYPE)
                dwConditionMask = FVerSetConditionMask(dwConditionMask
                                                        , VER_PRODUCT_TYPE
                                                        , VER_EQUAL);


            *bVerified = FVerifyVersionInfo(lpVersionInfo
                                            , dwTypeMask
                                            , dwConditionMask);
            bRet = TRUE;
        }
        FreeLibrary(hDll);
    }

    return bRet;
}

BOOL WINAPI IsWinXP()
{
    OSVERSIONINFOEX osi;
    DWORDLONG dwlConditionMask = 0;
    BOOL bRet = FALSE;
    BOOL bVerified = FALSE;

    ZeroMemory(&osi, sizeof(OSVERSIONINFOEX));
    osi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    osi.dwMajorVersion = 5;
    osi.dwMinorVersion = 1;
    osi.dwPlatformId = VER_PLATFORM_WIN32_NT;

    bRet = _VerifyVersionInfo(&osi
                        , VER_MAJORVERSION | VER_MINORVERSION | VER_PLATFORMID
                        , dwlConditionMask
                        , &bVerified);
    if(bRet == TRUE)
        return bVerified;

    osi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    if(GetVersionEx((OSVERSIONINFO *) &osi) == 0)
    {
        osi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        if(GetVersionEx((OSVERSIONINFO *) &osi) == 0)
            return 0;
    }       
    
	/* check http://www.winasm.net/forum/index.php?showtopic=2723 */
    return  (osi.dwMajorVersion == 5) 
            && (osi.dwMinorVersion == 1) 
            && (osi.dwPlatformId == VER_PLATFORM_WIN32_NT) 
            && (osi.wProductType == VER_NT_WORKSTATION);
}

BOOL WINAPI IsWinXPOver()
{
    OSVERSIONINFOEX osi;
    DWORDLONG dwlConditionMask = 0;
    BOOL bRet = FALSE;
    BOOL bVerified = FALSE;

    ZeroMemory(&osi, sizeof(OSVERSIONINFOEX));
    osi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    osi.dwMajorVersion = 6;
    osi.dwMinorVersion = 1;
    osi.dwPlatformId = VER_PLATFORM_WIN32_NT;

    bRet = _VerifyVersionInfo(&osi
                        , VER_MAJORVERSION | VER_MINORVERSION | VER_PLATFORMID
                        , dwlConditionMask
                        , &bVerified);
    if(bRet == TRUE)
        return bVerified;

    osi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    if(GetVersionEx((OSVERSIONINFO *) &osi) == 0)
    {
        osi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        if(GetVersionEx((OSVERSIONINFO *) &osi) == 0)
            return 0;
    }       
    
	/* check http://www.winasm.net/forum/index.php?showtopic=2723 */
    return  (osi.dwMajorVersion > 5) 
            && (osi.dwPlatformId == VER_PLATFORM_WIN32_NT) 
            && (osi.wProductType == VER_NT_WORKSTATION);
}

bool eucjp_to_cp949(const string& in, wstring& out)
{
	iconv_t cd;
	bool ret = false;
	const char* pszIn;
	char* pszOut, *pos;
	size_t inLen, outLen;
	pszOut=NULL;
	
	cd = iconv_open("UTF-16LE","SHIFT_JIS");
	if(cd == (iconv_t)(-1))
	{
		//OutputDebugString(L"error!\n");
		return false; 
	}
	
	inLen = in.length();
	pszIn = in.c_str();
	
	outLen = (inLen+1) * 2;
	pos = pszOut = (char*)calloc(outLen, sizeof(char));
	if(!pszOut) {
		//OutputDebugString(L"error - no size\n");
		goto clean;
	}
	
	if(iconv(cd, &pszIn, &inLen, &pos, &outLen)==-1) {
		//OutputDebugString(L"error - convert fail\n");
		goto clean;
	}
	out = wstring((TCHAR*)pszOut);
	ret = true;
clean:
	iconv_close(cd);
	if(pszOut) free(pszOut);
	return ret;
}
/*
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
}*/

Detector::Detector(Log *_l) {
	lstrcpy(LR2WinTitle, opt_lr2title);
	lstrcpy(LR2FormatStr, opt_message);

	memset(LR2BMSTitle, 0, sizeof(LR2BMSTitle));

	l = _l;
	OSDetect();

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

void Detector::OSDetect() {
	if (IsWinXP()) {
		_OFFSET = 0x0000040;
		l->writeLogLine(L"Operating System : Windows XP");
		return ;
	}

	if (IsWinXPOver()) {
		if (IsWow64()) {
			_OFFSET = 0x00060000;
			l->writeLogLine(L"Operating System : Windows 7/Vista or Higher x64");
		} else {
			_OFFSET = 0;
			l->writeLogLine(L"Operating System : Windows 7/Vista or Higher x86");
		}
		return ;
	}

	l->writeLogLine(L"OS not detected. default option - Win7K/x86");
}

bool Detector::detectLR2() {
	LR2hWnd = FindWindow(LR2WinTitle, LR2WinTitle);
	if (!LR2hWnd) return false;

	DWORD TID = GetWindowThreadProcessId(LR2hWnd, &LR2pid);
	LR2h = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, LR2pid);

	// get base addr
	/*
	app_hmoudle = LoadLibrary(L"LR2body.exe");
	baseAddr = (size_t)GetModuleHandle(L"LR2body.exe");
	//baseAddr = (size_t)GetModuleHandle(NULL);
	if (!baseAddr) {
		int gl = GetLastError();
		return false;
	}
	FreeLibrary(app_hmoudle);
*/

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

	// get baseOffset
	ReadProcessMemory(LR2h, (LPVOID)baseAddr, &baseOffset, sizeof(baseOffset), 0);

	LR2stat[LR_SCORE] = getMemValInt((LPVOID)(LR_SCORE+_OFFSET));
	LR2stat[LR_PG] = getMemValInt((LPVOID)(LR_PG+_OFFSET));
	LR2stat[LR_GR] = getMemValInt((LPVOID)(LR_GR+_OFFSET));
	LR2stat[LR_GD] = getMemValInt((LPVOID)(LR_GD+_OFFSET));
	LR2stat[LR_BD] = getMemValInt((LPVOID)(LR_BD+_OFFSET));
	LR2stat[LR_PR] = getMemValInt((LPVOID)(LR_PR+_OFFSET));
	LR2stat[LR_EXS] = getMemValInt((LPVOID)(LR_EXS+_OFFSET));
	LR2stat[LR_EXHS] = getMemValInt((LPVOID)(LR_EXHS+_OFFSET));
	LR2stat[LR_MC] = getMemValInt((LPVOID)(LR_MC+_OFFSET));
	LR2stat[LR_NC] = getMemValInt((LPVOID)(LR_NC+_OFFSET));
	LR2stat[LR_DIFF] = getMemValInt((LPVOID)(LR_DIFF+_OFFSET));
	LR2stat[LR_STAT] = getMemValInt((LPVOID)(LR_STAT+_OFFSET));
	LR2stat[LR_GUAGE] = getMemValInt((LPVOID)(LR_GUAGE+_OFFSET));
	LR2stat[LR_AUTO] = getMemValInt((LPVOID)(LR_AUTO+_OFFSET));
	LR2stat[LR_AUTOSCR] = getMemValInt((LPVOID)(LR_AUTOSCR+_OFFSET));
	LR2stat[LR_IRTOT] = getMemValInt((LPVOID)(LR_IRTOT+_OFFSET));
	LR2stat[LR_IRNOW] = getMemValInt((LPVOID)(LR_IRNOW+_OFFSET));

	int _i, _addr;
	SIZE_T rl;
	char _str[255];
	wstring nstr;

	_addr = getMemValInt((LPVOID)(LR_MAINTITLE+_OFFSET));
	ReadProcessMemory(LR2h, (LPVOID)_addr, _str, sizeof(_str), &rl);
	eucjp_to_cp949(string(_str), nstr);
	lstrcpy(LR2BMSMainTitle, nstr.c_str());

	_addr = getMemValInt((LPVOID)(LR_SUBTITLE+_OFFSET));
	ReadProcessMemory(LR2h, (LPVOID)_addr, _str, sizeof(_str), &rl);
	eucjp_to_cp949(string(_str), nstr);
	lstrcpy(LR2BMSSubTitle, nstr.c_str());
	
	_addr = getMemValInt((LPVOID)(LR_TITLE+_OFFSET));
	ReadProcessMemory(LR2h, (LPVOID)_addr, _str, sizeof(_str), &rl);
	eucjp_to_cp949(string(_str), nstr);
	lstrcpy(LR2BMSTitle, nstr.c_str());

	_addr = getMemValInt((LPVOID)(LR_ARTIST+_OFFSET));
	ReadProcessMemory(LR2h, (LPVOID)_addr, _str, sizeof(_str), &rl);
	eucjp_to_cp949(string(_str), nstr);
	lstrcpy(LR2BMSArtist, nstr.c_str());

	_addr = getMemValInt((LPVOID)(LR_GENRE+_OFFSET));
	ReadProcessMemory(LR2h, (LPVOID)_addr, _str, sizeof(_str), &rl);
	eucjp_to_cp949(string(_str), nstr);
	lstrcpy(LR2BMSGenre, nstr.c_str());
/*
	OutputDebugStringA(string(_str).append("\n").c_str());
	OutputDebugStringA(string(nstr).append("\n").c_str());
	TCHAR wstr[255];
	int iLen = ::MultiByteToWideChar(CP_ACP, 0, nstr.c_str(), -1, wstr, 0);
	::MultiByteToWideChar(CP_ACP, 0, nstr.c_str(), -1, wstr, iLen);
	lstrcpy(LR2BMSTitle, wstr);
*/

	//OutputDebugStringA(string(_str).append("\n").c_str());
	//OutputDebugStringW(wstring(nstr).append(L"\n").c_str());

	ReadProcessMemory(LR2h, (LPVOID)(LR_GUAGENUM+_OFFSET), &LR2Guage, sizeof(LR2Guage), &rl);

	return true;
}

bool Detector::isHighScore() {
	if (!LR2hWnd) return false;
	if (!IsWindow(LR2hWnd)) return false;
	return (LR2stat[LR_EXHS]>LR2stat[LR_EXS]);
}
bool Detector::isAutoPlaying() {
	if (!LR2hWnd) return false;
	if (!IsWindow(LR2hWnd)) return false;
	return (LR2stat[LR_AUTO]);
}

bool Detector::isCleared() {
	if (!LR2hWnd) return false;
	if (!IsWindow(LR2hWnd)) return false;
	bool guage_hard = false;
	switch (LR2stat[LR_GUAGE]) {
		case 1:
			guage_hard = true;
			break;
	}
	return !((LR2Guage < 80 && !guage_hard) || (LR2Guage==0 && guage_hard));
}

bool Detector::isResultScreen() {
	if (!LR2hWnd) return false;
	if (!IsWindow(LR2hWnd)) return false;
	return (LR2stat[LR_STAT] == 2);
}

void Detector::getLR2StatusString(TCHAR *str)
{
	TCHAR s0[20], s1[255], s2[100], s3[100], s4[255], s5[255], s6[255], s7[10];
	TCHAR _s[255];

	lstrcpy(s1, LR2BMSTitle);
	lstrcpy(s4, LR2BMSSubTitle);
	lstrcpy(s5, LR2BMSArtist);
	lstrcpy(s6, LR2BMSGenre);

	bool guage_hard = false;
	switch (LR2stat[LR_GUAGE]) {
		case 3:
			lstrcpy(s2, L"EASY");
			break;
		case 0:
			lstrcpy(s2, L"GROOVE");
			break;
		case 1:
			lstrcpy(s2, L"HARD");
			guage_hard = true;
			break;
		case 2:
			lstrcpy(s2, L"DEATH");
			guage_hard = true;
			break;
		case 4:
			lstrcpy(s2, L"P-ATTACK");
			guage_hard = true;
			break;
		case 5:
			lstrcpy(s2, L"G-ATTACK");
			guage_hard = true;
			break;
		default:
			lstrcpy(s2, L"//");
	}
	if (LR2stat[LR_MC] == LR2stat[LR_PG] + LR2stat[LR_GR] + LR2stat[LR_GD] )
		lstrcpy(s2, L"FULLCOMBO");

	if (!isCleared()) {
		lstrcpy(s0, L"FAIL");
	} else {
		lstrcpy(s0, L"CLEAR");
	}


	float rate = (float)LR2stat[LR_EXS] / (float)(LR2stat[LR_NC] * 2) * 100.0f;
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

	lstrcpy(str, LR2FormatStr);

	_itow(LR2stat[LR_DIFF], _s, 10);
	wcscpy(s7, L"☆");
	wcscat(s7, _s);
	checkDiffLevel(LR2BMSTitle, s7);

	replace_str(str, L"[RANK]", s3);
	replace_str(str, L"[GUAGE]", s2);
	replace_str(str, L"[RESULT]", s0);
	replace_str(str, L"[MAINTITLE]", LR2BMSMainTitle);
	replace_str(str, L"[SUBTITLE]", LR2BMSSubTitle);
	replace_str(str, L"[TITLE]", LR2BMSTitle);
	replace_str(str, L"[ARTIST]", s5);
	replace_str(str, L"[GENRE]", s6);
	replace_str(str, L"[EXS]", LR2stat[LR_EXS]);
	replace_str(str, L"[EXHS]", LR2stat[LR_EXHS]);
	replace_str(str, L"[EXMS]", LR2stat[LR_NC] * 2);
	replace_str(str, L"[PG]", LR2stat[LR_PG]);
	replace_str(str, L"[GR]", LR2stat[LR_GR]);
	replace_str(str, L"[GD]", LR2stat[LR_GD]);
	replace_str(str, L"[BD]", LR2stat[LR_BD]);
	replace_str(str, L"[PR]", LR2stat[LR_PR]);
	replace_str(str, L"[NC]", LR2stat[LR_NC]);
	replace_str(str, L"[MC]", LR2stat[LR_MC]);
	//replace_str(str, L"[DIFF]", LR2stat[LR_DIFF]);
	replace_str(str, L"[DIFF]", s7);
	replace_str(str, L"[SCORE]", LR2stat[LR_SCORE]);
	replace_str(str, L"[IRTOT]", LR2stat[LR_IRTOT]);
	replace_str(str, L"[IRNOW]", LR2stat[LR_IRNOW]);
	if (LR2stat[LR_AUTOSCR])
		replace_str(str, L"[AUTO]", L"(AUTO-SCR)");
	else 
		replace_str(str, L"[AUTO]", L"");
	swprintf(_s, L"%.2f", rate);
	replace_str(str, L"[RATE]", _s);

	/*swprintf(str, LR2FormatStr, s1, s2, LR2stat[LR_DIFF], LR2stat[LR_EXS], s0, rate, s3,
		LR2stat[LR_PG],LR2stat[LR_GR],LR2stat[LR_GD],LR2stat[LR_BD],LR2stat[LR_PR],LR2stat[LR_MC] );
	*/return;
}

/**depreciated*/
void Detector::replace_str(TCHAR *org, TCHAR *find, int n) {
	TCHAR _n[255];
	_itow(n, _n, 10);
	replace_str(org, find, _n);
}

void Detector::replace_str(TCHAR *org, TCHAR *find, TCHAR *n) {
	TCHAR *sr, *org_pt, result[255];//*result, *sr;
	size_t i, count = 0;
	size_t oldlen = wcslen(find); if (oldlen < 1) return;
	size_t newlen = wcslen(n);

	org_pt = org;
	if (newlen != oldlen) {
		for (i = 0; org[i] != L'\0';) {
			if (memcmp(&org[i], find, oldlen*2) == 0) count++, i += oldlen;
			else i++;
		}
	} else i = wcslen(org);

	//result = (TCHAR *) malloc((i + 1 + count * (newlen - oldlen))*2);
	//if (result == NULL) return;

	sr = result;
	while (*org) {
		if (memcmp(org, find, oldlen*2) == 0) {
			memcpy(sr, n, newlen*2);
			sr += newlen;
			org += oldlen;
		} else {
			//*sr = *org;
			//sr+=2;
			//org+=2;
			*sr++ = *org++;
		}
	}
	*sr = L'\0';

	lstrcpy(org_pt, result);


	//free(result);
	//return result;
}

void Detector::checkDiffLevel(TCHAR *title, TCHAR *diff)
{
	FILE *fp = fopen("level.txt", "rb");
	if (!fp) return;
	fseek(fp, 2, SEEK_SET);	// important

	TCHAR buf[255];
	TCHAR ndiff[10];
	while (!feof(fp)) {
		fgetws(buf, sizeof(buf), fp);
		int l = wcslen(buf);
		if (buf[l-1] == L'\n')
			buf[l-2] = L'\0';
		if (wcslen(buf)==0)
			continue;
		if (memcmp(buf,L"//",4) == 0)
			continue;
		if (memcmp(buf,L"lvl",6) == 0) {
			wcscpy(ndiff, buf+4);
		} else if (match(buf, title)/*wcscmp(buf, tt)==0*/) {
			wcscpy(diff, ndiff);
			wcscpy(title, buf);// char change
			return;
		}
	}

	fclose(fp);
}

bool Detector::IsIIDXBMS(TCHAR *title)
{
	FILE *fp = fopen("level.txt", "r");
	if (!fp) return false;
	fseek(fp, 2, SEEK_SET);

	wcscat(title, L"\r\n");
	TCHAR buf[255];
	while (!feof(fp)) {
		fgetws(buf, sizeof(buf), fp);
		if (memcmp(buf,L"//",4) == 0)
			continue;
		if (wcscmp(title, buf)==0)
			return true;
	}

	return false;
}


BOOL match(TCHAR *fname, TCHAR *filter) {
	int l = wcslen(filter);
	int j=0;
	for (int i=0; i<l; i++) {
		if (filter[i] == L'?') {
			j++;
			continue;
		} else if (filter[i] == L'*') {
			int k=1;
			while (TRUE) {
				if (filter[i+k] == 0 && fname[j+k-1] == 0) return TRUE;	// 같이 끝나면 true
				if (fname[j+k-1] == 0) return FALSE;	// 목표 문자열이 먼저 끝나면 false
				if (filter[i+k] == L'?' || filter[i+k] == L'*') {	// 다음 syntax면 exit
					j += k-1;
					i += k-1;
					break;
				}
				if (filter[i+k] != fname[j+k-1]) {
					j++;
					k=0;
				}
				k++;
			}
		} else {
			if (fname[i]!=filter[j]) return FALSE;
			j++;
		}
	}

	if (j != wcslen(fname)) return FALSE;

	return TRUE;
}