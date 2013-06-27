#ifndef _TWITCURL_H_
#define _TWITCURL_H_

#include <string>
#include <sstream>
#include <cstring>
#include <vector>
#include "oauthlib.h"
#include "curl/curl.h"

namespace twitCurlTypes
{
    typedef enum _eTwitCurlApiFormatType
    {
        eTwitCurlApiFormatXml = 0,
        eTwitCurlApiFormatJson,
        eTwitCurlApiFormatMax
    } eTwitCurlApiFormatType;

    typedef enum _eTwitCurlProtocolType
    {
        eTwitCurlProtocolHttp = 0,
        eTwitCurlProtocolHttps,
        eTwitCurlProtocolMax
    } eTwitCurlProtocolType;
};

/* Default values used in twitcurl */
namespace twitCurlDefaults
{
    /* Constants */
    const int TWITCURL_DEFAULT_BUFFSIZE = 1024;
    const std::string TWITCURL_COLON = ":";
    const char TWITCURL_EOS = '\0';
    const unsigned int MAX_TIMELINE_TWEET_COUNT = 200;

    /* Miscellaneous data used to build twitter URLs*/
    const std::string TWITCURL_STATUSSTRING = "status=";
    const std::string TWITCURL_TEXTSTRING = "text=";
    const std::string TWITCURL_QUERYSTRING = "query=";
    const std::string TWITCURL_SEARCHQUERYSTRING = "q=";
    const std::string TWITCURL_SCREENNAME = "screen_name=";
    const std::string TWITCURL_USERID = "user_id=";
    const std::string TWITCURL_EXTENSIONFORMATS[2] = { "",
                                                       ".json"
                                                     };
    const std::string TWITCURL_PROTOCOLS[2] =        { "",
                                                       "https://"
                                                     };
    const std::string TWITCURL_TARGETSCREENNAME = "target_screen_name=";
    const std::string TWITCURL_TARGETUSERID = "target_id=";
    const std::string TWITCURL_SINCEID = "since_id=";
    const std::string TWITCURL_TRIMUSER = "trim_user=true";
    const std::string TWITCURL_INCRETWEETS = "include_rts=true";
    const std::string TWITCURL_COUNT = "count=";
    const std::string TWITCURL_NEXT_CURSOR = "cursor=";
    const std::string TWITCURL_SKIP_STATUS = "skip_status=";
    const std::string TWITCURL_INCLUDE_ENTITIES = "include_entities=";
    const std::string TWITCURL_STRINGIFY_IDS = "stringify_ids=";

    /* URL separators */
    const std::string TWITCURL_URL_SEP_AMP = "&";
    const std::string TWITCURL_URL_SEP_QUES = "?";
};

/* Default twitter URLs */
namespace twitterDefaults
{

    /* Search URLs */
    const std::string TWITCURL_SEARCH_URL = "search.twitter.com/search";

    /* Status URLs */
    const std::string TWITCURL_STATUSUPDATE_URL = "http://api.twitter.com/1.1/statuses/update.json";
    const std::string TWITCURL_STATUSSHOW_URL = "http://api.twitter.com/1.1/statuses/show.json";
    const std::string TWITCURL_STATUDESTROY_URL = "http://api.twitter.com/1.1/statuses/destroy.json";
    const std::string TWITCURL_RETWEET_URL = "http://api.twitter.com/1.1/statuses/retweet.json";

    /* Timeline URLs */
    const std::string TWITCURL_HOME_TIMELINE_URL = "http://api.twitter.com/1.1/statuses/home_timeline.json";
    const std::string TWITCURL_PUBLIC_TIMELINE_URL = "http://api.twitter.com/1.1/statuses/public_timeline.json";
    const std::string TWITCURL_FEATURED_USERS_URL = "http://api.twitter.com/1.1/statuses/featured.json";
    const std::string TWITCURL_FRIENDS_TIMELINE_URL = "http://api.twitter.com/1.1/statuses/friends_timeline.json";
    const std::string TWITCURL_MENTIONS_URL = "http://api.twitter.com/1.1/statuses/mentions.json";
    const std::string TWITCURL_USERTIMELINE_URL = "http://api.twitter.com/1.1/statuses/user_timeline.json";

    /* Users URLs */
    const std::string TWITCURL_LOOKUPUSERS_URL = "http://api.twitter.com/1.1/users/lookup.json";
    const std::string TWITCURL_SHOWUSERS_URL = "http://api.twitter.com/1.1/users/show.json";
    const std::string TWITCURL_SHOWFRIENDS_URL = "http://api.twitter.com/1.1/statuses/friends.json";
    const std::string TWITCURL_SHOWFOLLOWERS_URL = "http://api.twitter.com/1.1/statuses/followers.json";

    /* Direct messages URLs */
    const std::string TWITCURL_DIRECTMESSAGES_URL = "http://api.twitter.com/1.1/direct_messages.json";
    const std::string TWITCURL_DIRECTMESSAGENEW_URL = "http://api.twitter.com/1.1/direct_messages/new.json";
    const std::string TWITCURL_DIRECTMESSAGESSENT_URL = "http://api.twitter.com/1.1/direct_messages/sent.json";
    const std::string TWITCURL_DIRECTMESSAGEDESTROY_URL = "http://api.twitter.com/1.1/direct_messages/destroy.json";

    /* Friendships URLs */
    const std::string TWITCURL_FRIENDSHIPSCREATE_URL = "http://api.twitter.com/1.1/friendships/create.json";
    const std::string TWITCURL_FRIENDSHIPSDESTROY_URL = "http://api.twitter.com/1.1/friendships/destroy.json";
    const std::string TWITCURL_FRIENDSHIPSSHOW_URL = "http://api.twitter.com/1.1/friendships/show.json";

    /* Social graphs URLs */
    const std::string TWITCURL_FRIENDSIDS_URL = "http://api.twitter.com/1.1/friends/ids.json";
    const std::string TWITCURL_FOLLOWERSIDS_URL = "http://api.twitter.com/1.1/followers/ids.json";

    /* Account URLs */
    const std::string TWITCURL_ACCOUNTRATELIMIT_URL = "http://api.twitter.com/1.1/account/rate_limit_status.json";
    const std::string TWITCURL_ACCOUNTVERIFYCRED_URL = "http://api.twitter.com/1.1/account/verify_credentials.json";

    /* Favorites URLs */
    const std::string TWITCURL_FAVORITESGET_URL = "http://api.twitter.com/1.1/favorites.json";
    const std::string TWITCURL_FAVORITECREATE_URL = "http://api.twitter.com/1.1/favorites/create.json";
    const std::string TWITCURL_FAVORITEDESTROY_URL = "http://api.twitter.com/1.1/favorites/destroy.json";

    /* Block URLs */
    const std::string TWITCURL_BLOCKSCREATE_URL = "http://api.twitter.com/1.1/blocks/create.json";
    const std::string TWITCURL_BLOCKSDESTROY_URL = "http://api.twitter.com/1.1/blocks/destroy.json";
    const std::string TWITCURL_BLOCKSLIST_URL = "http://api.twitter.com/1.1/blocks/list.json";
    const std::string TWITCURL_BLOCKSIDS_URL = "http://api.twitter.com/1.1/blocks/ids.json";

    /* Saved Search URLs */
    const std::string TWITCURL_SAVEDSEARCHGET_URL = "http://api.twitter.com/1.1/saved_searches.json";
    const std::string TWITCURL_SAVEDSEARCHSHOW_URL = "http://api.twitter.com/1.1/saved_searches/show.json";
    const std::string TWITCURL_SAVEDSEARCHCREATE_URL = "http://api.twitter.com/1.1/saved_searches/create.json";
    const std::string TWITCURL_SAVEDSEARCHDESTROY_URL = "http://api.twitter.com/1.1/saved_searches/destroy.json";

    /* Trends URLs */
    const std::string TWITCURL_TRENDS_URL = "http://api.twitter.com/1.1/trends.json";
    const std::string TWITCURL_TRENDSDAILY_URL = "http://api.twitter.com/1.1/trends/daily.json";
    const std::string TWITCURL_TRENDSCURRENT_URL = "http://api.twitter.com/1.1/trends/current.json";
    const std::string TWITCURL_TRENDSWEEKLY_URL = "http://api.twitter.com/1.1/trends/weekly.json";
    const std::string TWITCURL_TRENDSAVAILABLE_URL = "http://api.twitter.com/1.1/trends/available.json";

};

/* twitCurl class */
class twitCurl
{
public:
    twitCurl();
    ~twitCurl();

    /* Twitter OAuth authorization methods */
    oAuth& getOAuth();
    bool oAuthRequestToken( std::string& authorizeUrl /* out */ );
    bool oAuthAccessToken();
    bool oAuthHandlePIN( const std::string& authorizeUrl /* in */ );

    /* Twitter login APIs, set once and forget */
    std::string& getTwitterUsername();
    std::string& getTwitterPassword();
    void setTwitterUsername( std::string& userName /* in */ );
    void setTwitterPassword( std::string& passWord /* in */ );

    /* Twitter API type */
    void setTwitterApiType( twitCurlTypes::eTwitCurlApiFormatType eType );
    twitCurlTypes::eTwitCurlApiFormatType getTwitterApiType();

    /* Twitter protocol type */
    void setTwitterProcotolType( twitCurlTypes::eTwitCurlProtocolType eType );
    twitCurlTypes::eTwitCurlProtocolType getTwitterProcotolType();

    /* Twitter search APIs */
    bool search( std::string& searchQuery /* in */ );

    /* Twitter status APIs */
    bool statusUpdate( std::string& newStatus /* in */ );
    bool statusShowById( std::string& statusId /* in */ );
    bool statusDestroyById( std::string& statusId /* in */ );
    bool retweetById( std::string& statusId /* in */ );

    /* Twitter timeline APIs */
    bool timelineHomeGet( std::string sinceId = ""  /* in */ );
    bool timelinePublicGet();
    bool timelineFriendsGet();
    bool timelineUserGet( bool trimUser /* in */, bool includeRetweets /* in */, unsigned int tweetCount /* in */, std::string userInfo = "" /* in */, bool isUserId = false /* in */ );
    bool featuredUsersGet();
    bool mentionsGet( std::string sinceId = "" /* in */ );

    /* Twitter user APIs */
    bool userLookup( std::vector<std::string> &userInfo /* in */,  bool isUserId = false /* in */ );
    bool userGet( std::string& userInfo /* in */, bool isUserId = false /* in */ );
    bool friendsGet( std::string userInfo = "" /* in */, bool isUserId = false /* in */ );
    bool followersGet( std::string userInfo = "" /* in */, bool isUserId = false /* in */ );

    /* Twitter direct message APIs */
    bool directMessageGet( std::string sinceId = "" /* in */ );
    bool directMessageSend( std::string& userInfo /* in */, std::string& dMsg /* in */, bool isUserId = false /* in */ );
    bool directMessageGetSent();
    bool directMessageDestroyById( std::string& dMsgId /* in */ );

    /* Twitter friendships APIs */
    bool friendshipCreate( std::string& userInfo /* in */, bool isUserId = false /* in */ );
    bool friendshipDestroy( std::string& userInfo /* in */, bool isUserId = false /* in */ );
    bool friendshipShow( std::string& userInfo /* in */, bool isUserId = false /* in */ );

    /* Twitter social graphs APIs */
    bool friendsIdsGet( std::string& nextCursor /* in */,
                        std::string& userInfo /* in */, bool isUserId = false /* in */ );
    bool followersIdsGet( std::string& nextCursor /* in */,
                          std::string& userInfo /* in */, bool isUserId = false /* in */ );

    /* Twitter account APIs */
    bool accountRateLimitGet();
    bool accountVerifyCredGet();

    /* Twitter favorites APIs */
    bool favoriteGet();
    bool favoriteCreate( std::string& statusId /* in */ );
    bool favoriteDestroy( std::string& statusId /* in */ );

    /* Twitter block APIs */
    bool blockCreate( std::string& userInfo /* in */ );
    bool blockDestroy( std::string& userInfo /* in */ );
    bool blockListGet( std::string& nextCursor /* in */,
                        bool includeEntities /* in */, bool skipStatus /* in */ );
    bool blockIdsGet( std::string& nextCursor /* in */, bool stringifyIds /* in */ );

    /* Twitter search APIs */
    bool savedSearchGet();
    bool savedSearchCreate( std::string& query /* in */ );
    bool savedSearchShow( std::string& searchId /* in */ );
    bool savedSearchDestroy( std::string& searchId /* in */ );

    /* Twitter trends APIs (JSON) */
    bool trendsGet();
    bool trendsDailyGet();
    bool trendsWeeklyGet();
    bool trendsCurrentGet();
    bool trendsAvailableGet();

    /* cURL APIs */
    bool isCurlInit();
    void getLastWebResponse( std::string& outWebResp /* out */ );
    void getLastCurlError( std::string& outErrResp /* out */);

    /* Internal cURL related methods */
    int saveLastWebResponse( char*& data, size_t size );

    /* cURL proxy APIs */
    std::string& getProxyServerIp();
    std::string& getProxyServerPort();
    std::string& getProxyUserName();
    std::string& getProxyPassword();
    void setProxyServerIp( std::string& proxyServerIp /* in */ );
    void setProxyServerPort( std::string& proxyServerPort /* in */ );
    void setProxyUserName( std::string& proxyUserName /* in */ );
    void setProxyPassword( std::string& proxyPassword /* in */ );
    
    /* Clones this object */
    twitCurl* clone();

private:
    /* cURL data */
    CURL* m_curlHandle;
    char m_errorBuffer[twitCurlDefaults::TWITCURL_DEFAULT_BUFFSIZE];
    std::string m_callbackData;

    /* cURL flags */
    bool m_curlProxyParamsSet;
    bool m_curlLoginParamsSet;
    bool m_curlCallbackParamsSet;

    /* cURL proxy data */
    std::string m_proxyServerIp;
    std::string m_proxyServerPort;
    std::string m_proxyUserName;
    std::string m_proxyPassword;

    /* Twitter data */
    std::string m_twitterUsername;
    std::string m_twitterPassword;

    /* Twitter API type */
    twitCurlTypes::eTwitCurlApiFormatType m_eApiFormatType;
    twitCurlTypes::eTwitCurlProtocolType m_eProtocolType;

    /* OAuth data */
    oAuth m_oAuth;

    /* Private methods */
    void clearCurlCallbackBuffers();
    void prepareCurlProxy();
    void prepareCurlCallback();
    void prepareCurlUserPass();
    void prepareStandardParams();
    bool performGet( const std::string& getUrl );
    bool performGetInternal( const std::string& getUrl,
                             const std::string& oAuthHttpHeader );
    bool performDelete( const std::string& deleteUrl );
    bool performPost( const std::string& postUrl, std::string dataStr = "" );

    /* Internal cURL related methods */
    static int curlCallback( char* data, size_t size, size_t nmemb, twitCurl* pTwitCurlObj );
};


/* Private functions */
void utilMakeCurlParams( std::string& outStr, std::string& inParam1, std::string& inParam2 );
void utilMakeUrlForUser( std::string& outUrl, const std::string& baseUrl, std::string& userInfo, bool isUserId );

#endif // _TWITCURL_H_
