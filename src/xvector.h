#ifndef _XVECTORW_
#define _XVECTORW_

struct xvectorW__ { 

	byte* data; union { struct { 
	u16 size, msize; }; u32 size_; };
	void nalloc_(u32 ns, u32 sz);
	void nalloc2_(u32 ns, u32 sz) {
		if(ns > msize) nalloc_(ns, sz); }
	void init(void) { data = 0; size_ = 0; }
	void clear(void); void free(){ ::free(data); }
	
	// absolute allocators
	NEVER_INLINE byte* xreserve(u32 ns, u32 sz);
	NEVER_INLINE byte* xresize(u32 ns, u32 sz);
	NEVER_INLINE byte* xnalloc2(u32 ns, u32 sz);
	NEVER_INLINE byte* xncalloc2(u32 ns, u32 sz);
	
	// relative allocators
	NEVER_INLINE byte* xnalloc(u32 sz);
	NEVER_INLINE byte* xnalloc(u32 ds, u32 sz);
	NEVER_INLINE byte* xnralloc(u32 ds, u32 sz);
	NEVER_INLINE byte* xncalloc(u32 ds, u32 sz);
};

template <int sz> 
struct xvectorW_ : xvectorW__ 
{ 
	// absolute allocators
	byte* xreserve(u32 ns) {  return xvectorW__::xreserve(ns, sz); }
	byte* xresize(u32 ns) {  return xvectorW__::xresize(ns, sz); }
	byte* xnalloc2(u32 ns) {  return xvectorW__::xnalloc2(ns, sz); }
	byte* xncalloc2(u32 ns) {  return xvectorW__::xncalloc2(ns, sz); }
	
	// relative allocators
	byte* xnalloc(void) { return xvectorW__::xnalloc(sz); }
	byte* xnalloc(u32 ds) { return xvectorW__::xnalloc(ds, sz); }
	byte* xnralloc(u32 ds) { return xvectorW__::xnralloc(ds, sz); }
	byte* xncalloc(u32 ds) { return xvectorW__::xncalloc(ds, sz); }
};

TMPL(T) struct xvectorW
{
	T* data; u16 size, msize;
	DEF_BEGINEND(T, data, size);
	void init(void) { BT().init(); }
	
	// freeing / destructors
	void clear(void) { BT().clear(); }
	void free() { BT().free(); }
	void Clear() { Free(); data = 0; BT().size_ = 0; }
	void Free() { if(hasDtorT(T)) dtor(); else free(); }
	
	template<typename... Args> T& push_back(Args...
		args) { return *pNew(xnalloc(), args...); }
	void pop_back(void) { size--; end()->~T(); }
	T& back(void) { return end()[-1]; }
	T& ib() { return data[size++]; }
	
	// absolute allocators
	T* xreserve(u32 ns) {  return (T*)BT().xreserve(ns); }
	T* xresize(u32 ns) {  return (T*)BT().xresize(ns); }
	T* xnalloc2(u32 ns) {  return (T*)BT().xnalloc2(ns); }
	T* xncalloc2(u32 ns) {  return (T*)BT().xncalloc2(ns); }
	
	// relative allocators
	T* xnalloc(void) { return (T*)BT().xnalloc(); }
	T* xnalloc(u32 ds) { return (T*)BT().xnalloc(ds); }
	T* xnralloc(u32 ds) { return (T*)BT().xnralloc(ds); }
	T* xncalloc(u32 ds) { return (T*)BT().xncalloc(ds); }
		
	
	
private:
	xvectorW_<sizeof(T)>& BT(void) { return *(xvectorW_<sizeof(T)>*)this; }
	__attribute__ ((noinline)) void dtor() { if(data) {
		for(auto& obj : *this) obj.~T(); ::free(data); }}
};

TMPL(T) struct xVectorW : xvectorW<T>
{
	xVectorW() { this->init(); }
	~xVectorW() { this->Free(); }
};

#endif
