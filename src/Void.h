// ultimate void pointer
// V1.13 03/04/2014 
// DeadFish Shitware, 2013-2014

#ifndef _VOIDPTR_H_
#define _VOIDPTR_H_
#include <stdio.h>

TMPL(T)
struct IndexRef { int offset; IndexRef() {}
	IndexRef(int in) { offset = in; }
	operator int() { return offset; }};

struct Void
{
	// assigment/casing
	char* data; Void() = default;
	TMPL(T) Void(T in, size_t o = 0)	{ data = ((char*)in) + o; }
	TMPL(T) Void operator=(T in)		{ data = (char*)in; return *this; }
	TMPL(T) operator T() 			{ return (T)data; }
	TMPL(T) operator T() const 		{ return (T)data; }
	
	// arithmetic
	Void operator++(int)			{	return this->data++; }
	Void operator--(int)			{	return this->data--; }
	Void operator+=(Void offset)	{	return data += size_t(offset.data); }
	Void operator-=(Void offset)	{	return data -= size_t(offset.data); }
	Void operator+(size_t offset)	{	return data + offset; }
	
	// data access, old style
	unsigned char &operator[] (size_t n)	{	return *(unsigned char*)(data + n); }
	unsigned char &operator*()				{	return *(unsigned char*)(data + 0); }
	TMPL(T) T*& ptr(void)		{	return *(T**)&data; }
	TMPL(T) T& ref(size_t n = 0)	{	return Ref<T>(0, n); }
	TMPL(T) T* ptr(size_t n)		{	return Ptr<T>(0, n); }
	BYTE& byte(size_t n = 0)				{	return Byte(0, n); }
	WORD& word(size_t n = 0)				{	return Word(0, n); }
	DWORD& dword(size_t n = 0)				{	return Dword(0, n); }
	size_t sizet(size_t n = 0)				{	return Sizet(0, n); }
	
	// data access advanced
	TMPL(T) T& Ref(size_t o, size_t n = 0)	{	return ((T*)(data+o))[n]; }
	TMPL(T) T* Ptr(size_t o, size_t n = 0)	{	return &((T*)(data+o))[n]; }
	BYTE& Byte(size_t o, size_t n = 0)		{	return ((BYTE*)(data+o))[n]; }
	WORD& Word(size_t o, size_t n = 0)		{	return ((WORD*)(data+o))[n]; }
	DWORD& Dword(size_t o, size_t n = 0)	{	return ((DWORD*)(data+o))[n]; }
	size_t& Sizet(size_t o, size_t n = 0)	{	return ((size_t*)(data+o))[n]; }

	// special
	int offset(Void ptr) {	return ptr.data-data; }
	void align(int size) { data = (char*)((size_t(data)	+ (size-1)) & ~(size-1)); }
	TMPL(T) T& operator()(IndexRef<T> ref) { return *(T*)(data+ref); }
};

// External operators
#define VOID_OPERATOR(retType) \
	TMPL2(T,U) enable_if_t< \
	is_cstyle_castable<T, size_t>::value \
	&& is_cstyle_castable<U, size_t>::value \
	&& (std::is_same<T, Void>::value \
	|| std::is_same<U, Void>::value), retType>
VOID_OPERATOR(Void) operator-(const T& a, const U& b) {
	return (char*)(a) - (char*)(b); }
VOID_OPERATOR(bool)	operator ==(const T& a, const U& b) {
	return (char*)(a) == (char*)(b); }
VOID_OPERATOR(bool)	operator !=(const T& a, const U& b) {
	return (char*)(a) != (char*)(b); }
VOID_OPERATOR(bool)	operator <(const T& a, const U& b) {
	return (char*)(a) < (char*)(b); }
VOID_OPERATOR(bool)	operator >(const T& a, const U& b) {
	return (char*)(a) > (char*)(b); }
VOID_OPERATOR(bool)	operator <=(const T& a, const U& b) {
	return (char*)(a) <= (char*)(b); }
VOID_OPERATOR(bool)	operator >=(const T& a, const U& b) {
	return (char*)(a) >= (char*)(b); }
	
// extra shit
DEF_RETPAIR(VoidLen, Void, ptr, int, offset);

#endif
