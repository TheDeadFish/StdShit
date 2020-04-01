#include "stdshit.h"

__attribute__((section(".text$powersOf10")))
const uint powersOf10[] = {1, 10, 100, 1000, 10000,
	100000, 1000000, 10000000, 100000000, 1000000000 };
	
__attribute__((section(".text$tableOfHex")))
const byte tableOfHex[2][16] = {
	{'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'},
	{'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'}};

#ifndef _WIN64

// fast floor/ceil
#ifndef __SSE4_1__
 #define _DFC_(n, v) ASM_FUNC("_f"#n, "pushl $"#v"; fnstcw 2(%esp); " \
 "fldcw (%esp); frndint; fldcw 2(%esp); addl $4, %esp; ret; "); \
 ASM_FUNC("_i"#n, "pushl $"#v"; push %eax; fnstcw 6(%esp); fldcw 4(%esp);" \
 "fistpl (%esp); fldcw 6(%esp); pop %eax; addl $4, %esp; ret;");
 _DFC_(ceil, 0xB7F) _DFC_(floor, 0x77F);
 #undef _DFC_
#endif

#endif