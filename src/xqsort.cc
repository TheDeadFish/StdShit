#define __XQSORT_C_
#include <stdshit.h>
//#include "xqsort.h"

XQSORT_DEF();
	
void xqsort_short(char *lo, char *hi, size_t width,
	xqsort_swap swap, xqsort_comp comp, void * context)
{
    char *p, *max; while (hi > lo) { max = lo;
	for (p = lo+width; p <= hi; p += width) {
	if (comp(context, p, max) > 0) { max = p; }}
    swap(max, hi); hi -= width; }
}

void xqsort_(size_t width_, xqsort_swap swap_,
	void* base, size_t num, xqsort_comp comp_, void *context_)
{
	#define __SHORTSORT xqsort_short
	#define CUTOFF 8
	#define STKSIZ (8*sizeof(void*) - 2)
	#define __COMPARE comp
	#define SWAP(x,y,z) swap(x,y)	
		
	#define ARGFIX(arg) asm("" : "+m"(arg));
	size_t width = width_; void *context = context_;
	xqsort_swap swap = swap_; xqsort_comp comp = comp_;
	ARGFIX(width); ARGFIX(context); ARGFIX(swap); ARGFIX(comp);	
    char *lo, *hi; char *mid; char *loguy, *higuy; size_t size;
    char *lostk[STKSIZ], *histk[STKSIZ]; int stkptr;
	
    if(!base ||(num < 2)) return;
	stkptr = 0; lo = (char *)base; hi = (char *)base + width * (num-1); 
recurse: size = (hi - lo) / width + 1; if (size <= CUTOFF) {
        __SHORTSORT(lo, hi, width, swap, comp, context); }
    else { mid = lo + (size / 2) * width; if (__COMPARE(context, lo, mid) > 0) {
	SWAP(lo, mid, width); } if (__COMPARE(context, lo, hi) > 0) { SWAP(lo, hi, width); }
	if (__COMPARE(context, mid, hi) > 0) { SWAP(mid, hi, width); } loguy = lo; higuy = hi;
    for (;;) { if (mid > loguy) { do  { loguy += width; } while (loguy < mid && 
	__COMPARE(context, loguy, mid) <= 0); }if (mid <= loguy) { do  {loguy += width;
	} while (loguy <= hi && __COMPARE(context, loguy, mid) <= 0); } do  { higuy -= width;
    } while (higuy > mid && __COMPARE(context, higuy, mid) > 0); if (higuy < loguy) break;
    SWAP(loguy, higuy, width); if (mid == higuy) mid = loguy; } higuy += width;
    if (mid < higuy) { do  { higuy -= width; } while (higuy > mid && __COMPARE(
	context, higuy, mid) == 0); } if (mid >= higuy) { do  { higuy -= width; } while 
	(higuy > lo && __COMPARE(context, higuy, mid) == 0); } if ( higuy - lo >= hi - loguy ) {
	if (lo < higuy) { lostk[stkptr] = lo; histk[stkptr] = higuy; ++stkptr; }
    if (loguy < hi) { lo = loguy; goto recurse; }} else {if (loguy < hi) {
	lostk[stkptr] = loguy; histk[stkptr] = hi;  ++stkptr;  } if (lo < higuy) { hi = higuy;
	goto recurse; } } } --stkptr; if (stkptr >= 0) { lo = lostk[stkptr]; hi = histk[stkptr];
        goto recurse; } else return;
}
