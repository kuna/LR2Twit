#include <string>

class Twitpic {
private:
	char customerkey[256];
	char customersecretkey[256];
	char accesstoken[256];
	char accesstokensecret[256];
	void getLR2PicPath();

	HANDLE hmm_LR2ScreenCapture;
	int *p_LR2ScreenCapture;
	HANDLE hmm_LR2PicPath;
	char *p_LR2PicPath;
	void releaseSharedMemory();
	void setSharedMemory();
public:
	Twitpic();
	~Twitpic();
	char LR2PicPath[256];
	void set_account(char *ckey, char *cskey, char *atoken, char *astoken);
	std::string upload_pic(char *filepath, char *comment);
	std::string upload_pic(char *filepath);
	std::string upload_status(char *comment);
	std::string getBearerToken();
	BOOL CaptureScreen();
};

struct MemoryStruct {
	char *data;
	size_t size; //< bytes remaining (r), bytes accumulated (w)

	size_t start_size; //< only used with ..AndCall()
	void (*callback)(void*,int,size_t,size_t); //< only used with ..AndCall()
	void *callback_data; //< only used with ..AndCall()
};

static size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data) {
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)data;

	mem->data = (char *)realloc(mem->data, mem->size + realsize + 1);
	if (mem->data) {
		memcpy(&(mem->data[mem->size]), ptr, realsize);
		mem->size += realsize;
		mem->data[mem->size] = 0;
	}
	return realsize;
}
