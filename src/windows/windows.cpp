#include "stdshit.h"

asm(".section .text$cmd_esc_len;"
	"_cmd_esc_msk: .long 0xFFFFFFFF,"
	"0x58001367, 0x78000000, 0x10000000,"
	"0xFFFFFFFF, 5, 0, 0;"
	"cmd_esc_len1: incl %eax; cmd_esc_len2:"
	"addl %esi, %eax; test %ebx,%ebx; jz 6f;"
	"jns 5f; incl %eax; 5: incl %eax; 6: ret;" );

asm(".section .text$cmd_esc_lenA; _cmd_esc_lenA:"
	".long cmd_esc_len2, cmd_esc_len1, cmd_esc_getA;"
	"cmd_esc_getA: movzbl (%edi), %ebx; incl %edi; ret;");

asm(".section .text$cmd_esc_lenW; _cmd_esc_lenW:"
	".long cmd_esc_len2, cmd_esc_len1, cmd_esc_getW;"
	"cmd_esc_getW: movzwl (%edi), %ebx; add $2, %edi; ret;");

asm(".section .text$cmd_esc_cpyA;"
	"_cmd_esc_cpyA: .long 3f, 2f, cmd_esc_getA;"
    "2: movb $34, (%eax); incl %eax;"
	"3: decl %esi; js 4f; movb $92, (%eax);"
	"inc %eax; jmp 3b; 4: test %ebx,%ebx; jz 6f;"
	"jns 5f; movb $94, (%eax); incl %eax; 5:"
	"movb %bl, (%eax); incl %eax; 6: ret;" );
	
asm(".section .text$cmd_esc_cpyW;"
	"_cmd_esc_cpyW: .long 3f, 2f, cmd_esc_getW;"
    "2: movw $34, (%eax); addl $2, %eax;"
	"3: decl %esi; js 4f; movw $92, (%eax);"
	"addl $2, %eax; jmp 3b; 4: test %ebx,%ebx; jz 6f;"
	"jns 5f; movw $94, (%eax); addl $2, %eax; 5:"
	"movw %bx, (%eax); addl $2, %eax; 6: ret;" );
	
ASM_FUNC("__Z14cmd_escape_lenPKcji@12",
	"movl $_cmd_esc_lenA, %edx; xor %eax, %eax; jmp _cmd_escape@20");
ASM_FUNC("__Z14cmd_escape_lenPKwji@12",
	"movl $_cmd_esc_lenW, %edx; xor %eax, %eax; jmp _cmd_escape@20");
ASM_FUNC("__Z10cmd_escapePcPKcji@16",
	"movl $_cmd_esc_cpyA, %edx; jmp _cmd_escape@20");
ASM_FUNC("__Z10cmd_escapePwPKwji@16",
	"movl $_cmd_esc_cpyW, %edx; jmp _cmd_escape@20");
	
extern "C"
size_t REGCALL(2) cmd_escape(size_t dstLen, void* vtable,
	const void* src, size_t len, byte flags)
{
	len += (size_t)src;
	asm("movl %%ecx, %%ebp; andl $2, %%ebp; testb %%cl, %%cl; js loop;"
		"xor %%esi, %%esi; xor %%ebx, %%ebx; jmp chg_quot; loop: orl $-1, %%esi;"
		"1: incl %%esi; cmpl %3, %%edi; je end; call *8(%%edx);"
		"cmpl $92, %%ebx; je 1b; cmpl $127, %%ebx; ja norm;"
		"bt %%ebx, _cmd_esc_msk(,%%ebp,8); jnc norm;"
		"testl %%ebx, %%ebx; je end; cmpl $34, %%ebx; je quot;"
		"cmpl $37, %%ebx; je pcnt; testb %%cl, %%cl; jns chg_quot;"
		"norm: call *(%%edx); jmp loop; quot: leal 1(%%esi,%%esi),"
		"%%esi; testl %%ebp, %%ebp; jz norm; jmp esc_char;"
		"pcnt: testb $32, %%cl; jne norm; esc_char:"
		"bts $31, %%ebx; testb %%cl, %%cl; jns norm;"
		"chg_quot: xorb $128, %%cl; call *4(%%edx); jmp loop;"
		"end: xorl %%ebx, %%ebx; testb %%cl,%%cl; js in_quot;"
		"testb $5, %%cl; je done; testb $1, %%cl; jne en_quot;"
		"testl %%esi, %%esi; je done; en_quot: addl $4, %%edx;"
		"in_quot: testb $1, %%cl; jne done;" 
		"add %%esi, %%esi; movb $34, %%bl; done: call *(%%edx);"
	: "+a"(dstLen) : "c"(flags), "D"(src), "m"(len), "d"(vtable) :
		"ebx", "esi", "ebp", "memory"); return dstLen;
}

extern "C" {
// utf8 api, c/posix functions
FILE *freopen(cch* name, cch* mode, FILE *fp) {
	WCHAR wmode[32]; utf816_cpy(wmode, mode);
	return _wfreopen(getNtPathNameX(name), wmode, fp); }
FILE* fopen(cch* name, cch* mode) {
	WCHAR wmode[32]; utf816_cpy(wmode, mode);
	return _wfopen(getNtPathNameX(name), wmode); }
int rename(cch *old_name,cch *new_name) {return _wrename(
	getNtPathNameX(old_name), getNtPathNameX(new_name)); }
#define _FWNDFN1(n,wn) int n(cch *s) { return wn(getNtPathNameX(s)); }
_FWNDFN1(remove, _wremove);
_FWNDFN1(_mkdir, _wmkdir); _FWNDFN1(_rmdir, _wrmdir);
int system(cch* s) { return _wsystem(widen(s)); }
}


	
// utf8 api, WIN32 gui functions
int WINAPI messageBox(HWND a, cch* b, cch* c, UINT d) {
	return MessageBoxW(a, widen(b), widen(c), d); }

// setWindowText
BOOL WINAPI setWindowText(HWND h,cch* s) { 
	return SetWindowTextW(h, widen(s)); }
BOOL WINAPI setDlgItemText(HWND h,int i,cch* s) { 
	return SetDlgItemTextW(h,i,widen(s)); }
	
// getWindowText
cstrW WINAPI getWindowTextW(HWND h) { W32SARD_(
	GetWindowTextLengthW(h), GetWindowTextW(h, ws, sz)); }
cstrW WINAPI getWindowText2W(HWND h) { W32SARD2_(
	GetWindowTextLengthW(h), GetWindowTextW(h, ws, sz)); }
cstr WINAPI getWindowText(HWND h) { return narrowFree(getWindowTextW(h)); }
cstr WINAPI getWindowText2(HWND h) { return narrowFree(getWindowText2W(h)); }







HMODULE getModuleBase(void* ptr)
{
	MEMORY_BASIC_INFORMATION bmi;
	VirtualQuery(ptr, &bmi, sizeof(bmi));
	return (HMODULE)bmi.AllocationBase;
}

// utf8 api, __getmainargs
extern "C" void __wgetmainargs(int * argc, 
	wchar_t *** argv_, wchar_t *** envp, int mode, void * si);
extern "C" void __getmainargs(int * argc, 
	char *** argv_, char *** envp, int mode, void * si)
{
	wchar_t** wargv; __wgetmainargs(argc, 
		&wargv, (wchar_t***)envp, mode, si); 
	int size = 0; for(wchar_t** wargv_ = wargv; *wargv_;
		wargv_++) size += utf816_size(*wargv_);
	char** argv = *argv_ = xMalloc((*argc)+1);
	char* curPos = xmalloc(size);
	for(wchar_t** wargv_ = wargv; *wargv_;
		wargv_++, argv++) { *argv = curPos; curPos =
		utf816_cpy(curPos, *wargv_)+1; } *argv = 0;		
}



cstrW getEnvironmentVariable(cchw* ws) { 
	WCHAR* buff = 0; int bsz = 0; int len;
	for(;;) { len = GetEnvironmentVariableW(ws, buff, bsz);
	if(!len) { free_ref(buff); break; } if(len<bsz) break;
	bsz=len; xRealloc(buff, bsz); } return {buff, len}; }
cstr getEnvironmentVariable(cch* s) { cstrW t = 
	getEnvironmentVariable(widen(s)); return narrowFree(t); }
	
// findfirst/findnext file UTF16
int REGCALL(1) findFirstFile(HANDLE& hFind, cchw* fName,
	WIN32_FIND_DATAW* pfd) { pfd->cFileName[0] = 0; hFind = 
	FindFirstFileW(getNtPathNameX(fName), pfd); if(isIHV(hFind)) { 
	int e = GetLastError(); return is_one_of(e,2,3) ? 1:((e==5) ? 2:3); }
	return (RI(pfd->cFileName) == '.') ? findNextFile(hFind, pfd) : -1; }
int WINAPI findNextFile(HANDLE hFind, WIN32_FIND_DATAW* pfd) {
	pfd->cFileName[0] = 0; AGAIN: if(FindNextFileW(hFind, pfd)) { 
	if((RI(pfd->cFileName) == 0x2E002E)&&(!pfd->cFileName[2]))
	goto AGAIN; return -1; } return (GetLastError() == 18) ? 0 : 3; }
	
// findfirst/findnext file UTF8
void WIN32_FIND_DATAU::init(WIN32_FIND_DATAW* src) { memcpy(this,
	src, offsetof(WIN32_FIND_DATAW, dwReserved1)); PI(&nFileSize)
	[0] = src->nFileSizeLow; PI(&nFileSize)[1] = src->nFileSizeHigh; 
	fnLength = utf816_cpy(cFileName, src->cFileName)-cFileName; }
int REGCALL(1) findFirstFile(HANDLE& hFind, cch* fName, 
	WIN32_FIND_DATAU* pfd) { WIN32_FIND_DATAW fd; SCOPE_EXIT(
	pfd->init(&fd)); return findFirstFile(hFind,widen(fName),&fd); }
int WINAPI findNextFile(HANDLE hFind, WIN32_FIND_DATAU* pfd) {
	WIN32_FIND_DATAW fd; SCOPE_EXIT(pfd->init(&fd)); 
	return findNextFile(hFind, &fd); }
	
cstr narrowFree(LPWSTR s) { SCOPE_EXIT(
	free(s)); return utf816_dup(s); }
	
cstrW ansi_to_wide_dup(cch* ansi, int len)
{
	int size = MultiByteToWideChar(CP_ACP, 0, ansi, len, 0, 0);
	if(!size) return {0,0}; WCHAR* buff = xMalloc(size); return {
	buff,MultiByteToWideChar(CP_ACP,0, ansi, len, buff, size)-1};
}

BOOL WINAPI createProcess(LPCSTR a, LPCSTR b,
	LPSECURITY_ATTRIBUTES c, LPSECURITY_ATTRIBUTES d,
	BOOL e, DWORD f, LPVOID g, LPCSTR h, LPSTARTUPINFOA i,
	LPPROCESS_INFORMATION j
) {
	SCOPE_EXIT(
		free_ref(i->lpDesktop); free_ref(i->lpTitle);
		free((void*)a); free((void*)b); free((void*)g); free((void*)h););
	
	// convert strings to utf16
	a = (cch*)utf816_dup(a).data; b = (cch*)utf816_dup(b).data;
	h = (cch*)utf816_dup(h).data; g = utf816_strLst_dup((cch*)g).data;
	i->lpDesktop = (char*)utf816_dup(i->lpDesktop).data;
	i->lpTitle = (char*)utf816_dup(i->lpTitle).data;
	
	return CreateProcessW((WCHAR*)a, (WCHAR*)b, c, 
		d, e, f, g, (WCHAR*)h, (LPSTARTUPINFOW)i, j);
}


// getFullPathName utf16
cstrW getFullPath(cchw* str) { return getFullPath_(cstrW(str),0); }
cstrW getFullPath(cstrW str) { return getFullPath_(str, 0); }
cstrW getFullPathF(cchw* str) { SCOPE_EXIT(free((void*)str)); return getFullPath(str); }
cstrW getFullPathF(cstrW str) { WCHAR* tmp = str; 
	SCOPE_EXIT(free(tmp)); return getFullPath(str); }
	
// getFullPathName utf8
cstr getFullPath(cch* str) { return getFullPath({str,-1}); }
cstr getFullPathF(cch* str) { return getFullPathF({str,-1}); }
cstr getFullPath(cstr str) { return narrowFree(
	getFullPathF(utf816_dup(str))); }
cstr getFullPathF(cstr str) { cstrW tmp = utf816_dup(str);
	free(str.data); return narrowFree(getFullPathF(tmp)); }
	
// getNtPathName, (support > 260 filenames)
cstrW getNtPathName(cch* s) { return getNtPathName({s,-1}); }
cstrW getNtPathName(cchw* s) { return getNtPathName(cstrW(s)); }
cstrW getNtPathName(cstrW s) { return getFullPath_(s, 2); }
cstrW getNtPathName(cstr s) { cstrW tmp = utf816_dup(s);
	WCHAR* tmp2 = tmp; SCOPE_EXIT(free(tmp2)); return getNtPathName(tmp); }

REGCALL(3)
cstrW getFullPath_(cstrW str_, int mode)
{
	// check for full path
	cstrW ret;
	if((str_.slen >= 4)&&(RI(str_.data) == 0x5C005C)
	&&(RI(str_.data+2) == 0x2F003F)) { ret = str_.xdup();
		RET: if(mode & 1) RB(ret+1) = '?'; return ret; }

	WCHAR* str = str_, *strEnd = str_.end();
	WCHAR* str2 = 0, *str2End, *backupPos;
	WCHAR buff[MAX_PATH]; WCHAR tmp[5];
	DWORD tmpLen;
	
	// 
PREFIX_REDO:
	int val1 = 0; for(WCHAR *e = str+4, *i = str; i < e; i++) { 
	val1 <<= 8; if(i < strEnd) { movb2(val1, RB(i));
	if(u8(val1) == '/') { movb2(val1, '\\'); } if(*i>>8) { 
	movb2(val1, 0xFF); } }}
	
	int val = bswap32(val1);
	if ( u8(val) == '\\' ) { 
	if ( !cmp8H(val,'\\') ) { tmpLen = 1; goto L1; }
	if ( cmp8H(val1, '.') ) {
		if( u8(val1) == 0 ) { tmpLen = 2; goto L1; }
		ei ( u8(val1) =='\\' ) {
	
			// Local device name "\\.\"
			backupPos = str+4;
			if(mode & 1) str += 4;
			goto DO_ALLOC;
		}}
		
		// Unc name. prefix = \\server\share
		backupPos = str+2;
		for(int sepCount = -1; backupPos < strEnd; backupPos++) {
		if(isPathSep(*backupPos)&&(++sepCount > 0)) break; }
		if(mode) { mode += 2; str += 2; }
	}
	else if ( cmp8H(val, ':')) {
		if ( cmp8H(val1, '\\')) { backupPos = str+3; }
		else { tmpLen = 2; goto L1; }
	} else {
		
		// 
		tmpLen = 0; L1: 
		WCHAR* tmpPos = tmp; memcpy_ref(tmpPos, str, tmpLen); 
		RI(tmpPos) = '.'; tmpPos++; if(str != strEnd) 
		RI(tmpPos) = '\\';  str2 = str; str2End = strEnd;
		DWORD len = GetFullPathNameW(tmp, MAX_PATH, buff, 0);
		str = buff; strEnd = buff+len; goto PREFIX_REDO;		
	}
	
DO_ALLOC:;

	// allocate and write prefix
	if(!str2) { str2End = strEnd; strEnd = str2 = backupPos; }
	DWORD buffSz = PTRDIFF(strEnd,str)+PTRDIFF(str2End,str2);
	DWORD extra = ALIGN(mode,1)*2;
	WCHAR* rbuff = xmalloc(buffSz+(extra+1)*2);
	WCHAR* dstPos = memcpyX(rbuff, L"\\\\?\\UNC\\", extra);
	backupPos += dstPos-str;
	while(str < strEnd) { WCHAR ch = *str; str++;
		if(ch == '/') ch = '\\'; stosw(dstPos, ch); }
	
	// write the main text
	WCHAR* src = movll(str2); WCHAR* end = movll(str2End);
	WCHAR* dstPart = dstPos;
	for(;;src++) { WCHAR ch; if(src < end) { ch = *src;
		if(!isPathSep(ch)) { stosw(dstPos, ch); continue; }}
	if(uint diff = PTRDIFF(dstPos,dstPart)) {
	if((*dstPart == '.')&&(diff <= 4)) {
		dstPos = dstPart; if(diff == 4) { VARFIX(dstPos);
		if(dstPart[1] == '.') { if(dstPos > backupPos) { dstPos--;
		while((dstPos > backupPos)&&(*--dstPos != '\\')); }}}}}
	if(!(src < end)) break; if(dstPos[-1] != '\\') {
		WRI(dstPos, '\\'); dstPart = dstPos; } } *dstPos = 0;
	ret.data = movll(rbuff); ret.setend(dstPos); goto RET;
}

#define NWIDE 0
#include "windows8W.cxx"
#define NWIDE 1
#include "windows8W.cxx"
