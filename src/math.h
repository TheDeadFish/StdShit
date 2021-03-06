// fast math shit / tables
#ifndef _DF_MATH_H_
#define _DF_MATH_H_
#ifndef _WIN64

#ifdef __SSE4_1__
#define _DFC_(n, t, x, m) static inline t MCAT(f,n)(t f) { \
	asm("ROUNDP" #x " $" #m ", %1, %0" : "=x"(f) : "x"(f)); return f; } \
	static inline int MCAT(i,n)(t f) { f = MCAT(f,n)(f); int y; asm( \
		"cvtts"#x"2si %1, %0" : "=g"(y) : "x"(f)); return y; }
#else
#define _DFC_(n, t, x, m) static inline t MCAT(f,n)(t f) { asm("call _f"\
	#n : "+t"(f)); return f; } static inline int MCAT(i,n)(t f) { int y; \
	asm("call _i"#n : "=a"(y) : "t"(f) : "st"); return y; }
#endif
_DFC_(floor, float, S, 1); _DFC_(floor, double, D, 1);
_DFC_(ceil, float, S, 2); _DFC_(ceil, double, D, 2);
#undef _DFC_

static inline void lrintf (int& dst, float x) {
  __asm__ __volatile__
    ("fistpl %0"  : "=m" (dst) : "t" (x) : "st"); }

#endif

extern const uint powersOf10[10];
extern const byte tableOfHex[2][16];

#endif
