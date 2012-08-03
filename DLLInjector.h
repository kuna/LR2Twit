

class DLLInjector {
private:
	DWORD PID;
	HMODULE g_hModule;
	DWORD GET_PID(char *win_Class, char *win_Title);
	BOOL DLL_Injection(int PID, char* PATH);
public:
	BOOL Inject(char *win_Class, char *win_Title, char *dll_Path);
	BOOL Eject();
};