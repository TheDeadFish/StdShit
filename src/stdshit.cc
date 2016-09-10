#define _STDSHIT_CC_
#ifndef _STDSHIT_X_
#include "stdshit.h"
#include "math.cpp"
#include "memalloc.cpp"
#include "utfconv.cpp"

// tristi quod ad hunc, const strings
#define DEF_RDTEXT(name, text) \
	__attribute__((section(".text$"#name))) \
	static const char name[] = text;
DEF_RDTEXT(str_pcs_pcs, "%s: %s");
DEF_RDTEXT(str_fatal_error, "Fatal Error");
DEF_RDTEXT(str_error, "Error");
DEF_RDTEXT(str_out_of_mem, "Out of memory/resources");
DEF_RDTEXT(str_max_path, "MAX_PATH exceeded");
DEF_RDTEXT(str_out_of_space, "Out of disk space");
DEF_RDTEXT(str_io_fail, "file IO failure");
DEF_RDTEXT(str_bad_file, "invalid file format");
DEF_RDTEXT(str_open_fileA, "Cannot open file: \"%s\"");
DEF_RDTEXT(str_open_fileW, "Cannot open file: \"%S\"");
DEF_RDTEXT(str_rbA, "rb");

#define DEF_RDTEXTW(name, text) \
	__attribute__((section(".text$"#name))) \
	static const WCHAR name[] = text;
DEF_RDTEXTW(str_rbW, L"rb");

// libstdc++ bullshit
namespace std { 
DEF_RDTEXT(length_error, "length_error: %s");
__attribute__((section(".text$nothrow_t")))
const nothrow_t nothrow = nothrow_t();
void __attribute__((__noreturn__))
__throw_length_error(const char* str) { fatalError(length_error, str); }}
extern "C" void* emulate_nt_new(unsigned len, const std::nothrow_t& junk) {
	return malloc(len); }
extern "C" void* emulate_cc_new(unsigned len) { return xmalloc(len); }
extern "C" void* emulate_delete(void* p) { free(p); }
void* operator new(std::size_t, const std::nothrow_t&) __attribute__((alias("emulate_nt_new")));
void* operator new[](std::size_t, const std::nothrow_t&) __attribute__((alias("emulate_nt_new")));
void* operator new  (unsigned len) __attribute__((alias("emulate_cc_new")));
void* operator new[](unsigned len) __attribute__((alias("emulate_cc_new")));   
void  operator delete  (void* p)   __attribute__((alias("emulate_delete")));
void  operator delete[](void* p)   __attribute__((alias("emulate_delete")));
__attribute__((section(".bss$cxa_pure_virtual")))
void* __cxa_pure_virtual = 0;

// Error handling
void comnError(HWND hwnd, bool fatal,
	const char* fmt, va_list args)
{
	char caption[64]; char text[2048];
	sprintf(caption, str_pcs_pcs, progName,
		fatal ? str_fatal_error : str_error);
	vsprintf(text, fmt, args);
	MessageBoxA(hwnd, text, caption, MB_OK);
	if(fatal) ExitProcess(1);
}

#define ERRORM(hwnd, fatal, x) {				\
	va_list args; va_start (args, fmt);		\
	comnError(hwnd, fatal, fmt, args);		\
	va_end (args); x; }
void fatalError(const char* fmt, ...) ERRORM(
	GetLastActivePopup(GetActiveWindow()), true, UNREACH);
void fatalError(HWND hwnd, const char* fmt, ...) ERRORM(hwnd, true, UNREACH)
void contError(HWND hwnd, const char* fmt, ...) ERRORM(hwnd, false, )

// Standard errors
void errorAlloc() { fatalError(str_out_of_mem); }
void errorMaxPath() { fatalError(str_max_path); }
void errorDiskSpace() { fatalError(str_out_of_space); }
void errorDiskFail() { fatalError(str_io_fail); }
void errorBadFile() { fatalError(str_bad_file); }
void errorDiskWrite() { (GetLastError() == ERROR_DISK_FULL)
	? errorDiskSpace() : errorDiskFail(); }
SHITCALL
int fopen_ErrChk(void) { switch(errno)  { case ENOENT:
	case EACCES: case EISDIR: case EINVAL: return 1;
	case ENOSPC: return -1;  default: return 0; } 
}

// ANSI/Unicode
#include "stdshit.cc"
#include "stdshit.h"
#include "windows\windows.cpp"

// File handling
SHITCALL 
void xfread(void* ptr, size_t size, size_t num, FILE* fp)
{
	size_t result = fread(ptr, size, num, fp);
	if(result != num)
	{
		if(ferror(fp))
			errorDiskFail();
		else
			errorBadFile();
	}
}

SHITCALL
void xfwrite(const void* ptr, size_t size, size_t num, FILE* fp)
{
	size_t result = fwrite(ptr, size, num, fp);
	if(result != num)
		errorDiskWrite();
}

SHITCALL
void xchsize(FILE* fp, long size)
{
	int fd = fileno(fp);
	if(_chsize(fd, size))
		errorDiskWrite();
}

SHITCALL
int fsize(FILE* fp)
{
	int curPos = ftell(fp);
	fseek(fp, 0, SEEK_END);
	int endPos = ftell(fp);
	fseek(fp, curPos, SEEK_SET);
	return endPos-curPos;
}

loadFile_t loadFile(FILE* fp, int extra)
{
	if(!fp) return loadFile_t(0,-1);
	SCOPE_EXIT(fclose(fp));
	loadFile_t result; result.size = fsize(fp);
	if(result.data = malloc(result.size+extra)) {
		memset(result.data+result.size, 0, extra);
		xfread(result.data, result.size, fp);
	} else { min_ref(result.size, 0x7FFFFFFF); }
	return result;
}

char** loadText(FILE* fp, int& LineCount)
{
	// load file
	auto file = loadFile(fp, 1);
	if( fp == NULL ) { LineCount = -1;
		return NULL; } LineCount = 0;
	
	// split into lines 
	Void curPos = file.data;
	Void endPos = curPos+file.size;
	char** lineData = NULL;
	while(curPos < endPos)
	{
		xNextAlloc( lineData, LineCount )  = curPos;
		Void nextPos = strchr(curPos, '\n');
		if(nextPos != NULL) nextPos[0] = '\0';
		else nextPos = strchr(curPos, '\0');
		removeCrap((char*)curPos);
		curPos = nextPos+1;
	}
	return lineData;
}

Void memmem(const void *b1, const void *b2,
	size_t len1, size_t len2)
{
	char *sp = (char *) b1;
	char *pp = (char *) b2;
	char *eos   = sp + len1 - len2;
	if(!(b1 && b2 && len1 && len2))
		return NULL;
	while (sp <= eos) {
		if (*sp == *pp)
			if (memcmp(sp, pp, len2) == 0)
				return sp;
		sp++;
	}
	return NULL;
}

int xvfprintf ( FILE * stream, const char * format, va_list arg) {
	int result = vfprintf(stream, format, arg);
	if(result < 0) errorDiskWrite(); return result; }
int xfprintf ( FILE * stream, const char * format, ... ) {
	va_list vl; va_start(vl,format);
	return xvfprintf(stream, format, vl); va_end(vl); }
void xfputs (const char * str, FILE * stream) {
	if(fputs(str, stream) < 0) errorDiskWrite(); }

#endif

#include "string.cpp"
#include "xstrfmt.cpp"

RetEdx<int> xvector_::strcat2(const NCHAR* str)
{
	return this->write(str, strlen(str)+1);
}

int xvector_::strcat(const NCHAR* str)
{
	int strLen = strlen(str);
	this->write(str, strLen+1);
	dataSize -= sizeof(NCHAR); 
	return strLen;
}

SHITCALL NCHAR* strstr(const NCHAR* str1, const NCHAR* str2, int maxLen)
{
	int cmpLen = strlen(str2)-1;
	if(cmpLen < 0) return NULL;
	const NCHAR* endPos = (maxLen < 0) ? (NCHAR*)size_t(-1)
		: str1 + (maxLen - cmpLen);
	NCHAR findCh = *str2++; NCHAR ch;
	while((str1 < endPos)&&((lodsx(str1, ch), ch)))
	  if((ch == findCh)&&(!strncmp(str1, str2, cmpLen)))
		return (NCHAR*)str1-1;
	return NULL;
}

SHITCALL NCHAR* strstri(const NCHAR* str1, const NCHAR* str2, int maxLen)
{
	int cmpLen = strlen(str2)-1;
	if(cmpLen < 0) return NULL;
	const NCHAR* endPos = (maxLen < 0) ? (NCHAR*)size_t(-1)
		: str1 + (maxLen - cmpLen);
	NCHAR findCh = toUpper(*str2++); NCHAR ch;
	while((str1 < endPos)&&((lodsx(str1, ch), ch = toUpper(ch))))
	  if((ch == findCh)&&(!strnicmp(str1, str2, cmpLen)))
		return (NCHAR*)str1-1;
	return NULL;
}

#ifdef _OLDMINGW_
SHITCALL size_t MIF(NWIDE, wcsnlen, strnlen)(
	const NCHAR* str, size_t maxLen) { size_t len = 0;
	for(; str[len] && (len < maxLen); len++); return len; }
#endif

SHITCALL cstrT xstrdup(const NCHAR* str)
{
	if(str == NULL) return {0,0};
	int len = strlen(str);
	NCHAR* ret = xMalloc(len+1);
	return {strcpy(ret, str), len};
}

SHITCALL cstrT xstrdup(const NCHAR* str, size_t maxLen)
{
	if(str == NULL) return NULL;
	int strLen = strnlen(str, maxLen);
	NCHAR* buffer = xMalloc(strLen+1);
	return strcpyn(buffer, str, strLen);
}

// File handling
SHITCALL
FILE* xfopen(const NCHAR* fName, const NCHAR* mode)
{
	// prepare mode
	bool chkOpen = false;
	if(mode[0] == '!')
	{
		chkOpen = true;
		mode++;
	}
	
	// open file
LRETRY:
	FILE* fp = fopen(fName, mode);
	if(fp == NULL)
	{
		int err = fopen_ErrChk();
		if(err > 0) { if(chkOpen) fatalError(
			NCHFN(str_open_file), fName);
		} ei(err < 0) { 
			errorDiskSpace(); goto LRETRY; 
		} else { errorAlloc(); }
	}
	return fp;
}

SHITCALL
NCHAR* xfgets(NCHAR* str, int num, FILE* fp)
{
	NCHAR* tmp = fgets(str, num, fp);
	if((!tmp)&&(ferror(fp)))
		errorDiskFail();
	return tmp;
}

SHITCALL
loadFile_t loadFile(const NCHAR* fileName, int extra){
	return loadFile(xfopen(fileName, NCHFN(str_rb)), extra); }
SHITCALL
char** loadText(const NCHAR* fileName, int& LineCount){
	return loadText(xfopen(fileName, NCHFN(str_rb)), LineCount); }

// String Handling
SHITCALL
NCHAR* xstrdupr(NCHAR*& str1, const NCHAR* str2) {
	return free_repl(str1, xstrdup(str2)); }
SHITCALL
NCHAR* xstrdupr(NCHAR*& str1, const NCHAR* str2, size_t sz) {
	return free_repl(str1, xstrdup(str2, sz)); }

SHITCALL
int strcmp2(const NCHAR* str1, const NCHAR* str2)
{
	for(const NCHAR* curPos = str2;; curPos++) {
		NCHAR ch1; lodsx(str1, ch1);
		NCHAR ch2 = *curPos;
		if(ch1 != ch2) return curPos-str2;
		if(ch2 == 0) return -1;
	}
}

SHITCALL
int stricmp2(const NCHAR* str1, const NCHAR* str2)
{
	for(const NCHAR* curPos = str2;; curPos++) {
		NCHAR ch1; lodsx(str1, ch1); ch1 = toUpper(ch1);
		NCHAR ch2 = toUpper(*curPos);
		if(ch1 != ch2) return curPos-str2;
		if(ch2 == 0) return -1;
	}
}

SHITCALL
NCHAR* strScmp(const NCHAR* str1, const NCHAR* str2)
{
	while(1) {
		NCHAR ch2; lodsx(str2, ch2);
		if( ch2 == 0 )
			return (NCHAR*)str1;
		if( ch2 != *str1++ )
			return NULL;
	}
}

SHITCALL
NCHAR* strSicmp(const NCHAR* str1, const NCHAR* str2)
{
	while(1) { NCHAR ch2; 
		lodsx(str2, ch2); ch2 = toUpper(ch2);
		if( ch2 == 0 )
			return (NCHAR*)str1;
		if( ch2 != toUpper(*str1++) )
			return NULL;
	}
}

SHITCALL
int strEcmp(const NCHAR* str1, const NCHAR* str2)
{
	int diff = strlen(str1)-strlen(str2);
	if(diff < 0)
		return 1;
	return strcmp(str1+diff, str2);
}

SHITCALL
int strEicmp(const NCHAR* str1, const NCHAR* str2)
{
	int diff = strlen(str1)-strlen(str2);
	if(diff < 0)
		return 0;
	return stricmp(str1+diff, str2);
}

SHITCALL
int strNcpy(NCHAR* dst, const NCHAR* src, int num)
{
	for(int i = 0; i < num; i++)
	  if(!(dst[i] = src[i]))
		return i;
	if(num >= 0)
		dst[num] = '\0';
	return num;
}

SHITCALL cstr_<NCHAR> strcpyn(
	NCHAR* dst, const NCHAR* src, int len)
{
	memcpyX(dst, src, len);
	dst[len] = '\0'; return {dst, len};
}

SHITCALL
bool strcmpn(const NCHAR* str1, const NCHAR* str2, int len)
{
	if(strlen(str1) != len)
		return false;
	return !strncmp(str1, str2, len);
}

SHITCALL
bool stricmpn(const NCHAR* str1, const NCHAR* str2, int len)
{
	if(strlen(str1) != len)
		return false;
	return !strnicmp(str1, str2, len);
}

SHITCALL
int removeCrap(NCHAR* str)
{
	int len = strlen(str);
	while(len--)
		if(unsigned(str[len]) > ' ')
			break;
	str[len+1] = '\0';
	return len+1;
}

SHITCALL 
int strmove(NCHAR* dst, const NCHAR* src)
{
	int len = strlen(src)+1;
	memmove(dst, src, len*sizeof(NCHAR));
	return len;
}

// Path Handling
SHITCALL
int getPathLen(const NCHAR* fName)
{
	int i = strlen(fName);
	while(i--)
	{
		if((fName[i] == '\\')
		||(fName[i] == '/'))
			return i+1;
	}
	return 0;
}

SHITCALL
int getPath(NCHAR* fName)
{
	int len = getPathLen(fName);
	fName[len] = '\0';
	return len;
}

SHITCALL
NCHAR* getName(const NCHAR* fName)
{	return (NCHAR*)fName+getPathLen(fName); }

SHITCALL
int getName(NCHAR* dst, const NCHAR* src, size_t max)
{
	NCHAR* name = getName(src);
	return strNcpy(dst, src, max-1);
}

SHITCALL
bool isFullPath(const NCHAR* path)
{
	if( path
	&&((isPathSep(path[0]))
	||(path[1] == ':')))
		return true;
	return false;
}

// msvc2005 compatibility
int _vsnprintf_s(NCHAR *buffer, size_t sizeOfBuffer,
	const NCHAR *format, va_list ap) {
	if(sizeOfBuffer != 0) {
	size_t result =  MIF(NWIDE, _vsnwprintf, _vsnprintf)
		(buffer, sizeOfBuffer, format, ap);
		if(result < sizeOfBuffer)
			return result;
		buffer[sizeOfBuffer-1] = '\0'; }
	return -1; }
int sprintf_s(NCHAR *buffer, size_t sizeOfBuffer,
	const NCHAR *format, ... ) {
    int count;  va_list ap;
    va_start(ap, format);
    count = _vsnprintf_s(buffer, sizeOfBuffer, format, ap);
    va_end(ap); return count; }
