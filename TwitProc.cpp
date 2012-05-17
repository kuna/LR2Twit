#include "stdafx.h"
#include "TwitProc.h"


bool cp949_to_utf8(const string& in, string& out)
{
	iconv_t cd;
	bool ret = false;
	const char* pszIn;
	char* pszOut, *pos;
	size_t inLen, outLen;
	pszOut=NULL;
	
	cd = iconv_open("UTF-8",opt_encode);
	if(cd == (iconv_t)(-1))
	{
		return false; 
	}
	
	inLen = in.length();
	pszIn = in.c_str();
	
	outLen = (inLen+1) * 2;
	pos = pszOut = (char*)calloc(outLen, sizeof(char));
	if(!pszOut) goto clean;
	
	if(iconv(cd, &pszIn, &inLen, &pos, &outLen)==-1)
		goto clean;
	out = pszOut;
	ret = true;
clean:
	iconv_close(cd);
	if(pszOut) free(pszOut);
	return ret;
}


/*
TwitProc::TwitProc() {
	strcpy(customerKey, "v5AS0GWeiJ1ovbGHWY50w");
	strcpy(customerSecret, "TPI2qwpuLpj6Yj604zPoQMC0BaLwMGxHK6EYx8faPpc"); 

	strcpy(request_token_url, "http://api.twitter.com/oauth/request_token");
	strcpy(authorize_url, "http://api.twitter.com/oauth/authorize");
	strcpy(access_token_url, "http://api.twitter.com/oauth/access_token");

}*/

// http://code.google.com/p/twitcurl/source/browse/trunk/twitterClient/twitterClient.cpp
bool TwitProc::loadToken() {
    char tmpBuf[1024];
	
    /* OAuth flow begins */
    /* Step 0: Set OAuth related params. These are got by registering your app at twitter.com */
    twitterObj.getOAuth().setConsumerKey( std::string( "v5AS0GWeiJ1ovbGHWY50w" ) );
    twitterObj.getOAuth().setConsumerSecret( std::string( "TPI2qwpuLpj6Yj604zPoQMC0BaLwMGxHK6EYx8faPpc" ) );

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
	} else {
		return false; 
	}

	return true;
}

bool TwitProc::getToken(string id, string pass) {
    std::string tmpStr;
    std::string replyMsg;
    char tmpBuf[1024];

    /* Set twitter username and password */
    twitterObj.setTwitterUsername( id );
    twitterObj.setTwitterPassword( pass );

    /* OAuth flow begins */
    /* Step 0: Set OAuth related params. These are got by registering your app at twitter.com */
    twitterObj.getOAuth().setConsumerKey( std::string( "v5AS0GWeiJ1ovbGHWY50w" ) );
    twitterObj.getOAuth().setConsumerSecret( std::string( "TPI2qwpuLpj6Yj604zPoQMC0BaLwMGxHK6EYx8faPpc" ) );

    /* Step 1: Check if we alredy have OAuth access token from a previous run */
    std::string myOAuthAccessTokenKey("");
    std::string myOAuthAccessTokenSecret("");
    std::ifstream oAuthTokenKeyIn;
    std::ifstream oAuthTokenSecretIn;

        /* Step 2: Get request token key and secret */
        std::string authUrl;
        twitterObj.oAuthRequestToken( authUrl );

        /* Step 3: Get PIN  */
        memset( tmpBuf, 0, 1024 );
        /*printf( "\nDo you want to visit twitter.com for PIN (0 for no; 1 for yes): " );
        gets( tmpBuf );
        tmpStr = tmpBuf;
        if( std::string::npos != tmpStr.find( "1" ) )
        {*/
            /* Ask user to visit twitter.com auth page and get PIN */
          /*  memset( tmpBuf, 0, 1024 );
            MessageBox(NULL, L"\nPlease visit this link in web browser and authorize this application.", L"", NULL );
			WinExec(authUrl.c_str(), NULL);
            //printf( "\nEnter the PIN provided by twitter: " );
            gets( tmpBuf );
            tmpStr = tmpBuf;
            twitterObj.getOAuth().setOAuthPin( tmpStr );
       /* }
        else
        {*/
            /* Else, pass auth url to twitCurl and get it via twitCurl PIN handling */

            bool res = twitterObj.oAuthHandlePIN( authUrl );
			if (!res) return false;
        //}*/

        /* Step 4: Exchange request token with access token */
        twitterObj.oAuthAccessToken();

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


	return true;
}

bool TwitProc::sendTwit(string msg)
{
	string conv_msg;
	cp949_to_utf8(msg, conv_msg);

	if (twitterObj.statusUpdate( conv_msg )) {
		string replyMsg;
		twitterObj.getLastWebResponse( replyMsg );
		
		OutputDebugStringA( replyMsg.c_str() );
		return true;
	} else {
		string replyMsg;
		twitterObj.getLastCurlError( replyMsg );
		return false;
	}
	// wprintf(L"\r\nUpdate Result:\r\n%s\r\n", submitResult.c_str());
}