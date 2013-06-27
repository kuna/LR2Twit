#include "stdafx.h"
#include "Tool.h"

BOOL Tool::match(TCHAR *fname, TCHAR *filter) {
	// auto trim for ...
	int l = wcslen(filter);
	if (filter[l-2] == 13) filter[l-2]=0, l-=2;
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

BOOL Tool::eucjp_to_cp949(const string& in, wstring& out)
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