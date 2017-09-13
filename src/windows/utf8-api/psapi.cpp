#include "stdshit.h"
#include <Psapi.h>

cstr getModuleFileNameEx(
	HANDLE a, HMODULE b)
{	WCHAR buff[MAX_PATH];
	if(!GetModuleFileNameExW(a, b, buff, MAX_PATH))
		return {0,0}; return utf816_dup(buff); }
