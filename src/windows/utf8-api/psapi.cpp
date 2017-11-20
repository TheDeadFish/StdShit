#include "stdshit.h"
#include <Psapi.h>

cstrW getModuleFileNameExW(HANDLE a, HMODULE b){ WCHAR buff[MAX_PATH];
	DWORD len = GetModuleFileNameExW(a, b, buff, MAX_PATH);
	if(!len) return {0,0}; return xstrdup(buff); }
	
cstr getModuleFileNameEx(HANDLE a, HMODULE b){	WCHAR buff[MAX_PATH];
	DWORD len = GetModuleFileNameExW(a, b, buff, MAX_PATH);
	if(!len) return {0,0}; return utf816_dup(buff); }
