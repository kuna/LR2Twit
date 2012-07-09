#include "stdafx.h"
#include "log.h"
#include "stdio.h"
#include "time.h"

bool Log::InitLog(WCHAR *filename)
{
	return InitLog(filename, L"a,ccs=UTF-16LE");
}

bool Log::InitLog(WCHAR *filename, WCHAR *filemode)
{
	if (fp) {
		endLog();
	}

	fp = _wfopen(filename, filemode);
	if (!fp) return false;

	fseek(fp, 0, SEEK_END);
	/*if (ftell(fp) == 0) {
		char _BOM[] = {0xFF, 0xFE};
		fwrite(_BOM, sizeof(char), 2, fp);
	}*/

	return true;
}

bool Log::isLogInitalized() {
	return (fp);
}

bool Log::endLog()
{
	if (!fp) return false;
	fclose(fp);
	return true;
}

int Log::writeLogLine(WCHAR *key, WCHAR *body) {
	return fwprintf(fp, L"[%s], %s\n", key, body);
}
int Log::writeLogLine(WCHAR *key, const char* body) {
	return fwprintf(fp, L"[%s], %S\n", key, body);
}
int Log::writeLogLine_chr(WCHAR *key, WCHAR body) {
	return fwprintf(fp, L"[%s], %c\n", key, body);
}
int Log::writeLogLine_chr(WCHAR *key, char body) {
	return fwprintf(fp, L"[%s], %C\n", key, body);
}
int Log::writeLogLine_hex(WCHAR *key, int body) {
	return fwprintf(fp, L"[%s], 0x%02lX\n", key, body);
}
int Log::writeLogLine(WCHAR *key, int body) {
	return fwprintf(fp, L"[%s], %d\n", key, body);
}
int Log::writeLogLine(WCHAR *key, double body) {
	return fwprintf(fp, L"[%s], %lf\n", key, body);
}
int Log::writeLogLine(WCHAR *key, float body) {
	return fwprintf(fp, L"[%s], %f\n", key, body);
}
int Log::writeLogEmptyLine() {
	return fwprintf(fp, L"\n");
}


int Log::writeLogLine(WCHAR *body) {
	return fwprintf(fp, L"%s\n", body);
}
int Log::writeLogLine(const char* body) {
	return fwprintf(fp, L"%S\n", body);
}
int Log::writeLogLine_chr(WCHAR body) {
	return fwprintf(fp, L"%c\n", body);
}
int Log::writeLogLine_chr(char body) {
	return fwprintf(fp, L"%C\n", body);
}
int Log::writeLogLine_hex(int body) {
	return fwprintf(fp, L"0x%02lX\n", body);
}
int Log::writeLogLine(int body) {
	return fwprintf(fp, L"%d\n", body);
}
int Log::writeLogLine(double body) {
	return fwprintf(fp, L"%lf\n", body);
}
int Log::writeLogLine(float body) {
	return fwprintf(fp, L"%f\n", body);
}

int Log::writeDate() {
	WCHAR str[255];
	getDate(str);
	return fwprintf(fp, L"%s\n", str);
}
void Log::getDate(WCHAR *date)
{
	time_t timer;
	struct tm *t;

	timer = time(NULL); // 현재 시각을 초 단위로 얻기

	t = localtime(&timer); // 초 단위의 시간을 분리하여 구조체에 넣기

	swprintf(date, L"%d년 %d월 %d일 -  %d시 %d분 %d초",
		t->tm_year + 1900,
		t->tm_mon+1,
		t->tm_mday,
		t->tm_hour,
		t->tm_min,
		t->tm_sec);
}