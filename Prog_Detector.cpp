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
	
	cd = iconv_open("UTF-16LE",opt_encode);
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

Detector::Detector(Log *_l) {
	lstrcpy(LR2WinTitle, opt_lr2title);
	lstrcpy(LR2FormatStr, opt_message);

	memset(LR2BMSTitle, 0, sizeof(LR2BMSTitle));

	l = _l;
	OSDetect();
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

	return true;
}

int Detector::getMemValInt(LPVOID addr) {
	int _i;
	SIZE_T rl;

	ReadProcessMemory(LR2h, addr, &_i, sizeof(int), &rl);

	return _i;
}

/* for trim func */
wstring trimString( const wstring& s )
{
	typedef wstring::const_iterator StringIter;

	int begin = s.find_first_not_of(L' ');
	if (begin<0) begin = 0;

	int end	= s.find_last_not_of(L' ');
	if (end < 0) end = 0;

	return s.substr( begin, end-begin+1 );
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
	LR2stat[LR_GUAGE] = getMemValInt((LPVOID)(LR_GUAGE+_OFFSET));
	LR2stat[LR_AUTO] = getMemValInt((LPVOID)(LR_AUTO+_OFFSET));
	LR2stat[LR_AUTOSCR] = getMemValInt((LPVOID)(LR_AUTOSCR+_OFFSET));
	LR2stat[LR_IRTOT] = getMemValInt((LPVOID)(LR_IRTOT+_OFFSET));
	LR2stat[LR_IRNOW] = getMemValInt((LPVOID)(LR_IRNOW+_OFFSET));
	LR2stat[LR_MODE] = getMemValInt((LPVOID)(LR_MODE+_OFFSET));
	
	LR2stat[LR_ISMENU] = getMemValInt((LPVOID)(LR_ISMENU+_OFFSET));
	LR2stat[LR_ISPLAYING] = getMemValInt((LPVOID)(LR_ISPLAYING+_OFFSET));
	LR2stat[LR_ISRESULT] = getMemValInt((LPVOID)(LR_ISRESULT+_OFFSET));

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
	lstrcpy(LR2BMSTitle, trimString(nstr).c_str());	// trim

	_addr = getMemValInt((LPVOID)(LR_ARTIST+_OFFSET));
	ReadProcessMemory(LR2h, (LPVOID)_addr, _str, sizeof(_str), &rl);
	eucjp_to_cp949(string(_str), nstr);
	lstrcpy(LR2BMSArtist, nstr.c_str());

	_addr = getMemValInt((LPVOID)(LR_GENRE+_OFFSET));
	ReadProcessMemory(LR2h, (LPVOID)_addr, _str, sizeof(_str), &rl);
	eucjp_to_cp949(string(_str), nstr);
	lstrcpy(LR2BMSGenre, nstr.c_str());

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
	return (LR2stat[LR_ISRESULT] == 1);
}

bool Detector::isPlaying() {
	if (!LR2hWnd) return false;
	if (!IsWindow(LR2hWnd)) return false;
	return (LR2stat[LR_ISPLAYING] == 1);
}

bool Detector::isMenu() {
	if (!LR2hWnd) return false;
	if (!IsWindow(LR2hWnd)) return false;
	return (LR2stat[LR_ISMENU] == 1);
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
	if (LR2stat[LR_MC] == LR2stat[LR_NC] )
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
	checkDiffLevel(LR2BMSTitle, s7, LR2stat[LR_NC], LR2stat[LR_MODE]);

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
	replace_str(str, L"[KEY]", LR2stat[LR_MODE]);
	if (LR2stat[LR_AUTOSCR])
		replace_str(str, L"[AUTO]", L"(AUTO-SCR)");
	else 
		replace_str(str, L"[AUTO]", L"");
	swprintf(_s, L"%.2f", rate);
	replace_str(str, L"[RATE]", _s);

	return;
}

/**depreciated*/
void Detector::replace_str(TCHAR *org, TCHAR *find, int n) {
	TCHAR _n[255];
	_itow(n, _n, 10);
	replace_str(org, find, _n);
}

void Detector::replace_str(TCHAR *org, TCHAR *find, TCHAR *n) {
	TCHAR *sr, *org_pt, result[255];
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

	sr = result;
	while (*org) {
		if (memcmp(org, find, oldlen*2) == 0) {
			memcpy(sr, n, newlen*2);
			sr += newlen;
			org += oldlen;
		} else {
			*sr++ = *org++;
		}
	}
	*sr = L'\0';

	lstrcpy(org_pt, result);
}

void Detector::checkDiffLevel(TCHAR *title, TCHAR *diff, int totalNoteCnt, int keymode)
{
	FILE *fp = fopen("level.txt", "rb");
	if (!fp) return;
	fseek(fp, 2, SEEK_SET);	// important

	TCHAR preDiff[256];
	lstrcpy(preDiff, L"");

	int nkeymode = 7;
	TCHAR buf[255];
	TCHAR ndiff[10];
	while (!feof(fp)) {
		fgetws(buf, sizeof(buf), fp);
		int l = wcslen(buf);
		if (buf[l-1] == L'\n')
			buf[l-2] = L'\0';
		if (wcslen(buf)==0)
			continue;
		if (memcmp(buf,L"pre",6) == 0 && keymode == nkeymode)
			wcscpy(preDiff, buf+4);
		if (memcmp(buf,L"//",4) == 0)
			continue;
		if (memcmp(buf,L"lvl",6) == 0) {
			wcscpy(ndiff, buf+4);
		} else if (memcmp(buf, L"key", 6) == 0) {
			nkeymode = _wtoi(buf+4);
		} else if (memcmp(buf,L"<n>",6) == 0) {
			// custom option - note
			TCHAR notecnt[10];
			fgetws(notecnt, sizeof(notecnt), fp);
			int nc = _wtoi(notecnt);

			if (match(buf+4, title) && totalNoteCnt == nc && nkeymode == keymode) {
				wcscpy(diff, ndiff);
				wcscpy(title, buf+4);
				break;
			}
		} else if (match(buf, title) && nkeymode == keymode) {
			wcscpy(diff, ndiff);
			wcscpy(title, buf);
			break;
		}
	}

	fclose(fp);
	
	lstrcat(preDiff, diff);
	lstrcpy(diff, preDiff);
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
	int i,j=0;
	for (i=0; i<l; i++) {
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
			while (fname[i+1] == L' ')
				i++;
			while (filter[j+1] == L' ')
				j++;
			j++;
		}
	}

	if (fname[i] == 0 && filter[j] == 0) return TRUE;
	else return FALSE;
}

bool Detector::isLR2Vaild() {
	if (!LR2hWnd) return false;
	if (!IsWindow(LR2hWnd)) return false;
	return true;
}

void Detector::setRecordAlways(){
	DWORD addr = 0x000EF8BC + _OFFSET;
	int val = 1;
	SIZE_T len;
	WriteProcessMemory(LR2h, (LPVOID)(addr), &val, sizeof(val), &len);
}