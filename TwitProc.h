#include "twitCurl.h"
#pragma comment(lib, "twitCurl.lib")
#pragma comment(lib, "libcurl.lib")
#include "iconv.h"
#pragma comment(lib, "iconv.lib")
#include <algorithm>

#include "Log.h"

using namespace std;

class TwitProc {
	Log *l;
private:
	twitCurl twitterObj;
public:
	TwitProc(Log *_l);
	bool getToken(string id, string pass);
	bool loadToken();
	bool sendTwit(string msg);

	// add
	int postTwitpic( std::string& sUser, std::string& sPass, std::string& sFileName, std::string& desc);
};

