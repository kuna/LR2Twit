#pragma comment(lib, "twitCurl.lib")
#pragma comment(lib, "libcurl.lib")
#include "twitCurl.h"
#include "Log.h"
#include "shellapi.h"

#include <algorithm>
using namespace std;

class TwitProc {
private:
	Log *l;
	twitCurl twitterObj;
public:
	TwitProc(Log *_l);
	bool getToken(string id, string pass);
	bool loadToken();
	bool sendTwit(string msg);

	char accessToken[256];
	char accessTokenSecret[256];
	char customerKey[256];
	char customerSecret[256];

	// add
	int postTwitpic( std::string& sUser, std::string& sPass, std::string& sFileName, std::string& desc);
};

