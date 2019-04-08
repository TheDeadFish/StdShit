#include "stdshit.h"

// calculate UTF8 character length
ASM_FUNC("_UTF8_LEN1", "cmp $127, %eax; ja _UTF8_LEN2; mov $1, %eax; ret;"
	GLOB_LAB("_UTF8_LEN2") "cmp $2047, %eax; ja 1f; mov $2, %eax; ret;"
	"1: sub $65536, %eax; mov $4, %eax; sbb $0, %eax; ret");

// encode UTF8 character
ASM_FUNC("_UTF8_PUT1", "cmp $127, %eax; "
	"ja _UTF8_PUT2; stosb; ret;" GLOB_LAB("_UTF8_PUT2")
	"cmpl $2048, %eax; movl %eax, %edx; jae _UTF8_PUT34;"
	"sar $6, %eax; orb $192, %al; 1: stosb; andb $63, %dl;"
	"orb $128, %dl; mov %dl, (ptdi); inc ptdi; ret;"
	"_UTF8_PUT34: cmpl $65536, %eax; jae _UTF8_PUT4;"
	"sar $12, %eax; orb $224, %al; 2: stosb; movl %edx, %eax;"
	"sar $6, %eax; andb $63, %al; orb $128, %al; jmp 1b;"
	"_UTF8_PUT4: sar $18, %eax; orb $240, %al; stosb; movl %edx, %eax;"
	"sar $12, %eax; andb $63, %al; orb $128, %al; jmp 2b"
);

// decode UTF8 character
#define UTF8_GETB(x) "movb " #x "(ptsi), %dl; sall	$6, %eax;" \
	"xorb $128, %dl; cmpb $64, %dl; jae _UTF8_GETE; orb %dl, %al;"
#define UTF8_CHK(x) "cmp ptdx, ptsi; je _UTF8_GETE; andb $" #x ", %al;"
ASM_FUNC("_UTF8_GET1", "or $-1, ptdx;" GLOB_LAB("_UTF8_GET2")
	"cmpb $194, %al; jb _UTF8_GETE; cmpb $224, %al; jae _UTF8_GET34;"
	UTF8_CHK(31) UTF8_GETB(0) "inc ptsi; ret;"
	"_UTF8_GET34: dec ptdx;	cmpb $240, %al; jae _UTF8_GET4;"
	UTF8_CHK(15) UTF8_GETB(0) UTF8_GETB(1)
	"cmpw $2048, %ax; jb _UTF8_GETE; pt_inc2 ptsi; ret;"
	"_UTF8_GETE: movl $63, %eax; ret;"
	"_UTF8_GET4: dec ptdx;" UTF8_CHK(15)
	UTF8_GETB(0) "cmpl $16, %eax; jb _UTF8_GETE;"
	"cmpw $272, %ax; jae _UTF8_GETE;"
	UTF8_GETB(1) UTF8_GETB(2) "lea 3(ptsi), ptsi; ret"
);
	
// UTF16 encode/decode
ASM_FUNC("_UTF16_PUT1", "cmp $65536, %eax;"
	"jb 1f; _UTF16_PUT2:; movl %eax, %edx;"
	"shrl $10, %eax; andw $1023, %dx;"
	"addw $55232, %ax; addw $56320, %dx;"
	"stosw; movl %edx, %eax; 1: stosw; ret;" );
ASM_FUNC("_UTF16_GET1", "or $-1, ptdx; _UTF16_GET2: "
	"movzwl %ax, %eax; cmpw $0xD800, %ax; jb 1f; cmpw $0xDC00, %ax;"
	"jae 1f; cmp ptdx, ptsi; je 1f; movzwl (ptsi), %edx;"
	"cmpw $0xDC00, %dx; jb 1f; cmpw $0xE000, %dx; jae 1f; pt_inc2 ptsi;"
	"sal $10, %eax; lea -0x35FDC00(ptax,ptdx), %eax; 1: ret" );

// UTF8 to UTF16 size
#define UTF816SZ(z,n) utf816_size_tW __stdcall utf816_size(cch* str MIF(n, \
	(,int len),)) { int r = 0; asm("1: inc %0;" MIF(n, "cmp %2,%1; " \
	"jz 3f;",) "movzbl (%1),%%eax; inc %1; testb %%al,%%al;" MIF(z, \
	"je 3f;",) "jns 1b;" MIF(n, "mov %2,ptdx; call _UTF8_GET2", "call " \
	"_UTF8_GET1") ";shrl $16, %%eax; je 1b; inc %0; jmp 1b; 3:" : "+b"(r), \
	"+S"(str) : MIF(n,"c"(str+len),) : "eax", "edx" ); return {r*2,str}; }
UTF816SZ(1, 0); UTF816SZ(1, 1);

// UTF8 to UTF16 copy
asm(".macro utf816_cpyw_ sn,dn,sr; jmp 0f; 1:" AMIF(\\dn, "cmp ptcx, ptdi; ja 3f",) 
	"stosw; 0:" AMIF(\\sn, "cmp \\sr,ptsi; jz 3f",) "movzbl (ptsi),%eax; inc ptsi; "
	"testb %al,%al; je 3f; jns 1b;" AMIF(\\sn, "mov \\sr,ptdx; call _UTF8_GET2",
	"call _UTF8_GET1") AMIF(\\dn, "cmp $65535, %eax; jle 1b; cmp ptcx, ptdi; jnb 3f;"
	"call _UTF16_PUT2", "call _UTF16_PUT1") "jmp 0b; 3:; .endm;");
	
#define UTF816CP(dn,sn) NEVER_INLINE UTF816CP_(_,dn,sn) { asm("utf816_cpyw_ \
	"#sn", "#dn",%2" : "+D"(dst), "+S"(src) : MIF(sn, (MIF(dn, ("b"(src+len),),\
	"c"(src+len))),) MIF(dn, "c"(dst+dstMax-2),)); return dst; } \
UTF816CP_(,dn,sn) { MIF(dn, if(ptrdiff_t(dstMax) > 0),) { dst = utf816_cpy_(\
	dst MIF(dn,(,dstMax),), src MIF(sn,(,len),)); *dst = 0; } return dst; }
UTF816CP(0,0) UTF816CP(0,1) UTF816CP(1,0) UTF816CP(1,1)

// UTF16 to UTF8 size
ASM_FUNC("_UTF16TO8_LEN1", "or $-1, ptdx; _UTF16TO8_LEN2:"
	"inc %ecx; cmp $0x800, %ax; jae 1f; 2: ret; 1: inc %ecx;"
	"andw $0xFC00, %ax; cmpw $0xD800, %ax; jne 2b; cmp ptdx, ptsi;"
	"jae 2b; movw (%esi), %ax; andw $0xFC00, %ax; cmpw $0xDC00, %ax;"
	"jne 2b; inc %ecx; pt_inc2 ptsi; ret" );
utf816_size_t8 utf816_size(cchw* str) { 
	size_t sz = 0; asm("1: lodsw; inc %0; cmpw $128, %%ax; jb 2f;"
	"call _UTF16TO8_LEN1; jmp 1b;" "2: testb %%al, %%al; jne 1b;"
	: "+c"(sz), "+S"(str) :: "eax", "edx"); return {sz, str}; }	
utf816_size_t8 utf816_size(cchw* str, int len) { size_t sz = 0; asm(
	"1: inc %0; cmp %2, %1; jae 3f; lodsw; cmpw $128, %%ax; jb 1b;"
	"mov %2, ptdx; call _UTF16TO8_LEN2; jmp 1b; 3:" : "+c"(sz), 
	"+S"(str) : "b"(str+len) : "eax", "edx"); return {sz, str}; }
	
// UTF16 to UTF8 copy
char* utf816_cpy(char* mbs, cchw* ws) { asm("1: lodsw; cmpw $128, %%ax; jb 2f;"
	"call _UTF16_GET1; call _UTF8_PUT2; jmp 1b; 2: stosb; testb %%al, %%al; jne 1b;"
	: "+D"(mbs), "+S"(ws) :: "eax", "edx"); return mbs-1; }
char* utf816_cpy(char* mbs, cchw* ws, int wsz) { asm("jmp 0f; 1: lodsw; "
	"cmpw $128, %%ax; jb 2f; mov %2, ptdx; call _UTF16_GET2; call _UTF8_PUT2;"
	"jmp 0f; 2: stosb; 0: cmp %2, %1; jb 1b;" : "+D"(mbs), "+S"(ws) :
	"b"(ws+wsz) : "eax", "edx"); *mbs =0; return mbs; }

#define UTF816_DUP(t1, t2, t3) \
t1 __stdcall utf816_dup(const t3* src) { if(!src) return {0,0};	\
	t2* buff = (t2*) xmalloc(utf816_size(src)); ARGFIX(src); t2* end \
	= utf816_cpy( buff, src); return t1{buff, end-buff}; } \
t1 __stdcall utf816_dup(const t3* src, int len) { if(!len) return {0,0}; \
	t2* buff = (t2*) xmalloc(utf816_size(src, len)); ARGFIX(src); ARGFIX(len); \
	t2* end = utf816_cpy(buff, src, len); return t1{buff, end-buff}; }
UTF816_DUP(cstrW, wchar_t, char); UTF816_DUP(cstr, char, wchar_t);


// non-null terminated
int __stdcall utf816_size2(cch* s, int l) { cch* e = s+l;
	int len = 0; for(;;) { len++; if(s >= e) return len*2;
	int ch = u8(RDI(s)); UTF816_LEN8B(ch, s, e, len); }}	
WCHAR* __stdcall utf816_cpy2_(WCHAR* d, cch* s, int l) { cch* e = s+l;
	while(s < e) { int ch = u8(RDI(s)); if(s8(ch)>=0) { stosw(d, ch); 
	} else {UTF816_CPY8B(ch, d, s, e); }} return d; }
WCHAR* __stdcall utf816_cpy2(WCHAR* d, cch* s, int l) {
	WCHAR* t = utf816_cpy2_(d, s, l); *t = 0; return t; }
cstrW __stdcall utf816_dup2(cch* s, int l) { if(!l) return {0,0};
	WCHAR* b = xmalloc(utf816_size2(s, l)); ARGFIX(s); ARGFIX(l);
	WCHAR* e = utf816_cpy2(b, s, l); return {b, e-b}; }
cstrW __stdcall utf816_strLst_dup(cch* str) { if(!str) 
	return {0,0}; cch* tmp = str; while(RW(tmp)) tmp++; 
	return utf816_dup2(str, tmp-str+2); }
