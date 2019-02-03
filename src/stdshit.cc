#include "stdshit.h"

// tristi quod ad hunc, const strings
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
DEF_RDTEXT(str_rbA, "rb"); DEF_RDTEXT(str_rA, "r");
DEF_SECTDAT(mem_zp4) const char mem_zp4[16] = {};

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
ASM_FUNC("___cxa_pure_virtual", "int $3");

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



// File handling
SHITCALL void xfclose(FILE* fp) {
	if(fclose(fp)) errorDiskFail(); }
SHITCALL void xfclose_ref(FILE*& fp) {
	if(fclose_ref(fp)) errorDiskFail(); }

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

xarray<byte> loadFile(FILE* fp, int extra)
{
	if(!fp) return {0,-1}; SCOPE_EXIT(fclose(fp));
	xarray<byte> result; result.len = fsize(fp);
	if(result.data = malloc(result.len+extra)) {
		memset(result.data+result.len, 0, extra);
		xfread(result.data, result.len, fp);
	} else { min_ref(result.len, 0x7FFFFFFF); }
	return result;
}

xarray<char> loadText(FILE* fp)
{
	if(!fp) return {0,-1}; SCOPE_EXIT(fclose(fp));
	size_t size = fsize(fp); 
	char* buff = (char*)malloc(size+4); if(!buff) 
	return {0,min_ref(size, 0x7FFFFFFF)};
	size = fread(buff, 1, size, fp);
	if(ferror(fp)) errorDiskFail();
	RI(buff+size) = 0; return {buff, size};
}

char** loadText(FILE* fp, int& LineCount)
{
	// load file
	auto file = loadFile(fp, 1);
	if( fp == NULL ) { LineCount = -1;
		return NULL; } LineCount = 0;
	
	// split into lines 
	Void curPos = file.data;
	Void endPos = curPos+file.len;
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

int saveFile(cch* fName, 
	void* data, size_t size)
{	
	FILE* fp = fopen(fName, "wb");
	if(!fp) return fopen_ErrChk();
	SCOPE_EXIT(fclose(fp));
	if(fwrite(data, size, 1, fp) != 1)
		return -1; return 0;
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

RetEdx<int> xvector_::strcat2(const char* str)
{
	return this->write(str, strlen(str)+1);
}

int xvector_::strcat(const char* str)
{
	int strLen = strlen(str);
	this->write(str, strLen+1);
	dataSize -= sizeof(char); 
	return strLen;
}

/* old strstr replacements
SHITCALL char* strstr(const char* str1,
	const char* str2, int maxLen) {
	return cstr_str((char*)str1, strnlen(str1,
		maxLen), (char*)str2, strlen(str2)); }
SHITCALL char* strstri(const char* str1,
	const char* str2, int maxLen) {
	return cstr_istr((char*)str1, strnlen(str1,
		maxLen), (char*)str2, strlen(str2)); }
*/



cstr cstr_len(cch*si);



// File handling
SHITCALL
FILE* xfopen(const char* fName, const char* mode)
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
			str_open_fileA, fName);
		} ei(err < 0) { 
			errorDiskSpace(); goto LRETRY; 
		} else { errorAlloc(); }
	}
	return fp;
}

SHITCALL
char* xfgets(char* str, int num, FILE* fp)
{
	char* tmp = fgets(str, num, fp);
	if((!tmp)&&(ferror(fp)))
		errorDiskFail();
	return tmp;
}

SHITCALL
xarray<byte> loadFile(const char* fileName, int extra){
	return loadFile(xfopen(fileName, str_rbA), extra); }
SHITCALL
xarray<char> loadText(cch* fName) {
	return loadText(xfopen(fName, str_rA)); }
SHITCALL
char** loadText(const char* fileName, int& LineCount){
	return loadText(xfopen(fileName, str_rbA), LineCount); }

REGCALL(2) void* memswap(
	void* p1, void* p2, size_t sz)
{
	while(!isNeg(sz -= 16)) {
		asm volatile ("movups (%%eax), %%xmm0; movups (%%edx), %%xmm1;"
			"movups %%xmm0, (%%edx); movups %%xmm1, (%%eax)"
		: "+a"(p1), "+d"(p2) :: "xmm0", "xmm1"); p1 += 16; p2 += 16; }
	if(sz & 8) {
		asm volatile ("movsd (%%eax), %%xmm0; movsd (%%edx), %%xmm1;"
			"movsd %%xmm0, (%%edx); movsd %%xmm1, (%%eax)"
		: "+a"(p1), "+d"(p2) :: "xmm0", "xmm1"); p1 += 8; p2 += 8; }
	if(sz & 4) {
		asm volatile ("movss (%%eax), %%xmm0; movss (%%edx), %%xmm1;"
			"movss %%xmm0, (%%edx); movss %%xmm1, (%%eax)"
		: "+a"(p1), "+d"(p2) :: "xmm0", "xmm1"); p1 += 4; p2 += 4; }
	if(sz &= 3) { asm volatile("cmp $2, %%cl; jc 1f; pushw (%%eax);"
		"movw (%%edx), %%cx; popw (%%edx); movw %%cx, (%%eax); "
		"lea 2(%%eax), %%eax; lea 2(%%edx), %%edx; jz 2f;"
		"1: movb (%%eax), %%cl; movb (%%edx), %%ch; movb %%cl, (%%edx);"
		"movb %%ch, (%%eax); 2:" : "+a"(p1), "+d"(p2) : "c"(sz));
		p1 += 1; p2 += 1;  }
	
	return p1;
}

void* __thiscall xRngPtr_get(void** ptr, size_t size)
{
	void* tmp; return (ptrAddChk(*ptr, size, tmp)
	||(tmp > ptr[1])) ? 0 : release(*ptr, tmp);
}

void* array_insert1(void* ptr, size_t asize, size_t index, size_t size)
{
	asize *= size; index *= size; size_t iDst = index+size;
	void* dst = ptr+iDst; void* src = ptr+index; 
	memmove(dst, src, asize-iDst); return src; 
}
