#pragma once

static int test_enter_count;
static int test_fail_count;
static void test_enter(void) { test_enter_count ++; }
static void test_fail(void) { test_fail_count ++; }
static void test_print(cch* name) {
	printf("%s: %d, %d", name, test_enter_count, test_fail_count);
	if(test_fail_count) printf(" - FAIL\n");
	else printf(" - PASS\n");
	test_enter_count = 0; test_fail_count = 0;
}
