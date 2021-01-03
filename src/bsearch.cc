#include "stdshit.h"

xRngPtr<byte> bsearch2 (
	const void* key, const void* base,
  size_t num, size_t size, compar_t compar)
{
	byte* x = (byte*)bsearch(key, base, num, size, compar);
	byte* y = x; VARFIX(y);
	if(x) {
		byte* end = (byte*)base + num*size;
		while(((y += size) < end)&&(!compar(key, y))) { ARGFIX(key); ARGFIX(compar); }
		while(((x -= size) >= base)&&(!compar(key, x))) { ARGFIX(key);  ARGFIX(compar); }
		VARFIX(x); x += size;
	}
	return {x, y};
}
