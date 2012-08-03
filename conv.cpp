#include "stdafx.h"
#include "conv.h"
bool cp949_to_utf8(const string& in, string& out)
{
	iconv_t cd;
	bool ret = false;
	const char* pszIn;
	char* pszOut, *pos;
	size_t inLen, outLen;
	pszOut=NULL;
	
	cd = iconv_open("UTF-8",opt_encode);
	if(cd == (iconv_t)(-1))
	{
		return false; 
	}
	
	inLen = in.length();
	pszIn = in.c_str();
	
	outLen = (inLen+1) * 2;
	pos = pszOut = (char*)calloc(outLen, sizeof(char));
	if(!pszOut) goto clean;
	
	if(iconv(cd, &pszIn, &inLen, &pos, &outLen)==-1)
		goto clean;
	out = pszOut;
	ret = true;
clean:
	iconv_close(cd);
	if(pszOut) free(pszOut);
	return ret;
}
