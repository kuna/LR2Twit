#include <string>
using namespace std;

class LanguageSetting {
public:
	std::wstring GetLanguageW(TCHAR *key, TCHAR *name);
	std::string GetLanguageA(char *key, char *name);
};