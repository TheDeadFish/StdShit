// Memory allocation / containers
#ifndef _ARRAY_MEM_H_
#define _ARRAY_MEM_H_
#define _ARRAY_MEM_FIX

template <class T, typename... Args>
T* pNew(T* ptr, Args&& ... args) { __assume(ptr != NULL);
	return new(ptr) T(args...); }

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
