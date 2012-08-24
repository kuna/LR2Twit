#include "stdafx.h"
#include "Twitpic.h"

// for multipart curl 
#ifdef _DEBUG
	#pragma comment(lib, ".\\\\Debug_lib\\libcurld.lib")
	#pragma comment(lib, ".\\\\Debug_lib\\liboauth.lib")
#else
	#pragma comment(lib, "libcurl.lib")
	#pragma comment(lib, "liboauth.lib")
#endif
#include "curl/curl.h"

#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")
#include "oauth.h"

#include <string>
#include "stdio.h"
#include "io.h"
using namespace std;

#define API_PIC_TWITTER "http://upload.twitter.com/1/statuses/update_with_media.xml"
//#define API_PIC_TWITTER "http://api.twitter.com/1/statuses/update.xml"//

void Twitpic::set_account(char *ckey, char *cskey, char *atoken, char *astoken) {
	strcpy(customerkey, ckey);
	strcpy(customersecretkey, cskey);
	strcpy(accesstoken, atoken);
	strcpy(accesstokensecret, astoken);
}

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


Twitpic::Twitpic() {
	strcpy(LR2PicPath, "");
	setSharedMemory();
}

Twitpic::~Twitpic() {
	releaseSharedMemory();
}

string Twitpic::upload_pic(char *filepath, char *comment) {
	char *req_uri;
	char *postarg;

	req_uri = oauth_sign_url2(API_PIC_TWITTER, &postarg, OA_HMAC, NULL, customerkey, customersecretkey,
		accesstoken, accesstokensecret);
	
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

	for (int i=0; i<strlen(postarg); i++) {
		if (postarg[i] == '&') postarg[i] = ',';
	}


	CURL *curl;
	CURLcode res;
	struct curl_slist *slist=NULL;

	struct MemoryStruct chunk;
	chunk.data=NULL;
	chunk.size = 0;

	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, req_uri);
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	
	/* header set */
	char customh[1024];
	slist = curl_slist_append(slist, "Expect:");	// for big data transmission
	slist = curl_slist_append(slist, "Content-Type: multipart/form-data");
	strcpy(customh, "Authorization: OAuth ");
	strcat(customh, postarg);
	slist = curl_slist_append(slist, customh);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist); 
	
	/* data set */
	curl_httppost *formpost=NULL;
	curl_httppost *lastptr=NULL;
	curl_slist *headerlist=NULL;

	curl_formadd( &formpost, &lastptr, CURLFORM_COPYNAME, "media_data[]", CURLFORM_COPYCONTENTS, f_encode_data, CURLFORM_END );
	curl_formadd( &formpost, &lastptr, CURLFORM_COPYNAME, "status", CURLFORM_COPYCONTENTS, comment, CURLFORM_END );
	curl_easy_setopt( curl, CURLOPT_HTTPPOST, formpost );

	// server result in chunk.data
	res = curl_easy_perform(curl);

	curl_slist_free_all(slist);
	curl_easy_cleanup(curl);
	if (f_data) free(f_data);
	if (f_encode_data) free(f_encode_data);
	
	return string(chunk.data);
}

string Twitpic::upload_pic(char *filepath) {
	return upload_pic(filepath, "");
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