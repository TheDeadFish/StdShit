// rep movsx memcpy

asm(".macro movsn_ n; .if \\n>=4; movsd; movsn_ \\n-4; .elseif \\n>=2; movsw;"
	"movsn_ \\n-2; .elseif  \\n>=1; movsb; movsn_ \\n-1; .endif; .endm");
#define MOVSN_(d, s, n) asm volatile("movsn_ %a2" : "+D"(d), "+S"(s) : "i"(n))
#define MOVSNX_(d, s, c, n) ({int msnxc_; asm volatile("rep; movsn_ %a3" : \
	"+D"(d), "+S"(s), "=c"(msnxc_) : "i"(n), "c"(c));})

#define memcpyx_ref(sz, sz2) TMPL2(T,U) \
	ALWAYS_INLINE void memcpy##sz##_ref(T*& dst, U*& src, size_t count) { \
	if(__builtin_constant_p(count) && (((count*sz2/4)+((count*sz2)&3)) <= 6)) {\
	MOVSN_(dst, src, sz2*count); } else { MOVSNX_(dst,src,count,sz2);}} \
	TMPL(T) ALWAYS_INLINE T* memcpy##sz(T* dst, const void* src, size_t count) \
	{ memcpy##sz##_ref(dst, src, count); return dst;  }
memcpyx_ref(8, 1); memcpyx_ref(16, 2); memcpyx_ref(32, 4);

#define _memcpy_sz(macro) { \
	if((sizeof(T) % 4) == 0) macro(32, count*(sizeof(T)/4)) \
	ei((sizeof(T) % 2) == 0) macro(16, count*(sizeof(T)/2)) \
	else				 macro(8, count*sizeof(T)) }
#define _rep_memcpy_sz_(size, count) \
	memcpy##size##_ref(dst, src, count);
TMPL2(T,U) ALWAYS_INLINE
void memcpy_ref(T*& dst, U*& src, size_t count) {
	_memcpy_sz(_rep_memcpy_sz_); }
TMPL(T) ALWAYS_INLINE Void memcpyX(T* dst, const void* src,
	size_t count) { memcpy_ref(dst, src, count); return dst; }
TMPL(T) ALWAYS_INLINE Void memcpyY(T* dst, const void* src,
	size_t count) { memcpy_ref(dst, src, count); return src; }

// rep stosx memset
#define memsetx_ref(sz, inst, init...) template<class T,class U> \
	ALWAYS_INLINE void memset##sz##_ref(T*&dst,size_t count,const U&src){	init; \
	if(__builtin_constant_p(count) && (count <= 6)) while(count--) \
		asm volatile(#inst :"=D"(dst) :"a"(src),"D"(dst)); \
	else asm volatile("rep "#inst :"=D"(dst),"=c"(count) \
		:"a"(src),"D"(dst),"c"(count)); } template<class T,class U> \
	ALWAYS_INLINE T*memset##sz(T*dst,size_t count,const U&src){\
		memset##sz##_ref(dst,count,src);return dst; }
memsetx_ref(8, stosb, byte val = src); 
memsetx_ref(16, stosw, typename std::conditional<sizeof(U)<2
	,word,const U&>::type val=src;)
memsetx_ref(32, stosl, typedef typename std::conditional<std::
	is_same<double,U> ::value,float,const U&>::type type;
	typename std::conditional<sizeof(U)<4,uint,type>::type val=src)
	
// rep stosx memset overloads
template<class T,class U>
void memset_ref(T*&dst,size_t count,const U&src){
	if(sizeof(T) == 4)memset32_ref(dst,count,src);
	ei(sizeof(T) == 2)memset16_ref(dst,count,src);
	else	  		 memset8_ref(dst,count,src); }
template<class T,class U> T*memsetX(T*dst,size_t count,const U&src){
	memset_ref(dst,count,src);return dst; }
template <class T, class U, size_t N>
 void memsetX(T (*array)[N], size_t count, const U& val) {
	memsetX((T*)array, N*count, val); }
template <class T, class U, size_t N>
 void memsetX(T (&array)[N], const U& val) {
	memsetX(array, N, val); }
template <class T, class U, size_t N, size_t M> 
void memsetX(T (&array)[N][M], const U& val) {
	memsetX(array, N, val); }

// rep stosx memset filling/zeroing
#define _rep_memset_sz_(size, count) \
	memset##size##_ref(dst, count, fill);
template<class T>
void memfill_ref(T*&dst,size_t count,size_t fill){
	_memcpy_sz(_rep_memset_sz_); }
template<class T> 
T* memfillX(T*dst,size_t count,size_t fill = 0){
	memfill_ref(dst,count,fill);return dst; }
template<class T> void memfillX(T& obj, size_t fill = 0) {
	memfillX(&obj, 1, fill); }
#define memfill2(dst_, dstEnd, fill) {\
	uint byteLen = RangeOf(dst_, dstEnd); char* dst = (char*)&dst_;\
	if(!__builtin_constant_p(byteLen)) memset8_ref(dst,byteLen,(char)fill);\
	ei((byteLen % 4) == 0) memset32_ref(dst,byteLen/4,fill);\
	ei((byteLen % 2) == 0) memset16_ref(dst,byteLen/2,(word)fill);\
	else { memset8(dst,byteLen,(char)fill); }\
	__assume(dst == ((char*)&dst_)+byteLen); }
