
#include <shlobj.h>

#define isIHV(x) ((x) == INVALID_HANDLE_VALUE)

static inline void setTxtMode(FILE* fp, bool ena) {
	_setmode(fp->_file, ena ? 0x4000 : 0x8000); }
	
HMODULE _stdcall getModuleBase(void* ptr);

// command line escaping
enum { 
	ESC_LEAQUOT = 1, ESC_CPMODE = 2, ESC_SFEQUOT = 4, 
	ESC_ENVEXP = 32, ESC_ENTQUOT = 128 };
	
// getFullPathName, getNtPathName
cstrW REGCALL(3) getFullPath_(cstrW str_, int mode);
#define getNtPathNameX(s) xstr_(getNtPathName(s).data)
	
// utf8 various win32 functions
BOOL WINAPI setWindowText(HWND,cch*);
BOOL WINAPI setDlgItemText(HWND,int,cch*);
int WINAPI messageBox(HWND, cch*, cch*, UINT);
BOOL WINAPI createProcess(LPCSTR,LPCSTR,LPSECURITY_ATTRIBUTES,
	LPSECURITY_ATTRIBUTES,BOOL,DWORD,LPVOID,LPCSTR,
	LPSTARTUPINFOA,LPPROCESS_INFORMATION);
BOOL WINAPI setCurrentDirectory(cch*);

// UTF8/UTF16 conversion
cstr narrowFree(LPWSTR s);
ALWAYS_INLINE wxstr widen(cch* s) {
	return utf816_dup(s); }
cstrW ansi_to_wide_dup(cch* ansi, int len);

// wide api helpers
#define W32SARD_(l,g) WCHAR* ws; int sz = l+1; \
	ws = xMalloc (sz); return {ws, g};
#define W32SARD2_(l,g) WCHAR* ws=0; int sz = l; if(sz)\
	{ ws = xMalloc(sz+=1); sz = g; } return {ws,sz};

// UTF8 WIN32_FIND_DATA
struct WIN32_FIND_DATAU { 
	DWORD dwFileAttributes; FILETIME ftCreationTime; 
	FILETIME ftLastAccessTime; FILETIME ftLastWriteTime; 
	u64p4 nFileSize; DWORD reparseAttrib; 
	DWORD fnLength; char cFileName[MAX_PATH*3];
	void init(WIN32_FIND_DATAW* src); 
	cstr cStr() { return {cFileName, fnLength}; }
	bool isDir() { return dwFileAttributes & 0x10;}
};


#define NWIDE 0
#include "windows8W.h"
#define NWIDE 1
#include "windows8W.h"
