static inline void setTxtMode(FILE* fp, bool ena) {
	_setmode(fp->_file, ena ? 0x4000 : 0x8000); }

// command line escaping
enum { 
	ESC_LEAQUOT = 1, ESC_CPMODE = 2, ESC_SFEQUOT = 4, 
	ESC_FIXED = 8,  ESC_ENVEXP = 32, ESC_ENTQUOT = 128 };
int __stdcall cmd_escape_len(const char* src, size_t len, int flags);
int __stdcall cmd_escape_len(const wchar_t* src, size_t len, int flags);
char* REGCALL(1) cmd_escape(char* dst, const char* src, size_t len, int flags);
wchar_t* REGCALL(1) cmd_escape(wchar_t* dst, 
	const wchar_t* src, size_t len, int flags);
int sysfmt(const char* fmt, ...);

// UTF8/UTF16 conversion
static inline CstrW widen(cch* s) { return utf816_dup(s); }
static inline CstrW widen(cch* s, int l) { return utf816_dup(s, l); }
static inline Cstr narrow(LPWSTR s) { return utf816_dup(s); }
static inline Cstr narrow(LPCWSTR s, int l) { return utf816_dup(s, l); }

HMODULE _stdcall getModuleBase(void* ptr);
