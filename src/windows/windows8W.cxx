#include "stdshit.h"
#define NWIDE NWIDE2

NCHR* WINAPI cmdfmt(VaArgFwd<NCCH*> va) {
	NCHR* b = xMalloc(xstrfmt_len(va)+2);
	NCHR* e = xstrfmt_fill(b+1, va); 
	NWIF(RI,RW)(e) = '"'; *b = '"'; return b; }	
int sysfmt(NCCH* fmt, ...) { VA_ARG_FWD(fmt);
	return system(xstr_(cmdfmt(va)));  }
	
// WIN32 file functions
HANDLE WINAPI createFile(NCCH* a,DWORD b,DWORD c,
	LPSECURITY_ATTRIBUTES d,DWORD e,DWORD f,HANDLE g) 
{ 	return CreateFileW(getNtPathNameX(a), b, c, d, e, f, g); }
DWORD WINAPI getFileAttributes(NCCH* name) {
	return GetFileAttributesW(getNtPathNameX(name)); }
BOOL WINAPI createDirectory(NCCH* a, LPSECURITY_ATTRIBUTES b) {
	return CreateDirectoryW(getNtPathNameX(a), b); }
BOOL WINAPI copyFile(NCCH* a, NCCH* b, BOOL c) { return 
	CopyFileW(getNtPathNameX(a), getNtPathNameX(b), c); }
BOOL WINAPI moveFile(NCCH* a, NCCH* b) { return 
	MoveFileW(getNtPathNameX(a), getNtPathNameX(b)); }
BOOL WINAPI moveFileEx(NCCH* a, NCCH* b, DWORD c) { return 
	MoveFileExW(getNtPathNameX(a), getNtPathNameX(b), c); }
BOOL WINAPI deleteFile(NCCH* a) { return DeleteFileW(getNtPathNameX(a)); }
BOOL WINAPI setFileAttributes(NCCH* a, DWORD b) { return 
	SetFileAttributesW(getNtPathNameX(a), b); }
	
NCSTR WINAPI NWNM(getDlgItemText)(HWND h, int i) { 
	return NWNM(getWindowText)(GetDlgItem(h, i)); }
NCSTR WINAPI NWNM(getDlgItemText2)(HWND h, int i) { 
	return NWNM(getWindowText2)(GetDlgItem(h, i)); }
	
	
NCSTR NWNM(getModuleFileName)(HMODULE hModule) { WCHAR buff[MAX_PATH];
	DWORD len = GetModuleFileNameW(hModule, buff, MAX_PATH); if(!len) 
	return {0,0}; return NWIF(xstrdup(buff,len),utf816_dup(buff)); }
NCSTR NWNM(getProgramDir)(void) { return getPath0
	(NWNM(getModuleFileName)(NULL)); }
	
NCSTR WINAPI NWNM(shGetFolderPath)(int nFolder) { WCHAR buff[MAX_PATH]; 
	if(SHGetFolderPathW(0, nFolder, NULL, 0, buff)) 
	return {0,0}; return NWIF(xstrdup,utf816_dup)(buff);  }

bool WINAPI isFullPath(NCCH* path)
{
	if(isNull(path)) return false;
	if(path[1] == ':') return true;
	return isPathSep(path[0])
		&& isPathSep(path[1]);
}

#undef NWIDE
