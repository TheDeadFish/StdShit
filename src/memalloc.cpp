
SHITCALL uint snapNext(uint val) { if(val&(val-1)) return 0;
	if(val>=2) return val<<1; return val+1; }

// resource freeing functions
SHITCALL void free_ref(Void& ptr) {
	if(ptr != NULL) {free(ptr);	ptr = NULL; }}
SHITCALL void fclose_ref(FILE*& stream) {		
	if(stream != NULL) { fclose(stream); stream = NULL; }}
#undef fclose
SHITCALL int fclose_ ( FILE * stream ){
	if( stream == NULL ) return 0; return fclose(stream); }
SHITCALL void freeLst(Void ptr, int count) { if(!ptr) return;
	for(int i = 0; i < count; i++) free((void*)ptr.sizet(i)); }
SHITCALL void freeLst_ref(Void& ptr, int count) {
	freeLst(ptr, count); free_ref(ptr); }
	
// may-fail allocators
SHITCALL Void calloc (size_t size) { return calloc(1, size); }

SHITCALL2
Void nxalloc(Void& ptr, size_t& count_, size_t size)
{
	size_t result = movrl(b, count_); incml(count_);
	size_t count = movrl(d, result);
	int count_1 = movfx(a, result-1);
	result *= size; Void ptr2;
	if(likely(count & count_1)) {
		ptr2 = ptr; goto SKIP_ALLOC; }
	count <<= 1; if(count == 0) count++;
	if(!(ptr2 = realloc(ptr, size*count))) {
		return ptr2; count_--; } ptr = ptr2;
SKIP_ALLOC:
	return ptr2+result;
}

// succeed or die allocators
SHITCALL2 Void xmalloc(size_t size){ if(size == 0) return NULL;	return errorAlloc( malloc(size) ); }
SHITCALL2 Void xcalloc(size_t size) { if(size == 0) return NULL; return errorAlloc(calloc(1, size)); }
SHITCALL2 Void xrealloc(Void& ptr, size_t size){ if(size == 0) { free_ref(ptr); return NULL; }
	return ptr = errorAlloc( realloc(ptr, size) ); }
SHITCALL2 Void xrecalloc(Void& ptr, size_t size){ if(ptr == NULL) return (ptr = xcalloc(size));
	return xrealloc(ptr, size); }
SHITCALL2 Void xmemdup8(Void src, int count) { if(count == 0) return NULL;	
	void* dst = xmalloc(count); memcpy8(dst, src, count); return dst; }
SHITCALL2 Void xmemdup16(Void src, int count) {	if(count == 0) return NULL;
	void* dst = xmalloc(count*2); memcpy16(dst, src, count); return dst; }
SHITCALL2 Void xmemdup32(Void src, int count) {	if(count == 0) return NULL;
	void* dst = xmalloc(count*4); memcpy32(dst, src, count); return dst; }
	
SHITCALL2
Void xnxalloc(Void& ptr, size_t& count_, size_t size)
{
	size_t result = movrl(b, count_); incml(count_);
	size_t count = movrl(d, result);
	int count_1 = movfx(a, result-1);
	result *= size; Void ptr2;
	if(likely(count & count_1)) {
		ptr2 = ptr; goto SKIP_ALLOC; }
	count <<= 1; if(count == 0) count++;
	ptr2 = xrealloc(ptr, size*count);
SKIP_ALLOC:
	return ptr2+result;
}






	
// xvector
Void xvector_::xalloc_(size_t size)
{
	this->dataSize = size;
	this->allocSize = size;
	return dataPtr = xmalloc(size);
}

Void xvector_::xreserve_(size_t size)
{
	if(this->allocSize >= size)
		return this->dataPtr;
	this->allocSize = size;
	return xrealloc(this->dataPtr, size);
}

Void xvector_::xresize_(size_t size)
{
	this->dataSize = size;
	ARGFIX(size);
	return this->xreserve_(size);
}

VoidLen REGCALL(3) xvector_alloc(
	size_t reqSize, size_t curSize, xvector_* This)
{
	size_t memSize = This->allocSize << 1;
	if(memSize < reqSize) memSize = reqSize;
	memSize = ALIGN(memSize, 7); This->allocSize = memSize;
	return VoidLen(xrealloc(This->dataPtr, memSize), curSize);
}

VoidLen xvector_::xnxalloc2_(size_t size)
{
	size_t curSize = this->dataSize;
	size_t reqSize = movfx(a, curSize + size);
	this->dataSize = reqSize;
	Void ptr2;
	if(likely(this->allocSize >= reqSize)) {
		ptr2 = this->dataPtr;goto SKIP_ALLOC; }
	{GET_RETPAIR(ptr2, curSize, xvector_alloc(
		reqSize, curSize, this))};
SKIP_ALLOC:
	return VoidLen(ptr2+curSize, curSize);
}

VoidLen xvector_::xrxalloc2_(size_t size)
{
	size_t curSize = this->dataSize;
	size_t reqSize = movfx(a, curSize + size);
	Void ptr2;
	if(likely(this->allocSize >= reqSize)) {
		ptr2 = this->dataPtr;goto SKIP_ALLOC; }
	{GET_RETPAIR(ptr2, curSize, xvector_alloc(
		reqSize, curSize, this))};
SKIP_ALLOC:
	return VoidLen(ptr2+curSize, curSize);
}

RetEdx<int> xvector_::write8(Void src,int count) { VoidLen vl = 
	xnxalloc2_(count); memcpy8((byte*)vl.ptr, src, count); return vl.offset; }
RetEdx<int> xvector_::write16(Void src,int count) { VoidLen vl = 
	xnxalloc2_(count*2); memcpy16((word*)vl.ptr, src, count); return vl.offset; }
RetEdx<int> xvector_::write32(Void src,int count) { VoidLen vl = 
	xnxalloc2_(count*4); memcpy32((uint*)vl.ptr, src, count); return vl.offset; }
RetEdx<int> xvector_::write8(byte val) { VoidLen vl = xnxalloc2_(1);
	*(byte*)vl.ptr = val; return vl.offset; }
RetEdx<int> xvector_::write16(word val) { VoidLen vl = xnxalloc2_(2);
	*(word*)vl.ptr = val; return vl.offset; }
RetEdx<int> xvector_::write32(uint val) { VoidLen vl = xnxalloc2_(4);
	*(uint*)vl.ptr = val; return vl.offset; }
RetEdx<int> xvector_::write64(INT64 val) { VoidLen vl = xnxalloc2_(8);
	*(INT64*)vl.ptr = val; return vl.offset; }
