#include "stdshit.h"
#include "test.h"
const char progName[] = "algo test";

int sorted_buff1[] = { 1, 1, 1, 2, 3, 3, 3, 3, 4, 4, 5, 6, 6, 6, 7, 7, 7, 8, 8, 9 };
int sorted_buff2[] = { 1, 2, 2, 3, 3, 3, 4, 4, 4, 5, 6, 6, 7, 7, 7, 7, 8, 9, 9, 9 };

int bsearch_compar(const void* key, const void* elem)
{
	return (int)key - RI(elem);
}

cch* sorted_buff_name(int* base) {
	return (base == sorted_buff1)
		? "sorted_buff1" : "sorted_buff2";
}

void bsearch_check(xRngPtr<int> x, int* base, int key, int nKey)
{
	test_enter();
	if(x.count() != nKey) goto ERR;
	for(int i : x) { if(i != key) goto ERR; }
	if(0) { ERR:
		cch* name = sorted_buff_name(base);
		printf("bsearch: %d, %d, %d, %d, %s\n",
			key, nKey, x.data-base, x.count(), name);
		test_fail();
	}
}

void bsearch_test(void)
{
	#define TEST1(k, n1, n2) \
		bsearch_check(bsearch2((void*)k, sorted_buff1, bsearch_compar), sorted_buff1, k, n1);\
		bsearch_check(bsearch2((void*)k, sorted_buff2, bsearch_compar), sorted_buff2, k, n2);
	TEST1(1, 3, 1); TEST1(2, 1, 2); TEST1(3, 4, 3); TEST1(5, 1, 1);
	TEST1(6, 3, 2); TEST1(7, 3, 4); TEST1(8, 2, 1); TEST1(9, 1, 3);
	TEST1(-1, 0, 0); TEST1(11, 0, 0);

	test_print("bsearch");
}

void bsearch_lower_check(int* base, int key, int iKey)
{
	int i = bsearch_lower((void*)key, base, 20, bsearch_compar)-base;
	test_enter();
	if(i != iKey) {
		cch* name = sorted_buff_name(base);
		printf("bsearch_lower: %d, %d, %d, %s\n", key, iKey, i, name);
		test_fail();
	}
}

void bsearch_lower_test(void)
{
	#define TEST2(k, n1, n2) \
		bsearch_lower_check(sorted_buff1, k, n1);\
		bsearch_lower_check(sorted_buff2, k, n2);
	TEST2(1, 0, 0); TEST2(2, 3, 1); TEST2(3, 4, 3); TEST2(5, 10, 9);
	TEST2(6, 11, 10); TEST2(7, 14, 12); TEST2(8, 17, 16); TEST2(9, 19, 17);
	TEST2(-1, 0, 0); TEST2(11, 20, 20);

	test_print("bsearch_lower");
}

int main()
{
	bsearch_test();
	bsearch_lower_test();
}
