#include "stdshit.h"

#ifndef _WIN64


extern "C" NTSYSAPI DWORD NTAPI
RtlUpcaseUnicodeChar(DWORD);

ASM_FUNC("_UTF8_GETI", "movzbl (%esi),%eax; inc %esi;"
	"test %al,%al; jns 1f; call _UTF8_GET1;"
	"cmp $65535, %eax; jns 2f; 1: push %eax; "
	"call _RtlUpcaseUnicodeChar@4; 2: RET;")
	
int utf8_cmpi(cch* s1, cch* s2)
{
	int ret; asm("1: xchg %%esi, %%edi; call _UTF8_GETI;"
	"mov %%eax, %%ebx; xchg %%esi, %%edi; call _UTF8_GETI;"
	"subl %%ebx, %%eax; jnz 1f; test %%ebx,%%ebx; jnz 1b; 1:"
	: "=a"(ret) : "S"(s1), "D"(s2) : "ebx"); return ret;
}

int utf8_cmpi(cch* s1, int l1, WCHAR* s2) 
{
	for(cch* e1 = s1+l1;;) { int ch2 = RDI(s2);
		if(ch2 > 127) { swapReg(s1,s2);	UTF16_GET1(ch2,s1);
			swapReg(s1,s2);	if(ch2 > 65535) goto L1; }
		ch2 = RtlUpcaseUnicodeChar(ch2); L1: VARFIX(ch2);
		int ch1 = 0; if(s1 < e1) { ch1 = uns(RDI(s1));
		if(s8(ch1) < 0) { UTF8_GET2(ch1,s1,e1);
			if(ch1 > 65535) goto L2; }
		ch1 = RtlUpcaseUnicodeChar(ch1); L2:; }
		if((ch1 -= ch2)||(!ch2)) return ch1;
	}
}

#endif
