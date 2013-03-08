
#include "Markup.h"
#include "Log.h"
#define MAX_LIST 2000

class Data_Level {
private:
	// song MAX list: 5000
	// song title MAX str len: 80 (UTF16-LE)
	TCHAR filename[256];

	TCHAR m_Hash[MAX_LIST][40];
	int m_lv[MAX_LIST];
	int m_cnt;
	CMarkup xml;
	Log *l;
public:
	Data_Level();

	BOOL LoadFile(TCHAR *filename);
	BOOL LoadFromInternet();
	int getInsaneLevel(TCHAR *hash);
	void setLog(Log *_l);
};

#define _INSANE_URL "http://www.dream-pro.info/~lavalse/LR2IR/getinsanelist.cgi"
