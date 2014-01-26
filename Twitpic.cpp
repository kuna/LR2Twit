#include "stdafx.h"
#include "Twitpic.h"

#include <string>
#include "stdio.h"
#include "io.h"
using namespace std;

#include <string>
using namespace std;

/*------ Base64 Encoding Table ------*/
static const char MimeBase64[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/'
};

/*------ Base64 Decoding Table ------*/
static int DecodeMimeBase64[256] = {
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 00-0F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 10-1F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,  /* 20-2F */
    52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,  /* 30-3F */
    -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,  /* 40-4F */
    15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,  /* 50-5F */
    -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,  /* 60-6F */
    41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,  /* 70-7F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 80-8F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 90-9F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* A0-AF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* B0-BF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* C0-CF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* D0-DF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* E0-EF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1   /* F0-FF */
    };

int base64_decode(char *text, unsigned char *dst, int numBytes )
{
  const char* cp;
  int space_idx = 0, phase;
  int d, prev_d = 0;
  unsigned char c;

    space_idx = 0;
    phase = 0;

    for ( cp = text; *cp != '\0'; ++cp ) {
        d = DecodeMimeBase64[(int) *cp];
        if ( d != -1 ) {
            switch ( phase ) {
                case 0:
                    ++phase;
                    break;
                case 1:
                    c = ( ( prev_d << 2 ) | ( ( d & 0x30 ) >> 4 ) );
                    if ( space_idx < numBytes )
                        dst[space_idx++] = c;
                    ++phase;
                    break;
                case 2:
                    c = ( ( ( prev_d & 0xf ) << 4 ) | ( ( d & 0x3c ) >> 2 ) );
                    if ( space_idx < numBytes )
                        dst[space_idx++] = c;
                    ++phase;
                    break;
                case 3:
                    c = ( ( ( prev_d & 0x03 ) << 6 ) | d );
                    if ( space_idx < numBytes )
                        dst[space_idx++] = c;
                    phase = 0;
                    break;
            }
            prev_d = d;
        }
    }

    return space_idx;

}

int base64_encode(char *text, int numBytes, char **encodedText)
{
  unsigned char input[3]  = {0,0,0};
  unsigned char output[4] = {0,0,0,0};
  int   index, i, j, size;
  char *p, *plen;

  plen           = text + numBytes - 1;
  size           = (4 * (numBytes / 3)) + (numBytes % 3? 4 : 0) + 1;
  (*encodedText) = (char*)malloc(size);
  j              = 0;

    for  (i = 0, p = text;p <= plen; i++, p++) {
        index = i % 3;
        input[index] = *p;

        if (index == 2 || p == plen) {
            output[0] = ((input[0] & 0xFC) >> 2);
            output[1] = ((input[0] & 0x3) << 4) | ((input[1] & 0xF0) >> 4);
            output[2] = ((input[1] & 0xF) << 2) | ((input[2] & 0xC0) >> 6);
            output[3] = (input[2] & 0x3F);

            (*encodedText)[j++] = MimeBase64[output[0]];
            (*encodedText)[j++] = MimeBase64[output[1]];
            (*encodedText)[j++] = index == 0? '=' : MimeBase64[output[2]];
            (*encodedText)[j++] = index <  2? '=' : MimeBase64[output[3]];

            input[0] = input[1] = input[2] = 0;
        }
    }

    (*encodedText)[j] = '\0';

    return 0;
}

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

string Twitpic::getHashData(char *filepath) {
	char *f_data = (char*)malloc(3000000);
	char *f_encode_data;
	FILE *fp = fopen(filepath, "rb");
	if (!fp) return 0;
	fseek(fp, 0, SEEK_END);
	int f_len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	fread(f_data, 1, f_len, fp);
	fclose(fp);
	base64_encode(f_data, f_len, &f_encode_data);
	OutputDebugStringA(f_encode_data);

	string _ret = string(f_encode_data);

	if (f_data) free(f_data);
	if (f_encode_data) free(f_encode_data);

	return _ret;
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