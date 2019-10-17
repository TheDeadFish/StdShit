#ifndef _XQSORT_H_
#define _XQSORT_H_

// memswap implementation
template <int n> __fastcall
void memswapn(void* p1, void* p2)
{
	for(int i = n; i >= 16; i -= 16) {
		asm("movups %0, %%xmm0; movups %1, %%xmm1; movups %%xmm0, %1;"
		"movups %%xmm1, %0" : "+m"(RB(p1)), "+m"(RB(p2)) :: "xmm0", "xmm1"); p1 += 16; p2 += 16; }
	if(n & 8) { asm("movsd %0, %%xmm0; movsd %1, %%xmm1; movsd %%xmm0, %1;"
		"movsd %%xmm1, %0" : "+m"(RB(p1)), "+m"(RB(p2)) :: "xmm0", "xmm1"); p1 += 8; p2 += 8; }
	if(n & 4) { asm("movss %0, %%xmm0; movss %1, %%xmm1; movss %%xmm0, %1; "
		"movss %%xmm1, %0" : "+m"(RB(p1)), "+m"(RB(p2)) :: "xmm0", "xmm1"); p1 += 4; p2 += 4; }
	if(n & 2) { asm("pushw %0; movw %1, %%ax; popw %1; movw %%ax, %0;"
		: "+m"(RB(p1)), "+m"(RB(p2)) :: "eax"); p1 += 2; p2 += 2; }
	if(n & 1) { asm("movb %0, %%al; movb %1, %%ah; movb %%al, %1; movb %%ah, %0;"
		: "+m"(RB(p1)), "+m"(RB(p2)) :: "eax"); p1 += 1; p2 += 1; }
}

// qsort with context
typedef int (REGCALL(3) *xqsort_comp) 
	(const void *, const void *, const void *);
typedef void (__fastcall *xqsort_swap)(void* a, void* b);
void __fastcall xqsort_(size_t width, xqsort_swap swap,
	void* base, size_t num, xqsort_comp comp, void *ctx);
	
// xqsort overloads
template <int N> __stdcall void xqsort(void* base, size_t num, xqsort_comp comp, void *ctx) {
	xqsort_(N, memswapn<N>, base, num, comp, ctx); }
template <class T, class F> __stdcall void xqsort(T* base, size_t num, F comp, void* ctx) {
	xqsort<sizeof(T)>(base, num, (xqsort_comp)(void*)comp, ctx); }

// common object sizes
#ifdef __XQSORT_C_
#define extern
#endif
#define XQSORT_INST(N) extern template void xqsort<N>(void*, size_t, xqsort_comp, void*);
XQSORT_INST(1) XQSORT_INST(2) XQSORT_INST(4) XQSORT_INST(8) XQSORT_INST(16)
XQSORT_INST(12) XQSORT_INST(24) XQSORT_INST(32)
#undef extern

#endif
