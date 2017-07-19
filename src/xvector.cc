#include "stdshit.h"

void xvectorW__::clear(void) {
	free_ref(data); size_ = 0; }

DEF_RDTEXT(xvectorW_overflow, "xvectorW__ overflow");
void xvectorW__::nalloc_(u32 nsize, u32 sz) {
	u32 newSize = msize+(msize>>1); u32 limit = 65535; 
	VARFIX(limit); if(newSize > limit) newSize = limit;
	ARGFIX(nsize); if(newSize < nsize) { newSize = nsize; }
	if(newSize > limit) fatalError(xvectorW_overflow);
	msize = newSize; xrealloc(data, newSize*sz); }
	
// absolute allocators
byte* xvectorW__::xreserve(u32 ns, u32 sz) { 
	if(ns > msize) nalloc_(ns, sz); return data; }
byte* xvectorW__::xresize(u32 ns, u32 sz) {
	size = ns; return xreserve(ns, sz); }
byte* xvectorW__::xnalloc2(u32 ns, u32 sz) { 
	if(ns > size) { xresize(ns, sz); } return data; }
byte* xvectorW__::xncalloc2(u32 ns, u32 sz) { ns -= size;
	if(!isNeg(ns)) { xncalloc(ns, sz); } return data; }

// relative allocators	
byte* xvectorW__::xnalloc(u32 sz) { return xnalloc(1, sz); }
byte* xvectorW__::xnralloc(size_t ds, u32 sz) { ds += size;
	if(ds > msize) nalloc_(ds, sz); return data+size*sz; }
byte* xvectorW__::xnalloc(size_t ds, u32 sz) { ds += size;
	if(ds > msize) nalloc_(ds, sz); ARGFIX(sz);
	byte* ret = data+size*sz; size = ds; return ret; }
byte* xvectorW__::xncalloc(size_t ds, u32 sz) {
	return (byte*)memset(xnalloc(ds, sz), 0, ds*sz); }
