static inline void setTxtMode(FILE* fp, bool ena) {
	_setmode(fp->_file, ena ? 0x4000 : 0x8000); }

// command line escaping
enum { ESC_LEAQUOT = 2, ESC_SFEQUOT = 4,
	ESC_ENVEXP = 8, ESC_CPMODE = 16, ESC_ENTQUOT = 128 };
int __stdcall cmd_escape_len(const char* src, char* end, int flags);
int __stdcall cmd_escape_len(const wchar_t* src, wchar_t* end, int flags);
char* REGCALL(1) cmd_escape(char* dst, const char* src, char* end, int flags);
wchar_t* REGCALL(1) cmd_escape(wchar_t* dst, 
	const wchar_t* src, wchar_t* end, int flags);
