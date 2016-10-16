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

// getFullPathName utf8
cstr __stdcall getFullPath(cch*str, int len, int f = 0);
ALWAYS_INLINE cstr getFullPath(cch* s, int f = 0) {
	return getFullPath(s, -1, f); }
ALWAYS_INLINE cstr getFullPath(cstr s, int f = 0) {
	return getFullPath(s.data, s.slen, f); }

// getNtPathName, (support > 260 filenames)
cstrW __stdcall getNtPathName_(cch* s, int l, int f);
#define GETNTPNM_(s,l,f,so,bo) so = getNtPathName_(\
	s, l, f); asm volatile("" : "+A"(so), "=c"(bo));
#define FNWIDEN(x,s) cstrW MCAT(cs,x); WCHAR* MCAT(bs,x); GETNTPNM_\
	(s,-1,0,MCAT(cs,x),MCAT(bs,x)); SCOPE_EXIT(free(MCAT(bs,x)));

// UTF8/UTF16 conversion
ALWAYS_INLINE Cstr narrow(LPWSTR s) { return utf816_dup(s); }
ALWAYS_INLINE Cstr narrow(LPCWSTR s, int l) { return utf816_dup(s, l); }
ALWAYS_INLINE CstrW widen(cch* s) { return utf816_dup(s); }
ALWAYS_INLINE CstrW widen(cch* s, int l) { return utf816_dup(s, l); }

// path test functions
static inline bool isRelPath(cch* str) { bool ret; asm(
	"call _isRelPath0;" : "=c"(ret) : "a"(str)); return ret; }
static inline bool isRelPath(cstr str) { bool ret; asm(
	"call _isRelPath;" : "=c"(ret) : "A"(str)); return ret; }

HMODULE _stdcall getModuleBase(void* ptr);
