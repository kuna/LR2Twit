#include "stdafx.h"
#include "Twitpic.h"

#include <string>
#include "stdio.h"
#include "io.h"

#include <string>
using namespace std;

std::string char2hex( char dec )
{
	char dig1 = (dec&0xF0)>>4;
	char dig2 = (dec&0x0F);
	if ( 0<= dig1 && dig1<= 9) dig1+=48;    //0,48 in ascii
	if (10<= dig1 && dig1<=15) dig1+=65-10; //A,65 in ascii
	if ( 0<= dig2 && dig2<= 9) dig2+=48;
	if (10<= dig2 && dig2<=15) dig2+=65-10;

    std::string r;
	r.append( &dig1, 1);
	r.append( &dig2, 1);
	return r;
}

std::string urlencode( const std::string &c )
{

    std::string escaped;
	int max = c.length();
	for(int i=0; i<max; i++)
	{
		if ( (48 <= c[i] && c[i] <= 57) ||//0-9
			(65 <= c[i] && c[i] <= 90) ||//ABC...XYZ
			(97 <= c[i] && c[i] <= 122) || //abc...xyz
			(c[i]=='~' || c[i]=='-' || c[i]=='_' || c[i]=='.')
			)
		{
			escaped.append( &c[i], 1);
		}
		else
		{
			escaped.append("%");
			escaped.append( char2hex(c[i]) );//converts char 255 to string "FF"
		}
	}
	return escaped;
}


Twitpic::Twitpic() {
	strcpy(LR2PicPath, "");
	setSharedMemory();
}

Twitpic::~Twitpic() {
	releaseSharedMemory();
}

void Twitpic::getRawData(char *filepath, char **data, int *size) {
	FILE *fp = fopen(filepath, "rb");
	if (!fp) {
		*size = 0;
		return;
	}
	fseek(fp, 0, SEEK_END);
	int f_len = ftell(fp);
	*data = (char*)malloc(f_len + 100);
	fseek(fp, 0, SEEK_SET);
	fread(*data, 1, f_len, fp);
	fclose(fp);
	*size = f_len;
}

// shared memory
void Twitpic::setSharedMemory() {
	hmm_LR2PicPath = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(int), L"LR2PicPath");
	if (!hmm_LR2PicPath) {
		return;
	}
	p_LR2PicPath = (char*)MapViewOfFile(hmm_LR2PicPath, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (!p_LR2PicPath) {
		return;
	}

	hmm_LR2ScreenCapture = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(int), L"LR2ScreenCapture");
	if (!hmm_LR2ScreenCapture) {
		return;
	}
	p_LR2ScreenCapture = (int*)MapViewOfFile(hmm_LR2ScreenCapture, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (!p_LR2ScreenCapture) {
		return;
	}
}

void Twitpic::releaseSharedMemory()
{
	UnmapViewOfFile(p_LR2ScreenCapture);
	CloseHandle(hmm_LR2ScreenCapture);
	UnmapViewOfFile(p_LR2PicPath);
	CloseHandle(hmm_LR2PicPath);
}

// memory should be auto release
void Twitpic::getLR2PicPath() {
	// get screen capture path
	strcpy(LR2PicPath, p_LR2PicPath);
}

// memory should be auto release
BOOL Twitpic::CaptureScreen()
{
	if (!strlen(LR2PicPath))
		getLR2PicPath();

	int LR2Scr = *p_LR2ScreenCapture;

	if (LR2Scr == 0) {
		// set the application to capture the file
		*p_LR2ScreenCapture = 1;
	}

	// attempt to check Captured file
	for (int i=0; i<10; i++) {
		// check for 1 sec
		LR2Scr = *p_LR2ScreenCapture;
		if (LR2Scr == 2)
		{
			*p_LR2ScreenCapture = 0;
			return TRUE;
		} else if (LR2Scr == 3) {
			*p_LR2ScreenCapture = 0;
			return FALSE;	// failed
		}
		Sleep(100);
	}

	return FALSE;
}