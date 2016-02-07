#ifndef _MATH_CC_
#define _MATH_CC_

__attribute__((section(".text$powersOf10")))
const uint powersOf10[] = {1, 10, 100, 1000, 10000,
	100000, 1000000, 10000000, 100000000, 1000000000 };
	
__attribute__((section(".text$tableOfHex")))
const byte tableOfHex[2][16] = {
	{'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'},
	{'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'}};

// integer floor/ceil
int ifloor(float f) {	
	asm(
	"subl	$8, %esp				\n\t"
	"fnstcw	6(%esp)					\n\t"
	"flds	12(%esp)				\n\t"
	"movw	6(%esp), %ax			\n\t"
	"orb	$4, %ah					\n\t"
	"movw	%ax, 4(%esp)			\n\t"
	"fldcw	4(%esp)					\n\t"
	"fistpl	(%esp)					\n\t"
	"fldcw	6(%esp)					\n\t"
	"movl	(%esp), %eax			\n\t"
	"addl	$8, %esp				\n\t"); }
int iceil(float f) {	
	asm(
	"subl	$8, %esp				\n\t"
	"fnstcw	6(%esp)					\n\t"
	"flds	12(%esp)				\n\t"
	"movw	6(%esp), %ax			\n\t"
	"orb	$8, %ah					\n\t"
	"movw	%ax, 4(%esp)			\n\t"
	"fldcw	4(%esp)					\n\t"
	"fistpl	(%esp)					\n\t"
	"fldcw	6(%esp)					\n\t"
	"movl	(%esp), %eax			\n\t"
	"addl	$8, %esp				\n\t"); }

#endif
