#include "twitCurl.h"
#pragma comment(lib, "twitCurl.lib")
#include "iconv.h"
#pragma comment(lib, "iconv.lib")
#include <algorithm>
using namespace std;

class TwitProc {
private:
	twitCurl twitterObj;

public:
	bool getToken(string id, string pass);
	bool loadToken();
	bool sendTwit(string msg);
};

