
#include <shlobj.h>

#define isIHV(x) ((x) == INVALID_HANDLE_VALUE)

static inline void setTxtMode(FILE* fp, bool ena) {
	_setmode(fp->_file, ena ? 0x4000 : 0x8000); }

// command line escaping
enum { 
	ESC_LEAQUOT = 1, ESC_CPMODE = 2, ESC_SFEQUOT = 4, 
	ESC_ENVEXP = 32, ESC_ENTQUOT = 128 };
int __stdcall cmd_escape_len(const char* src, size_t len, int flags);
int __stdcall cmd_escape_len(const wchar_t* src, size_t len, int flags);
char* REGCALL(1) cmd_escape(char* dst, const char* src, size_t len, int flags);
wchar_t* REGCALL(1) cmd_escape(wchar_t* dst, 
	const wchar_t* src, size_t len, int flags);
int sysfmt(const char* fmt, ...);

// getFullPathName utf8
cstr __stdcall getFullPath(cch*str, int len, int f);
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
BOOL WINAPI createDirectory(cch*, LPSECURITY_ATTRIBUTES);
BOOL WINAPI copyFile(cch*, cch*, BOOL);
BOOL WINAPI moveFile(cch* a, cch* b);
BOOL WINAPI moveFileEx(cch* a, cch* b, DWORD c);
BOOL WINAPI deleteFile(cch*);
BOOL WINAPI setFileAttributes(cch*, DWORD);
int WINAPI messageBox(HWND, cch*, cch*, UINT);

// utf8 wide apis, intput
cstr __stdcall getModuleFileNameEx(
	HANDLE hProcess, HMODULE hModule);
cstr __stdcall getModuleFileName(HMODULE hModule);
cstr __stdcall getProgramDir(void);
cstr WINAPI getWindowText(HWND); 
cstr WINAPI getWindowText2(HWND); 
cstr WINAPI getDlgItemText(HWND,int); 
cstr WINAPI getDlgItemText2(HWND,int);
cstr WINAPI shGetFolderPath(int nFolder);
cstr getEnvironmentVariable(cch* name);
cstr expandEnvironmentStrings(cch* env);

// UTF8/UTF16 conversion
cstr narrowFree(LPWSTR s);
ALWAYS_INLINE wxstr widen(cch* s) { return utf816_dup(s); }



/*ALWAYS_INLINE Cstr narrow(LPWSTR s) { return utf816_dup(s); }
ALWAYS_INLINE Cstr narrow(LPCWSTR s, int l) { return utf816_dup(s, l); }

ALWAYS_INLINE CstrW widen(cch* s, int l) { return utf816_dup(s, l); }*/

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
};

int REGCALL(1) findFirstFile(HANDLE& hFile,
	cch* fileName, WIN32_FIND_DATAU* fd);
int WINAPI findNextFile(HANDLE hFind, WIN32_FIND_DATAU* fd);

// wide api helpers
#define W32SARD_(l,g) WCHAR* ws; { int sz = l+1; \
	ws = xMalloc (sz); g; } return narrowFree(ws);
#define W32SARD2_(l,g) WCHAR* ws=0; { if(int sz = l){\
	ws = xMalloc(sz+=1); g; }} return narrowFree(ws);
	
cstrW ansi_to_wide_dup(cch* ansi, int len);



BOOL WINAPI createProcess(
	LPCSTR lpApplicationName,
	LPCSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCSTR lpCurrentDirectory,
	LPSTARTUPINFOA lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation
);
