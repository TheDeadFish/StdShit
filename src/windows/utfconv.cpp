

// calculate UTF8 character length
ASM_FUNC("_UTF8_LEN1", "cmp $127, %eax; ja _UTF8_LEN2; mov $1, %eax; ret;"
	GLOB_LAB("_UTF8_LEN2") "cmp $2047, %eax; ja 1f; mov $2, %eax; ret;"
	"1: sub $65536, %eax; mov $4, %eax; sbb $0, %eax; ret");

// encode UTF8 character
ASM_FUNC("_UTF8_PUT1", "cmp $127, %eax; "
	"ja _UTF8_PUT2; stosb; ret;" GLOB_LAB("_UTF8_PUT2")
	"cmpl $2048, %eax; movl %eax, %edx; jae _UTF8_PUT34;"
	"sar $6, %eax; orb $192, %al; 1: stosb; andb $63, %dl;"
	"orb $128, %dl; mov %dl, (%edi); inc %edi; ret;"
	"_UTF8_PUT34: cmpl $65536, %eax; jae _UTF8_PUT4;"
	"sar $12, %eax; orb $224, %al; 2: stosb; movl %edx, %eax;"
	"sar $6, %eax; andb $63, %al; orb $128, %al; jmp 1b;"
	"_UTF8_PUT4: sar $18, %eax; orb $240, %al; stosb; movl %edx, %eax;"
	"sar $12, %eax; andb $63, %al; orb $128, %al; jmp 2b"
);

// decode UTF8 character
#define UTF8_GETB(x) "movb " #x "(%esi), %dl; sall	$6, %eax;" \
	"xorb $128, %dl; cmpb $64, %dl; jae _UTF8_GETE; orb %dl, %al;"
#define UTF8_CHK(x) "cmpl %edx, %esi; je _UTF8_GETE; andb $" #x ", %al;"
ASM_FUNC("_UTF8_GET1", "orl $-1, %edx;" GLOB_LAB("_UTF8_GET2")
	"cmpb $194, %al; jb _UTF8_GETE; cmpb $224, %al; jae _UTF8_GET34;"
	UTF8_CHK(31) UTF8_GETB(0) "inc %esi; ret;"
	"_UTF8_GET34: dec %edx;	cmpb $240, %al; jae _UTF8_GET4;"
	UTF8_CHK(15) UTF8_GETB(0) UTF8_GETB(1)
	"cmpw $2048, %ax; jb _UTF8_GETE; inc %esi; inc %esi; ret;"
	"_UTF8_GETE: movl $63, %eax; ret;"
	"_UTF8_GET4: dec %edx;" UTF8_CHK(15)
	UTF8_GETB(0) "cmpl $16, %eax; jb _UTF8_GETE;"
	"cmpw $272, %ax; jae _UTF8_GETE;"
	UTF8_GETB(1) UTF8_GETB(2) "lea 3(%esi), %esi; ret"
);
	
// UTF16 encode/decode
ASM_FUNC("_UTF16_PUT1",
	"cmp $65536, %eax; jb 1f; movl %eax, %edx;"
	"shrl $10, %eax; andw $1023, %dx;"
	"addw $55232, %ax; addw $56320, %dx;"
	"stosw; movl %edx, %eax; 1: stosw; ret;" );
ASM_FUNC("_UTF16_GET1", "orl $-1, %edx; _UTF16_GET2: "
	"movzwl %ax, %eax; cmpw $0xD800, %ax; jb 1f; cmpw $0xDC00, %ax;"
	"jae 1f; cmpl %edx, %esi; je 1f; movzwl (%esi), %edx;"
	"cmpw $0xDC00, %dx; jb 1f; cmpw $0xE000, %dx; jae 1f; inc %esi;"
	"inc %esi; sal $10, %eax; lea 0xFCA02400(%eax,%edx), %eax; 1: ret" );

// UTF8 to UTF16 size
#define UTF816SZ(z,n,x) int __stdcall MCAT(utf816_size,x)(cch* str MIF(n, \
	(,int len),)) { int r = 0; asm("1: inc %0;" MIF(n, "cmp %2,%1; " \
	"jz 3f;",) "movzbl (%1),%%eax; inc %1; testb %%al,%%al;" MIF(z, \
	"je 3f;",) "jns 1b;" MIF(n, "movl %2,%%edx; call _UTF8_GET2", "call " \
	"_UTF8_GET1") ";shrl $16, %%eax; je 1b; inc %0; jmp 1b; 3:" : "+b"(r) \  
	: "S"(str) MIF(n,(,"c"(str+len)),) : "eax", "edx" ); return r*2; }
UTF816SZ(1, 0, ); UTF816SZ(1, 1, );

// UTF8 to UTF16 copy
#define UTF816CP(n) WCHAR* __stdcall utf816_cpy_(WCHAR* dst, cch* str MIF(n, \
	(,int len),)) { asm("jmp 0f; 1: stosw; 0:" MIF(n, "cmp %2,%1; jz 3f;",) \
	"movzbl (%1),%%eax; inc %1; testb %%al,%%al; je 3f; jns 1b;" \
	MIF(n, "movl %2,%%edx; call _UTF8_GET2", "call _UTF8_GET1") \
	";call _UTF16_PUT1; jmp 0b; 3:" : "+D"(dst) : "S"(str) \
	MIF(n,(,"c"(str+len)),) : "eax", "edx" ); return dst; } \
WCHAR* __stdcall utf816_cpy(WCHAR* dst, cch* str MIF(n, (,int len),)) { \
	WCHAR* tmp = utf816_cpy_(dst, str MIF(n,(,len),)); *tmp = 0; return tmp; }
UTF816CP(1) UTF816CP(0)

// UTF16 to UTF8 conversion
ASM_FUNC("_UTF16TO8_LEN1", "orl $-1, %edx; _UTF16TO8_LEN2:"
	"inc %ebx; cmp $0x800, %ax; jae 1f; 2: ret; 1: inc %ebx;"
	"andw $0xFC00, %ax; cmpw $0xD800, %ax; jne 2b; cmpl %edx, %esi;"
	"jae 2b; movw (%esi), %ax; andw $0xFC00, %ax; cmpw $0xDC00, %ax;"
	"jne 2b; inc %ebx; inc %esi; inc %esi; ret" );
ASM_FUNC("__Z11utf816_sizePKw@4", "pushl %esi; pushl %ebx; movl 12(%esp), %esi;"
	"xorl %ebx, %ebx; 1: lodsw; inc %ebx; cmpw $128, %ax; jae 2f; testb %al, %al;"
	"jne 1b; movl %ebx, %eax; popl %ebx; popl %esi; ret $4;"
	"2: call _UTF16TO8_LEN1; jmp 1b" );
ASM_FUNC("__Z11utf816_sizePKwi@8", "pushl %ebx; pushl %ebp; xorl %ebx, %ebx; pushl %esi;"
	"movl 20(%esp), %ebp; movl 16(%esp), %esi; lea (%esi,%ebp,2), %ebp; 1: inc %ebx;cmpl %ebp, %esi;"
	"jae 3f; lodsw; cmpw $128, %ax; jb 1b;movl %ebp, %edx; call _UTF16TO8_LEN2; jmp 1b;"
	"3: popl %esi; popl %ebp; movl %ebx, %eax; popl %ebx; ret $8;" );
ASM_FUNC("__Z10utf816_cpyPcPKw@8", "pushl %esi; pushl %edi; movl 16(%esp), %esi;"
	"movl 12(%esp), %edi; 1: lodsw; cmpw $128, %ax; jae 2f; stosb;"
	"testb %al, %al; jne 1b; lea -1(%edi), %eax; popl %edi; popl %esi;"
	"ret $8; 2: call _UTF16_GET1; call _UTF8_PUT2; jmp 1b" );
ASM_FUNC("__Z10utf816_cpyPcPKwi@12", "pushl %edi; pushl %ebp; pushl %esi; movl 24(%esp), %ebp;"
	"movl 20(%esp), %esi; movl 16(%esp), %edi; lea (%esi,%ebp,2), %ebp; jmp 1f;"
	"0: stosb; 1: cmpl %ebp, %esi; jae 3f; lodsw; cmpw $128, %ax; jb 0b;"
	"movl %ebp, %edx; call _UTF16_GET2; call _UTF8_PUT2; jmp 1b;"
	"3: popl %esi; popl %ebp; movb $0, (%edi); popl %edi; ret $12;" );

#define UTF816_DUP(t1, t2, t3) \
t1 __stdcall utf816_dup(const t3* src) { if(!src) return {0,0};	\
	t2* buff = (t2*) xmalloc(utf816_size(src)); ARGFIX(src); t2* end \
	= utf816_cpy( buff, src); return t1{buff, end-buff}; } \
t1 __stdcall utf816_dup(const t3* src, int len) { if(!len) return {0,0}; \
	t2* buff = (t2*) xmalloc(utf816_size(src, len)); ARGFIX(src); ARGFIX(len); \
	t2* end = utf816_cpy(buff, src, len); return t1{buff, end-buff}; }
UTF816_DUP(cstrW, wchar_t, char); UTF816_DUP(cstr, char, wchar_t);
