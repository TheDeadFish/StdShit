
#include <shlobj.h>

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
cstrW __stdcall getNtPathName_(cch* s);
cstrW __stdcall getNtPathName_(cch* s, int l, int f);
#define GETNTPNM_(so,bo, ...) so = getNtPathName_(\
	__VA_ARGS__); asm volatile("" : "+A"(so), "=c"(bo));
#define FNWIDEN(x,s) cstrW MCAT(cs,x); WCHAR* MCAT(bs,x); GETNTPNM_\
	(MCAT(cs,x),MCAT(bs,x),s); SCOPE_EXIT(free(MCAT(bs,x)));
	
// utf8 wide apis, output
HANDLE WINAPI createFile(LPCSTR,DWORD,DWORD,
	LPSECURITY_ATTRIBUTES,DWORD,DWORD,HANDLE);
BOOL WINAPI setWindowText(HWND,cch*);
BOOL WINAPI setDlgItemText(HWND,int,cch*);
DWORD WINAPI getFileAttributes(cch*);

// utf8 wide apis, intput
cstr __stdcall getModuleFileName(HMODULE hModule);
cstr __stdcall getProgramDir(void);
cstr WINAPI getWindowText(HWND); 
cstr WINAPI getDlgItemText(HWND,int);
cstr WINAPI shGetFolderPath(int nFolder);

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



// find file functions
struct WIN32_FIND_DATAU { 
	DWORD dwFileAttributes; FILETIME ftCreationTime; 
	FILETIME ftLastAccessTime; FILETIME ftLastWriteTime; 
	u64p4 nFileSize; DWORD reparseAttrib; 
	DWORD fnLength; char cFileName[MAX_PATH*3];
	
	void init(WIN32_FIND_DATAW* src); 
	cstr cStr() { return {cFileName, fnLength}; }
	bool isDir() { return dwFileAttributes & 0x10;}
	bool isDot() { return RW(cFileName) == 0x2E;}
	bool isDot2() { return RI(cFileName) == 0x2E2E;}
};
DEF_RETPAIR(findFirstFile_t, int, status, HANDLE, hFind);
findFirstFile_t WINAPI findFirstFile(cch* fileName, WIN32_FIND_DATAU* fd);
int WINAPI findNextFile(HANDLE hFind, WIN32_FIND_DATAU* fd);
