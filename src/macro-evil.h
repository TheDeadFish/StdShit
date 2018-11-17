// basic stuff
#define MCAT2(name, ...) name##__VA_ARGS__
#define MCAT(name, ...) MCAT2(name, __VA_ARGS__)
#define EXTRACT(...) EXTRACT __VA_ARGS__
#define NOTHING_EXTRACT
#define UNPAREN(x) MCAT(NOTHING_, EXTRACT x)
#define MSTR(x) #x

// varidac macros, by Anonymous Coward
// http://stackoverflow.com/a/6936114
#define VA_NARGS_IMPL(_1, _2, _3, _4, _5, N, ...) N
#define VA_NARGS(...) VA_NARGS_IMPL(X,##__VA_ARGS__, 4, 3, 2, 1, 0)
#define VARARG_IMPL2(base, count, ...) base##count(__VA_ARGS__)
#define VARARG_IMPL(base, count, ...) VARARG_IMPL2(base, count, __VA_ARGS__) 
#define VARARG(base, ...) VARARG_IMPL(base, VA_NARGS(__VA_ARGS__), __VA_ARGS__)

// conditional logic
#define _MIF_0(a, b) UNPAREN(b)
#define _MIF_1(a, b) UNPAREN(a)
#define MIF(c,a,b) MCAT(_MIF_, c)(a,b)

// recursive eval
#define M_EVAL(...) MEVALA(__VA_ARGS__)
#define MEVALA(...) MEVAL9(MEVAL9(__VA_ARGS__))
#define MEVAL9(...) MEVAL8(MEVAL8(__VA_ARGS__))
#define MEVAL8(...) MEVAL7(MEVAL7(__VA_ARGS__))
#define MEVAL7(...) MEVAL6(MEVAL6(__VA_ARGS__))
#define MEVAL6(...) MEVAL5(MEVAL5(__VA_ARGS__))
#define MEVAL5(...) MEVAL4(MEVAL4(__VA_ARGS__))
#define MEVAL4(...) MEVAL3(MEVAL3(__VA_ARGS__))
#define MEVAL3(...) MEVAL2(MEVAL2(__VA_ARGS__))
#define MEVAL2(...) MEVAL1(MEVAL1(__VA_ARGS__))
#define MEVAL1(...) MEVAL0(MEVAL0(__VA_ARGS__))
#define MEVAL0(...) __VA_ARGS__

// defer for recursion
#define M_EMT()
#define M_DEFER1(id) id M_EMT()
#define M_DEFER2(id) id M_EMT M_EMT()()

