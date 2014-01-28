#include "stdafx.h"
#include "TwitProc.h"

TwitProc::TwitProc(Log *_l) {
	l = _l;

	strcpy(customerKey, "v5AS0GWeiJ1ovbGHWY50w");
	strcpy(customerSecret, "TPI2qwpuLpj6Yj604zPoQMC0BaLwMGxHK6EYx8faPpc");
}

bool TwitProc::loadToken() {
    char tmpBuf[1024];
	
    /* OAuth flow begins */
    /* Step 0: Set OAuth related params. These are got by registering your app at twitter.com */
    twitterObj.getOAuth().setConsumerKey( std::string( customerKey ) );
    twitterObj.getOAuth().setConsumerSecret( std::string( customerSecret ) );

    /* Step 1: Check if we alredy have OAuth access token from a previous run */
    std::string myOAuthAccessTokenKey("");
    std::string myOAuthAccessTokenSecret("");
    std::ifstream oAuthTokenKeyIn;
    std::ifstream oAuthTokenSecretIn;

    oAuthTokenKeyIn.open( "twitterClient_token_key.txt" );
    oAuthTokenSecretIn.open( "twitterClient_token_secret.txt" );

    memset( tmpBuf, 0, 1024 );
    oAuthTokenKeyIn >> tmpBuf;
    myOAuthAccessTokenKey = tmpBuf;

    memset( tmpBuf, 0, 1024 );
    oAuthTokenSecretIn >> tmpBuf;
    myOAuthAccessTokenSecret = tmpBuf;

    oAuthTokenKeyIn.close();
    oAuthTokenSecretIn.close();

    if( myOAuthAccessTokenKey.size() && myOAuthAccessTokenSecret.size() )
    {
        /* If we already have these keys, then no need to go through auth again */
        printf( "\nUsing:\nKey: %s\nSecret: %s\n\n", myOAuthAccessTokenKey.c_str(), myOAuthAccessTokenSecret.c_str() );

        twitterObj.getOAuth().setOAuthTokenKey( myOAuthAccessTokenKey );
        twitterObj.getOAuth().setOAuthTokenSecret( myOAuthAccessTokenSecret );

		// copy key
		strcpy(accessToken, myOAuthAccessTokenKey.c_str() );
		strcpy(accessTokenSecret, myOAuthAccessTokenSecret.c_str() );
	} else {
		return false; 
	}

	return true;
}

bool TwitProc::getToken(string id, string pass) {
	// 기존 토큰 삭제
	DeleteFileA("twitterClient_token_key.txt");
	DeleteFileA("twitterClient_token_secret.txt");

    std::string tmpStr;
    std::string replyMsg;
    char tmpBuf[1024];

    /* Set twitter username and password */
    twitterObj.setTwitterUsername( id );
    twitterObj.setTwitterPassword( pass );

    /* OAuth flow begins */
    /* Step 0: Set OAuth related params. These are got by registering your app at twitter.com */
    twitterObj.getOAuth().setConsumerKey( std::string( customerKey ) );
    twitterObj.getOAuth().setConsumerSecret( std::string( customerSecret ) );

    /* Step 1: Check if we alredy have OAuth access token from a previous run */
    std::string myOAuthAccessTokenKey("");
    std::string myOAuthAccessTokenSecret("");

	/* Step 2: Get request token key and secret */
	std::string authUrl;
	twitterObj.oAuthRequestToken( authUrl );

	/* Step 3: Get PIN  */
	if (MessageBox(NULL, L"[예]를 누르면 계정을 자동으로 인증합니다(recommended).\n[아니오]를 누르면 계정을 수동으로 인증합니다.", L"", MB_YESNO | MB_ICONINFORMATION) == IDYES) {
		// auto authorizez
		if (!twitterObj.oAuthHandlePIN( authUrl )) {
			MessageBox(NULL, L"Token 가져오기에 실패했습니다.", L"", MB_ICONEXCLAMATION);
			return false;
		}
	} else {
		// user auth
		memset( tmpBuf, 0, 1024 );
		ShellExecuteA(NULL, "Open", authUrl.c_str(), NULL, NULL, SW_SHOW);
		AllocConsole();
		freopen("CONOUT$", "wt", stdout);
		freopen("CONIN$", "r", stdin);
		printf("From this site:%s\nEnter PIN :\n", authUrl.c_str());
		gets( tmpBuf );
		FreeConsole();
		tmpStr = tmpBuf;
		twitterObj.getOAuth().setOAuthPin( tmpStr );
	}

	/* Step 4: Exchange request token with access token */
	if (!twitterObj.oAuthAccessToken()) {
		MessageBox(NULL, L"Token 가져오기에 실패했습니다.", L"", MB_ICONEXCLAMATION);
		return false;
	}

	/* Step 5: Now, save this access token key and secret for future use without PIN */
	twitterObj.getOAuth().getOAuthTokenKey( myOAuthAccessTokenKey );
	twitterObj.getOAuth().getOAuthTokenSecret( myOAuthAccessTokenSecret );

	/* Step 6: Save these keys in a file or wherever */
	std::ofstream oAuthTokenKeyOut;
	std::ofstream oAuthTokenSecretOut;

	oAuthTokenKeyOut.open( "twitterClient_token_key.txt" );
	oAuthTokenSecretOut.open( "twitterClient_token_secret.txt" );

	oAuthTokenKeyOut.clear();
	oAuthTokenSecretOut.clear();

	oAuthTokenKeyOut << myOAuthAccessTokenKey.c_str();
	oAuthTokenSecretOut << myOAuthAccessTokenSecret.c_str();

	oAuthTokenKeyOut.close();
	oAuthTokenSecretOut.close();
	/* OAuth flow ends */

	// copy key
	strcpy(accessToken, myOAuthAccessTokenKey.c_str() );
	strcpy(accessTokenSecret, myOAuthAccessTokenSecret.c_str() );

	return true;
}

bool TwitProc::sendTwit(string msg)
{
	if (msg.length() > 140) {
		msg = msg.substr(0, 140);
	}

	if (twitterObj.statusUpdate( msg )) {
		string replyMsg;
		twitterObj.getLastWebResponse( replyMsg );
		l->writeLogLine( L"Twit", replyMsg.c_str() );
		return true;
	} else {
		string replyMsg;
		twitterObj.getLastCurlError( replyMsg );
		l->writeLogLine( L"Twit Error", replyMsg.c_str() );
		return false;
	}
}

bool TwitProc::sendMediaTwit(string msg, char *data, int size)
{
	if (msg.length() > 117) { // include image url
		msg = msg.substr(0, 117);
	}

	if (twitterObj.uploadPictureRaw( data, size, msg )) {
		string replyMsg;
		twitterObj.getLastWebResponse( replyMsg );
		l->writeLogLine( L"MediaTwit", replyMsg.c_str() );
		return true;
	} else {
		string replyMsg;
		twitterObj.getLastCurlError( replyMsg );
		l->writeLogLine( L"MediaTwit Error", replyMsg.c_str() );
		return false;
	}
}

size_t write_data(void *buffer, size_t size, size_t nmemb, void* userp)
{
 
	std::stringstream strmResponse;
	size_t nReal = size * nmemb;
	strmResponse << std::string((char*)buffer, size*nmemb);
	std::string sLine("");
	while (getline(strmResponse, sLine)) {
		std::cout << sLine.c_str() << std::endl;
	}
	return nReal;
}

#define END_POINT "http://twitpic.com/api/uploadAndPost"
#define EXPECT_ARGS 4
int TwitProc::postTwitpic(std::string& sUser, std::string& sPass, std::string& sFileName, std::string& desc)
{
	int result = 0;
	CURL* hCurl = NULL;
	CURLcode hResult;
	//Curl for form data
	struct curl_httppost *post = NULL;
	struct curl_httppost *last = NULL;
	try{
		//Initialize curl, just don't let easy_init to do it for you
		curl_global_init(CURL_GLOBAL_ALL);
		//Handle to the curl
		hCurl = curl_easy_init();
		if(NULL == hCurl) {
			throw false;
		}
		//Construct the form
		curl_formadd(&post, &last, CURLFORM_COPYNAME, "username", CURLFORM_COPYCONTENTS, sUser.c_str(), CURLFORM_END);
		curl_formadd(&post, &last, CURLFORM_COPYNAME, "password", CURLFORM_COPYCONTENTS, sPass.c_str(), CURLFORM_END);
		curl_formadd(&post, &last, CURLFORM_COPYNAME, "media", CURLFORM_FILE, sFileName.c_str(), CURLFORM_END);
		curl_formadd(&post, &last, CURLFORM_COPYNAME, "message", CURLFORM_COPYCONTENTS, desc.c_str(), CURLFORM_END);
		//Specify the API Endpoint
		hResult = curl_easy_setopt(hCurl,CURLOPT_URL, END_POINT);
		//Specify the HTTP Method
		hResult = curl_easy_setopt(hCurl, CURLOPT_HTTPPOST, post);
		//Post Away !!!
		hResult = curl_easy_perform(hCurl);
		if(hResult != CURLE_OK){
			std::cout << "Cannot find the twitpic site " << std::endl;
			throw false;
		}
	}
	catch (...) {
		result = -1;
	}
	curl_formfree(post);
    curl_easy_cleanup(hCurl);
	curl_global_cleanup();
    return result;
}
