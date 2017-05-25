// supliment for <algorithm>
// many of its functions are completly useless
// who the hell designed stl, total stupidity

#ifndef _DF_ALGORITHM_H_
#define _DF_ALGORITHM_H_
#include <algorithm>

#ifndef no_compar
#define DEF_HAS_METHOD(func) \
	template<typename, typename T> struct has_##func {}; \
	template<typename C, typename Ret, typename... Args> \
	struct has_##func<C, Ret(Args...)> { private: \
	template<typename T> static constexpr auto check(T*) -> typename \
	std::is_same<decltype( std::declval<T>().func(	\
		std::declval<Args>()... ) ),Ret>::type; \
    template<typename> static constexpr std::false_type check(...); \
    typedef decltype(check<C>(0)) type; public: \
	static constexpr bool value = type::value; };
	DEF_HAS_METHOD(compar);
	
	template <bool>	struct _DFCompar_;
	template <>	struct _DFCompar_<true> {
	TMPL2(T,U) static int comp(const T& a, const U& b) {
		return a.compar(b); }};
	template <> struct _DFCompar_<false> {
	TMPL2(T,U) static int comp(const T& a, const U& b) { 
		if(b < a) return 1;	if(a < b) return -1;
		return 0; }};
	TMPL2(T,U) int comp_fwd(const T& a, const U& b) {
		return _DFCompar_<has_compar<T,
		int(const T&)>::value>::comp(a, b); }
	static int comp_fwd(const int& a, const int& b) 
		{ return a - b; }
	TMPL2(T,U) int comp_rev(const T& a, const U& b)
		{	return comp_fwd(b, a); }
		
	// qsort / bsearch overloads
	TMPL(T) void qsort(T* base, size_t num) {
		qsort(base, num, comp_fwd<T, T>); }
	TMPL(T) void qsort_rev(T* base, size_t num) {
		qsort(base, num, comp_rev<T, T>); }
	TMPL(T) T* bsearch(void* key, T* base, size_t num) {
		return bsearch(base, num, comp_fwd<T, T>); }
	TMPL(T) T* bsearch_rev(void* key, T* base, size_t num) {
		return bsearch(base, num, comp_rev<T, T>); }
		
	// binary_search replacement
	#define comp_ltn(type1, type2, comp)\
		[&](type1 a, type2 b) {	return comp(a, b) < 0; }
	template<typename _ForwardIterator, typename _Tp>
	_ForwardIterator binary_find (_ForwardIterator __first, _ForwardIterator __last, const _Tp& __val) 
	{	typedef typename std::iterator_traits<_ForwardIterator>::value_type	_ValueType;
		__first = std::lower_bound(__first,__last, __val,comp_ltn(_ValueType&, const _Tp&, 
			(comp_fwd<typeof(*__first), _Tp>)));
		return (__first!=__last && !comp_fwd(*__first, __val)) ? __first : __last; }
	template<typename _ForwardIterator, typename _Tp, typename _Compare>	
	_ForwardIterator binary_find (_ForwardIterator __first, _ForwardIterator __last,
	const _Tp& __val, _Compare __comp) 	{ 
		typedef typename std::iterator_traits<_ForwardIterator>::value_type	_ValueType;
		__first = std::lower_bound(__first,__last, __val,comp_ltn(_ValueType&, const _Tp&, __comp));
		return (__first!=__last && !comp_fwd<typeof(*__first),_Tp>(*__first, __val)) ? __first : __last; }
	template<typename _ForwardIterator, typename _Tp>
	size_t binary_find (_ForwardIterator __first, size_t length, const _Tp& __val) 
	{	return binary_find(__first, __first+length, __val)-__first; }
	template<typename _ForwardIterator, typename _Tp, typename _Compare>	
	size_t binary_find (_ForwardIterator __first, size_t length, const _Tp& __val, _Compare __comp) {
		return binary_find(__first, __first+length, __val, __comp)-__first; }
#endif

#ifndef no_lowerSect
	// upper_sect & lower_sect
	// these functions return the interior enclosed by the boundary
	// of upper_bound and lower_bound
	template<typename _ForwardIterator, typename _Tp, typename _Compare>
	_ForwardIterator lower_sect(_ForwardIterator __first, _ForwardIterator __last,
		const _Tp& __val, _Compare __comp)
	{
      typedef typename std::iterator_traits<_ForwardIterator>::value_type
	_ValueType;
      typedef typename std::iterator_traits<_ForwardIterator>::difference_type
	_DistanceType;
      _DistanceType __len = std::distance(__first, __last);

	_ForwardIterator __middle = __first;
      while (__len > 0)
	{
	  _DistanceType __half = __len >> 1;
	  std::advance(__middle, __half);
	  if (__comp(*__middle, __val))
	    {
	      __first = __middle;
	      __len = __len - __half - 1;
	    }
	  else
	    __len = __half;
	  __middle = __first+1;
	}
      return __first;
	}
	
	template<typename _ForwardIterator, typename _Tp, typename _Compare>
	_ForwardIterator
	upper_sect(_ForwardIterator __first, _ForwardIterator __last,
		const _Tp& __val, _Compare __comp)
    {
      typedef typename std::iterator_traits<_ForwardIterator>::value_type
	_ValueType;
      typedef typename std::iterator_traits<_ForwardIterator>::difference_type
	_DistanceType;
      _DistanceType __len = std::distance(__first, __last);

	_ForwardIterator __middle = __first;
      while (__len > 0)
	{
	  _DistanceType __half = __len >> 1;
	  std::advance(__middle, __half);
	  if (__comp(__val, *__middle))
	    __len = __half;
	  else
	    {
	      __first = __middle;
	      __len = __len - __half - 1;
	    }
	  __middle = __first+1;
	}
      return __first;
	}
	
	// lower_sect overloads
	TMPL2(T,U)
	bool lower_sect_compare(const T& a, const U& b)
	{	return a < b; }
	template<typename _ForwardIterator, typename _Tp>
	_ForwardIterator lower_sect(_ForwardIterator __first,
		_ForwardIterator __last, const _Tp& __val)
	{	return lower_sect(__first, __last, __val, 
		lower_sect_compare<typename std::iterator_traits
		<_ForwardIterator>::value_type, _Tp>); }	
	template<typename _ForwardIterator, typename _Tp>
    size_t lower_sect(_ForwardIterator __first, size_t length, const _Tp& __val)
	{	return lower_sect(__first, __first+length, __val)-__first; }
	template<typename _ForwardIterator, typename _Tp, typename _Compare>
    size_t lower_sect(_ForwardIterator __first, size_t length, const _Tp& __val,  _Compare __comp)
	{	return lower_sect(__first, __first+length, __val, __comp)-__first; }
	
	// upper_sect overloads
	template<typename _ForwardIterator, typename _Tp>
	_ForwardIterator upper_sect(_ForwardIterator __first,
		_ForwardIterator __last, const _Tp& __val)
	{	return upper_sect(__first, __last, __val, 
		lower_sect_compare<typename std::iterator_traits
		<_ForwardIterator>::value_type, _Tp>); }	
	template<typename _ForwardIterator, typename _Tp>
    size_t upper_sect(_ForwardIterator __first, size_t length, const _Tp& __val)
	{	return upper_sect(__first, __first+length, __val)-__first; }
	template<typename _ForwardIterator, typename _Tp, typename _Compare>
    size_t upper_sect(_ForwardIterator __first, size_t length, const _Tp& __val,  _Compare __comp)
	{	return upper_sect(__first, __first+length, __val, __comp)-__first; }
#endif
	
	// lower_bound & upper_bound overloads
	template<typename _ForwardIterator, typename _Tp>
    size_t lower_bound(_ForwardIterator __first, size_t length, const _Tp& __val)
	{	return std::lower_bound(__first, __first+length, __val)-__first; }
	template<typename _ForwardIterator, typename _Tp, typename _Compare>
    size_t lower_bound(_ForwardIterator __first, size_t length, const _Tp& __val,  _Compare __comp)
	{	return std::lower_bound(__first, __first+length, __val, __comp)-__first; }
	template<typename _ForwardIterator, typename _Tp>
    size_t upper_bound(_ForwardIterator __first, size_t length, const _Tp& __val)
	{	return std::upper_bound(__first, __first+length, __val)-__first; }
	template<typename _ForwardIterator, typename _Tp, typename _Compare>
    size_t upper_bound(_ForwardIterator __first, size_t length, const _Tp& __val,  _Compare __comp)
	{	return std::upper_bound(__first, __first+length, __val, __comp)-__first; }
	
	// std::find replacements, iterators suck
	TMPL2(T, U) int find(T* ptr, int count, const U& value) 
	{ for(int i = 0; i < count; i++) if(ptr[i] == value) return i; return -1; }

#endif
