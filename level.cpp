#include "stdafx.h"
#include "level.h"
#include "Tool.h"
#include <stdio.h>

#ifdef _DEBUG
	#pragma comment(lib, ".\\\\Debug_lib\\libcurld.lib")
	#pragma comment(lib, ".\\\\Debug_lib\\liboauth.lib")
#else
	#pragma comment(lib, "libcurl.lib")
	#pragma comment(lib, "liboauth.lib")
#endif
#include "curl/curl.h"

/* curl func */
int writer(char *data, size_t size, size_t nmemb, string *buffer)
{
  int result = 0;
  if (buffer != NULL)
  {
      buffer->append(data, size * nmemb);
      result = size * nmemb;
  }
  return result;
}

Data_Level::Data_Level() {
	l = 0;
}

void Data_Level::setLog(Log *_l) {
	l = _l;
}

BOOL Data_Level::LoadFile(TCHAR *filename) {
	if (xml.Load( filename )) {
		m_cnt = 0;
		xml.FindElem(L"list");
		xml.IntoElem();

		while (xml.FindElem(L"song")) {
			xml.IntoElem();

			xml.FindElem(L"hash");
			wstring str = xml.GetData();
			wcscpy(m_Hash[ m_cnt ], str.c_str());
			xml.FindElem(L"exlevel");
			str = xml.GetData();
			m_lv[ m_cnt ] = _wtoi( str.c_str() );

			xml.OutOfElem();


			m_cnt++;

			if (m_cnt >= MAX_LIST) {
				// DATA Overflow
				if (l) l->writeLogLine("[INSANE XML] Data too much, stopped loading");
				return TRUE;
			}
		}
	} else {
		if (LoadFromInternet()) {
			LoadFile(filename);
		} else {
			return FALSE;
		}
	}

	return TRUE;
}

BOOL Data_Level::LoadFromInternet() {
	CURL *curl;
	CURLcode res;
	string buffer;

	curl = curl_easy_init();
	
    curl_easy_setopt(curl, CURLOPT_URL, _INSANE_URL);
    curl_easy_setopt(curl, CURLOPT_HEADER, 0);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	if (res == CURLE_OK) {
		FILE *fp;
		fp = fopen("getinsanelist.xml", "wb");
		if (!fp) return FALSE;
		fwrite(buffer.c_str()+1, 1, buffer.length()-1, fp);
		fclose(fp);
		return TRUE;
	} else {
		return FALSE;
	}
}

int Data_Level::getInsaneLevel(TCHAR *hash) {
	for (int i=0; i<m_cnt; i++)
		if (wcscmp(hash, m_Hash[i])==0)
			return m_lv[i];

	return -1;
}