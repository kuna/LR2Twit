#pragma once

#include <string>
using namespace std;
#include "iconv.h"
#pragma comment(lib, "iconv.lib")

class Tool {
public:
	static BOOL match(TCHAR *fname, TCHAR *filter);
	static BOOL eucjp_to_cp949(const string& in, wstring& out);
};