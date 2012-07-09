#ifndef _LOG
#define _LOG
class Log {
public:
	bool InitLog(WCHAR* filename);
	bool InitLog(WCHAR* filename, WCHAR* filemode);
	int writeLogLine(WCHAR* key, WCHAR* body);
	int writeLogLine(WCHAR* key, const char* body);
	int writeLogLine(WCHAR* key, int body);
	int writeLogLine(WCHAR* key, double body);
	int writeLogLine(WCHAR* key, float body);
	int writeLogLine(WCHAR* key, char body);
	int writeLogLine(WCHAR* key, WCHAR body);
	int writeLogLine_chr(WCHAR* key, char body);
	int writeLogLine_chr(WCHAR* key, WCHAR body);
	int writeLogLine_hex(WCHAR* key, int body);
	int writeLogEmptyLine();

	int writeLogLine(WCHAR* body);
	int writeLogLine(const char* body);
	int writeLogLine(int body);
	int writeLogLine(double body);
	int writeLogLine(float body);
	int writeLogLine(char body);
	int writeLogLine(WCHAR body);
	int writeLogLine_chr(char body);
	int writeLogLine_chr(WCHAR body);
	int writeLogLine_hex(int body);

	int writeDate();

	bool isLogInitalized();
	bool endLog();
private:
	FILE *fp;
	void getDate(WCHAR* date);
};
#else
#endif