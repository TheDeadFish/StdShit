

// command line escaping
int __stdcall cmd_escape_len(NCCH* src, size_t len, int flags);
NCHR* REGCALL(1) cmd_escape(NCHR* dst, NCCH* src, size_t len, int flags);
int sysfmt(NCCH* fmt, ...);

// getFullPathName, getNtPathName
NCSTR WINAPI getFullPath(NCCH* str); NCSTR REGCALL(2) getFullPath(NCSTR str);
NCSTR WINAPI getFullPathF(NCCH* str); NCSTR REGCALL(2) getFullPathF(NCSTR str);
cstrW WINAPI getNtPathName(NCCH* s); cstrW REGCALL(2) getNtPathName(NCSTR s);

// WIN32 file functions
HANDLE WINAPI createFile(NCCH*,DWORD,DWORD,
	LPSECURITY_ATTRIBUTES,DWORD,DWORD,HANDLE);
DWORD WINAPI getFileAttributes(NCCH*);
BOOL WINAPI createDirectory(NCCH*,LPSECURITY_ATTRIBUTES);
BOOL WINAPI copyFile(NCCH*,NCCH*,BOOL);
BOOL WINAPI moveFile(NCCH*,NCCH*);
BOOL WINAPI moveFileEx(NCCH*,NCCH*,DWORD);
BOOL WINAPI deleteFile(NCCH*);
BOOL WINAPI setFileAttributes(NCCH*,DWORD);

NCSTR WINAPI NWNM(getWindowText)(HWND); 
NCSTR WINAPI NWNM(getWindowText2)(HWND); 
NCSTR WINAPI NWNM(getDlgItemText)(HWND,int); 
NCSTR WINAPI NWNM(getDlgItemText2)(HWND,int);

NCSTR WINAPI NWNM(getModuleFileNameEx)(HANDLE, HMODULE);
NCSTR WINAPI NWNM(getModuleFileName)(HMODULE);
NCSTR WINAPI NWNM(getProgramDir)(void);
NCSTR WINAPI NWNM(shGetFolderPath)(int nFolder);
NCSTR WINAPI getEnvironmentVariable(NCCH* name);
NCSTR WINAPI expandEnvironmentStrings(NCCH* env);

int REGCALL(1) findFirstFile(HANDLE&, NCCH*, 
	NWIF(WIN32_FIND_DATAW,WIN32_FIND_DATAU)*);
int WINAPI findNextFile(HANDLE,
	NWIF(WIN32_FIND_DATAW,WIN32_FIND_DATAU)*);

#undef NWIDE
