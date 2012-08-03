#include "stdafx.h"
#include "lang.h"

string replaceAll(const string &str, const string &pattern, const string &replace)
{
	string result = str;
	string::size_type pos = 0;
	string::size_type offset = 0;

	while((pos = result.find(pattern, offset)) != string::npos)
	{
		result.replace(result.begin() + pos, result.begin() + pos + pattern.size(), replace);
		offset = pos + replace.size();
	}

	return result;
}

wstring replaceAll(const wstring &str, const wstring &pattern, const wstring &replace)
{
	wstring result = str;
	wstring::size_type pos = 0;
	wstring::size_type offset = 0;

	while((pos = result.find(pattern, offset)) != string::npos)
	{
		result.replace(result.begin() + pos, result.begin() + pos + pattern.size(), replace);
		offset = pos + replace.size();
	}

	return result;
}

std::wstring LanguageSetting::GetLanguageW(TCHAR *key, TCHAR *name) {
	TCHAR str[255];
	GetPrivateProfileStringW(key, name, L"", str, 255, L".\\lang.ini");
	if (wcslen(str) == 0) {
		swprintf(str, L"Error: No Language keyname \"%s\" in Key \"%s\".", key, name);
	}

	return replaceAll(std::wstring(str), L"\\n", L"\n");
}

std::string LanguageSetting::GetLanguageA(char *key, char *name) {
	char str[255];
	GetPrivateProfileStringA(key, name, "", str, 255, ".\\lang.ini");
	if (strlen(str) == 0) {
		sprintf(str, "Error: No Language keyname \"%s\" in Key \"%s\".", key, name);
	}
	return replaceAll(std::string(str), "\\n", "\n");
}
