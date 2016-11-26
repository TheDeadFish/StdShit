// Memory allocation / containers
#ifndef _ARRAY_MEM_H_
#define _ARRAY_MEM_H_
#define _ARRAY_MEM_FIX

TMPL(T) struct xRngPtr 
{ 
	T* data; T* end_; 

	// construction
	xRngPtr() = default; 
	xRngPtr(const xRngPtr& that) = default;
	xRngPtr(T* d, int l) : data(d), end_(d+l) {}
	xRngPtr(T* d, T* e) : data(d), end_(e) {}
	
	// pointer / length access
	T* begin() { return data; }
	T* end() { return end_; }
	operator T*() { return data; }
	int count() { return end_-data; }
	int offset(T* pos) { return pos-data; }
	T* operator->() { return data; }
	xRngPtr& operator++() { data++; return *this; }

	// trimming operations	
	ALWAYS_INLINE bool chk() { return end_ > data; }
	ALWAYS_INLINE T& f() { return *data; } 
	ALWAYS_INLINE T& l() { return end_[-1]; }
	ALWAYS_INLINE T& fi() { return *data++; }
	ALWAYS_INLINE T& ld() { return *--end_; };
	
	// reference trimming helpers
	ALWAYS_INLINE bool chk(T* cp) { return end_ > cp; }
	ALWAYS_INLINE bool chk2() { ARGFIX(end_); return chk(); }
	ALWAYS_INLINE bool chk2(T* cp) { ARGFIX(end_); return chk(cp); }
};

// constructor helpers
template <class T, typename... Args>
T* pNew(T* ptr, Args&& ... args) { __assume(ptr != NULL);
	return new(ptr) T(args...); }
TMPL(T) void pDel(T* ptr) { ptr->~T(); }
TMPL(T) constexpr bool hasDtor(T* p) { return
	!std::is_trivially_destructible<T>::value; }
	
#define XARRAY_COMMON(C, T, len) \
	C() = default; C(const C& that) = default; \
	C(const T* d, int l) : data((T*)d), len(l) {} \
	C(const T* d, const T* e) : data((T*)d), len(e-d) {} \
	template<typename... Args> C& init(Args... args) \
		{ return *this = C(args...); } \
	void init(){ data=0; len=0; } void free() { ::free(data); } \
	C release() { return {::release(data), ::release(len) }; } \
	void setbase( T* pos) { len += data-pos; data = pos; } \
	void setend(T* pos) { len = pos-data; } int offset( \
	T* pos) { pos - data; } DEF_BEGINEND(T, data, len); \
	C left(int i) { return C(data, i); } \
	C right(int i) { return C(data+i, len-i); } \
	C endRel(int i) { return C(end(), i-len); } \
	ALWAYS_INLINE xRngPtr<T> ptr() { return xRngPtr<T>{data, end()}; } \
	ALWAYS_INLINE void set(xRngPtr<T> ptr) { init(ptr.data, ptr.end_); } \
	ALWAYS_INLINE void sete(xRngPtr<T> ptr) { setend(ptr.end_); }
	
TMPL(T) struct xarray 
{
	// creation / assignment
	T* data; size_t len;
	XARRAY_COMMON(xarray, T, len);
	template<int l> xarray(T(& d)[l]) : xarray((T*)d, l) {}
		
	// destructor safe ops
	void Free() { for(auto& ref : *this) ref.~T(); this->free(); }
	void Clear() { this->Free(); this->init(); }
	T* xAlloc(size_t size) { T* ptr = xalloc(size);
		for(int i = 0; i < size; i++) pNew(ptr+i); }
	template<typename... Args>
	T& push_back(Args... args) { T* ptr = &xnxalloc(); 
		__assume(ptr); return *(new (ptr) T(args...)); }
	void pop_back(void) { len--; end()->~T(); }
	
	// destructor unsafe ops
	T* xalloc(size_t size) { return data = xMalloc(len = size); }
	T* xresize(size_t size) { return xRealloc(data, len = size); }
	T& xnxalloc() {	return *(T*)xnxalloc2(this, sizeof(T)); }
	
	// copying functions
	T* xcopy(const xarray& that) { return xcopy(that.data, that.len); }
	T* xcopy(const T* di, size_t ci) { memcpyX(xalloc(ci), di, ci); return data; }
	T* xCopy(const xarray& that) { return xCopy(that.data, that.len); }
	T* xCopy(const T* di, size_t ci) { if(!hasDtor(di)) return xcopy(di, ci);
	 T* ptr = xalloc(ci); for(int i = 0; i < ci; i++) pNew(ptr+i, di[i]); return ptr; }
};

TMPL(T) struct xArray : xarray<T>
{
	xArray() : xarray<T>(0,0) {}
	~xArray() { this->Free(); }
	xArray(const xarray<T>& that) : xarray<T>(that) {}
	xArray(const xArray& that) { this->xCopy(that); }
};

struct xvector_ {
	Void dataPtr; size_t dataSize, allocSize; 
	void init_() { ZINIT; }
	void init_(Void buff, int buffSize) {
		dataPtr = buff; dataSize = 0; allocSize = buffSize; }
	void free_() { free_ref(dataPtr); }	
	void clear_() { free_(); dataSize = 0; allocSize = 0; }
	Void release_(void) { ::release(dataPtr); }
	Void xalloc_(size_t size);
	Void xreserve_(size_t size); Void xresize_(size_t size);
	VoidLen xnxalloc2_(size_t size); VoidLen xrxalloc2_(size_t size);
	Void xnxalloc_(size_t size) { return xnxalloc2_(size); }
	Void xrxalloc_(size_t size) { return xrxalloc2_(size); }
	
	// pointer/size access
	Void begin(size_t offset = 0) { return dataPtr+offset; }
	Void end(size_t offset = 0)  { return dataPtr+dataSize+offset; }
	size_t addSize(size_t val) { return ::release(dataSize, dataSize+val); }
	size_t setEnd(Void endPos) { return ::release(dataSize, endPos-dataPtr); }
	TMPL(T)
	T& operator()(IndexRef<T> ref) { return dataPtr(ref); }

	// write operations
	#define _xvector_write_(sz, count) return write##sz(src,count);
	TMPL(T) int write(const T*src,int count) {
		_memcpy_sz(_xvector_write_); }
	TMPL(T) int write(const T& val) {
		if(sizeof(T) == 1) return write8(CAST(byte,val));
		ei(sizeof(T) == 2) return write16(CAST(word,val));
		ei(sizeof(T) == 4) return write32(CAST(uint,val));
		ei(sizeof(T) == 8) return write64(CAST(INT64, val));
		else write(&val, 1); }
	RetEdx<int> write8(Void src,int count);
	RetEdx<int> write16(Void src,int count);
	RetEdx<int> write32(Void src,int count);
	RetEdx<int> write8(byte); RetEdx<int> write16(word);
	RetEdx<int> write32(uint); RetEdx<int> write64(INT64);
	RetEdx<int> strcat2(const char* str);
	RetEdx<int> strcat2(const WCHAR* str);

	// string operations
	int strcat(const char* str); 
	int strcat(const WCHAR* str); 
	void fmtcat(const char* fmt, ...);
	void fmtcat(const WCHAR* fmt, ...);
};

TMPL(T)
struct xvector : xvector_ {
	xvector() {}
	//xvector(const xarray<T>& That) { init(That); }
	xvector(const xvector<T>& That) { init(That); }
	xvector(T* di, size_t ci) { init(di, ci); }
	xvector(T* di, size_t ds, size_t as) { init2(di, ds, as); }

	// destructor safe ops
	size_t getCount() const { return dataSize/sizeof(T); }
	size_t getAlloc() const { return allocSize/sizeof(T); }	
	DEF_BEGINEND(T, dataPtr, dataSize);
	//xarray<T> release(void) { xarray<T> result(dataPtr, getCount());
	//	this->init(); return result; }
	xvector<T> release2(void) { xvector<T> result = *this;
		this->init(); return result; }
	void Free() { if(std::is_trivially_destructible<T>::value) 
		{ this->free_(); } else { dtor(); }}
	void Clear() { this->Free(); dataSize = 0; allocSize = 0; }
	//T* xCopy(const xarray<T>& That) { return xCopy(That.dataPtr, That.count); }
	T* xCopy(const xvector<T>& That) { return xCopy(That.dataPtr, That.getCount()); }
	T* xCopy(const T* di, size_t ci) { if(std::is_trivially_destructible<T>::value)
		{ return xcopy(di, ci); } else { ctor((T*)di, ci); }}
	T* xAlloc(size_t size) { T* ptr = xalloc(size);
		for(int i = 0; i < size; i++) pNew(ptr); }
	T* xReserve(size_t size) { return xreserve_(size*sizeof(T)); }
	T* xNalloc(size_t size) { T* ptr = xnalloc(size);
		for(int i = 0; i < size; i++) pNew(ptr+i); }
	template<typename... Args>
	T& push_back(Args... args) { T* ptr = xnalloc(1); 
		__assume(ptr != NULL); return *(new (ptr) T(args...)); }
	void pop_back(void) { addCount(-1); end()->~T(); }
	T* xRalloc(size_t size) { return xrxalloc_(size*sizeof(T)); }
	
	// destructor unsafe ops
	void addCount(size_t val) { dataSize += (val*sizeof(T)); }
	void setCount(size_t val) { dataSize = (val*sizeof(T)); }
	//void init(const xarray<T>& That) { init(That.dataPtr, That.count); }
	void init(const xvector<T>& That) { init2(That.dataPtr, That.dataSize, That.allocSize); }
	void init(T* di, size_t ci) { init2(di, ci*sizeof(T), ci*sizeof(T)); }
	void init2(T* di, size_t ds, size_t as) {
		this->dataPtr = di; this->dataSize = ds; this->allocSize = as; }
	//xarray<T> get() const { return xarray<T>(this->dataPtr, getCount()); }
	xvector<T> get2() const  { return *this; }
	//T* xcopy(const xarray<T>& That) { return xcopy(That.dataPtr, That.count); }
	T* xcopy(const xvector<T>& That) { return xcopy(That.dataPtr, That.getCount()); }
	T* xcopy(const T* di, size_t ci) { memcpyX(xresize(ci), di, ci); return this->dataPtr; }
	T* xalloc(size_t size) { return this->xalloc_(size*sizeof(T)); }
	T* xresize(size_t size) { return xresize_(size*sizeof(T)); }
	T* xnalloc(size_t size) { return xnxalloc_(size*sizeof(T)); }
private:
	__attribute__ ((noinline)) void ctor(T* di, size_t ci) {
		T* dstPos = xalloc(ci); for(int i = 0; i < ci; i++) {
			pNew(dstPos+i, di[i]); }}
	__attribute__ ((noinline)) void dtor() { if(dataPtr) {
		for(auto& obj : *this) obj.~T(); ::free(dataPtr); }}
};		

TMPL(T)
struct xVector : xvector<T> {
	xVector() { xvector_::init_(); }
	//xVector(const xarray<T>& That) { this->xCopy(That); }
	xVector(const T* di, size_t ci) { this->xCopy(di, ci); }
	//xVector(const xArray<T>& That) { this->xCopy(That.get()); }
	xVector(const xvector<T>& That) { this->xCopy(That); }
	xVector(const xVector<T>& That) { this->xCopy(That.get2()); }
	xVector(xVector<T>&& That) { this->init2(
		That.dataPtr, That.dataSize, That.allocSize); }
	~xVector() { this->Free(); }
};

#endif
