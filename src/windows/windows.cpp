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

int sysfmt(const char* fmt, ...) {
	VA_ARG_FWD(fmt); char* str = xstrfmt(va);
	SCOPE_EXIT(free(str)); return system(str);  }
	
extern "C" {
// utf8 api, c/posix functions
FILE *freopen(cch* name, cch* mode, FILE *fp) {
	WCHAR wmode[32]; utf816_cpy(wmode, mode);
	FNWIDEN(1,name); return _wfreopen(cs1, wmode, fp); }
FILE* fopen(cch* name, cch* mode) {
	WCHAR wmode[32]; utf816_cpy(wmode, mode);
	FNWIDEN(1,name); return _wfopen(cs1, wmode); }
int rename(cch *old_name,cch *new_name) { FNWIDEN(1,old_name);
	FNWIDEN(2,new_name); return _wrename(cs1, cs2); }
#define _FWNDFN1(n,wn) int n(cch *s) { FNWIDEN(1,s); return wn(cs1); }
_FWNDFN1(remove, _wremove);
_FWNDFN1(_mkdir, _wmkdir); _FWNDFN1(_rmdir, _wrmdir);
int system(cch* s) { return _wsystem(widen(s)); }
}


// utf8 api, WIN32 file functions
HANDLE WINAPI createFile(LPCSTR a,DWORD b,DWORD c,
	LPSECURITY_ATTRIBUTES d,DWORD e,DWORD f,HANDLE g) 
{ 	FNWIDEN(1,a); return CreateFileW(cs1, b, c, d, e, f, g); }
DWORD WINAPI getFileAttributes(cch* name) {
	FNWIDEN(1,name); return GetFileAttributesW(cs1); }
BOOL WINAPI createDirectory(cch* a, LPSECURITY_ATTRIBUTES b) {
	FNWIDEN(1,a); return CreateDirectoryW(cs1, b); }
BOOL WINAPI copyFile(cch* a, cch* b, BOOL c) {
	FNWIDEN(1,a); FNWIDEN(2,b); return CopyFileW(cs1, cs2, c); }
BOOL WINAPI moveFile(cch* a, cch* b) {
	FNWIDEN(1,a); FNWIDEN(2,b); return MoveFileW(cs1, cs2); }
BOOL WINAPI moveFileEx(cch* a, cch* b, DWORD c) {
	FNWIDEN(1,a); FNWIDEN(2,b); return MoveFileExW(cs1, cs2, c); }
BOOL WINAPI deleteFile(cch* a) { FNWIDEN(1,a); return DeleteFileW(cs1); }
BOOL WINAPI setFileAttributes(cch* a, DWORD b) { FNWIDEN(1,a);
	return SetFileAttributesW(cs1, b); }
	
// utf8 api, WIN32 gui functions
int WINAPI messageBox(HWND a, cch* b, cch* c, UINT d) {
	return MessageBoxW(a, widen(b), widen(c), d); }

// utf8 api, simple WIN32 functions
BOOL WINAPI setWindowText(HWND h,cch* s) {
	return SetWindowTextW(h, widen(s)); }
BOOL WINAPI setDlgItemText(HWND h,int i,cch* s) {
	return SetDlgItemTextW(h,i,widen(s)); }
cstr WINAPI getWindowText(HWND h) { W32SARD_(
	GetWindowTextLengthW(h), GetWindowTextW(h, ws, sz)) }	
cstr WINAPI getWindowText2(HWND h) { W32SARD2_(
	GetWindowTextLengthW(h), GetWindowTextW(h, ws, sz)) }
cstr WINAPI getDlgItemText(HWND h, int i) {
	return getWindowText(GetDlgItem(h, i)); }
cstr WINAPI getDlgItemText2(HWND h, int i) {
	return getWindowText2(GetDlgItem(h, i)); }

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

// the final abomination, getFullPath utf8
ASM_FUNC("getFullPath_", "push %ebx; movl 12(%esp), %ebx; pushl 16(%esp);"
	"push %ebx; call __Z10utf816_dupPKci@8; shrb 20(%esp); movl %eax, %ecx;"
	"jnc 1f; push %ebx; call _sfree; 1: cmpl $0x5C005C, (%eax); jnz 1f; cmpl "
	"$0x2F003F, 4(%eax); jz 0f; 1: push %eax; add $260, %edx; pushl $0;"
	"lea 12(%edx,%edx), %eax; push %eax; call _xmalloc; lea 12(%eax), %ebx;"
	"push %ebx; push %edx; push %ecx; call _GetFullPathNameW@16; call _sfree;"
	"movl %eax, %edx; movl %ebx, %eax; lea -12(%ebx), %ecx; 0: pop %ebx; ret;");
ASM_FUNC("__Z11getFullPathPKcii@12", "call getFullPath_; push %ecx;"
	"push %eax; call __Z10utf816_dupPKw@4; call _sfree; ret $12");
ASM_FUNC("__Z14getNtPathName_PKcii@12", "call getFullPath_; cmp %eax, %ecx; jz 1f;"
	"cmpl $0x005C005C, (%eax); jnz 2f; cmpl $0x005C002E, 4(%eax); jz 3f;"
	"sub $4, %eax; movl $0x004E0055, (%eax); movb $0x43, 4(%eax);"
	"2: sub $8, %eax; movl $0x005C005C, (%eax); 3: movl $0x005C003F,"
	"4(%eax); 1: shrb 12(%esp); jnc 1f; movb $63, 2(%eax); 1: ret $12;");
ASM_FUNC("__Z14getNtPathName_PKc@4", "push $0; push $-1; push 12(%esp);"
	"call __Z14getNtPathName_PKcii@12; ret $4;");
	
// simple path handling helpers
ASM_FUNC("_isRelPath0", "test %eax, %eax; jz 1f;"
	"cmpb $0, (%eax); jz 1f; jmp 4f;"
GLOB_LAB("_isRelPath") "cmp $1, %edx; jbe 2f; 4:cmpb $58, 1(%eax); jnz 3f;"
	"0: movb $0, %cl; ret; 2: test %edx, %edx; jz 1f; 3: cmpb $92, (%eax);"
	"jz 0b; cmpb $47, (%eax); jz 0b; 1: movb $1, %cl; ret;");
ASM_FUNC("__Z7getPath4cstr@8", "mov %edx, %ecx; 0: test %edx, %edx; "
	"jz 1f; cmpb $92, -1(%eax,%edx); jz 1f; cmpb $47, -1(%eax,%edx);"
	"jz 1f; dec %edx; jmp 0b; 1: test %edx, %edx; jz 1f; 0: ret; 1:"
	"cmp $2, %ecx; jb 0b; cmpb $58, 1(%eax); jnz 0b; add $2, %edx; ret;" 
GLOB_LAB("__Z7getName4cstr@8") "call __Z7getPath4cstr@8;"
	"subl %edx, %ecx; add %edx, %eax; mov %ecx, %edx; ret;");
	
cstr getModuleFileName(HMODULE hModule) {
	WCHAR buff[MAX_PATH];
	if(!GetModuleFileNameW(hModule, buff, MAX_PATH))
		return {0,0}; return utf816_dup(buff); }
cstr getProgramDir(void) {
	return getPath0(getModuleFileName(NULL)); }
	
cstr getEnvironmentVariable(cch* s) {
	wxstr ws = widen(s); WCHAR* buff = 0; int bsz = 0;
	for(;;) { int len = GetEnvironmentVariableW(ws, buff, bsz);
	if((len<bsz)||!len) break; bsz=len; xRealloc(buff, bsz); }
	return narrowFree(buff); }
	
// findfirst/findnext file
void WIN32_FIND_DATAU::init(WIN32_FIND_DATAW* src) { memcpy(this,
	src, offsetof(WIN32_FIND_DATAW, dwReserved1)); PI(&nFileSize)
	[0] = src->nFileSizeLow; PI(&nFileSize)[1] = src->nFileSizeHigh; 
	RI(cFileName) &= 0; fnLength = utf816_cpy(
		cFileName, src->cFileName)-cFileName; }
int REGCALL(1) findFirstFile(HANDLE& hFind, 
	cch* fileName, WIN32_FIND_DATAU* pfd){ WIN32_FIND_DATAW fd; 
	{ FNWIDEN(1,fileName); hFind = FindFirstFileW(cs1, &fd); }
	if(isIHV(hFind)) { int e = GetLastError(); 
	return is_one_of(e,2,3) ? 1:((e==5) ? 2:3); }
	pfd->init(&fd); if(RW(pfd->cFileName) == '.') 
	return findNextFile(hFind, pfd); return -1; }
int WINAPI findNextFile(HANDLE hFind, WIN32_FIND_DATAU* pfd) {
	WIN32_FIND_DATAW fd; AGAIN: if(FindNextFileW(hFind, &fd)) { 
	pfd->init(&fd); if(RI(pfd->cFileName) == '..') goto AGAIN; 
	return -1; } return (GetLastError() == 18) ? 0 : 3; }
	
cstr WINAPI shGetFolderPath(int nFolder) { WCHAR buff[MAX_PATH]; 
	if(SHGetFolderPathW(0, nFolder, NULL, 0, buff)) 
		return {0,0}; return utf816_dup(buff);  }
	
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
