/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minunit.h"
#include "tbl.h"

int tests_run;

typedef struct str {
	size_t len;
	char *str;
} str_t;

static int verify_integer(void *ctx, long long value)
{
	if (*(long long *)ctx != value)
		return -1;
	return 0;
}

static int verify_string(void *ctx, char *value, size_t length)
{
	str_t *str = (str_t *)ctx;
	if (str->len != length)
		return -1;
	else if (strncmp(str->str, value, length))
		return -1;
	return 0;
}

static tbl_callbacks_t callbacks;

static char *test_common()
{
	char *ptr;
	tbl_error_t err;

	err = tbl_parse(ptr, 0, &callbacks, NULL);
	mu_assert("empty buffer", err == TBL_E_NONE);

	err = tbl_parse(ptr, 8, NULL, NULL);
	mu_assert("no callbacks", err == TBL_E_NO_CALLBACKS);

	return NULL;
}

static char *test_integer()
{
	long long result;
	tbl_error_t err;

	err = tbl_parse("i1234e", 6, &callbacks, &result);
	mu_assert("no callback triggered", err == TBL_E_NONE);

	callbacks.tbl_integer = verify_integer;

	result = 1234;
	err = tbl_parse("i1234e", 6, &callbacks, &result);
	mu_assert("positive integer", err == TBL_E_NONE);

	result = -123;
	err = tbl_parse("i-123e", 6, &callbacks, &result);
	mu_assert("nagtive integer", err == TBL_E_NONE);

	result = 123456789123;
	err = tbl_parse("i123456789123e", 14, &callbacks, &result);
	mu_assert("big integer", err == TBL_E_NONE);

	result = 0;
	err = tbl_parse("i0e", 3, &callbacks, &result);
	mu_assert("zero", err == TBL_E_NONE);

	result = 4321;
	err = tbl_parse("i1234e", 6, &callbacks, &result);
	mu_assert("cancel by user", err == TBL_E_CANCELED_BY_USER);

	err = tbl_parse("i1234567891234567891234567e", 14, &callbacks, &result);
	mu_assert("too big integer", err == TBL_E_INVALID_DATA);

	err = tbl_parse("i0012e", 6, &callbacks, &result);
	mu_assert("no preceding zeroes allowed", err == TBL_E_INVALID_DATA);

	err = tbl_parse("ia28ze", 6, &callbacks, &result);
	mu_assert("malformed integer", err == TBL_E_INVALID_DATA);

	err = tbl_parse("i12345", 6, &callbacks, &result);
	mu_assert("missing 'e'", err == TBL_E_INVALID_DATA);

	return NULL;
}

static char *test_string()
{
	str_t result;
	tbl_error_t err;

	err = tbl_parse("4:test", 6, &callbacks, &result);
	mu_assert("no callback triggered", err == TBL_E_NONE);

	callbacks.tbl_string = verify_string;

	result.len = 4;
	result.str = "test";
	err = tbl_parse("4:test", 6, &callbacks, &result);
	mu_assert("simple string", err == TBL_E_NONE);

	err = tbl_parse("4:test", 5, &callbacks, &result);
	mu_assert("string too long", err == TBL_E_INVALID_DATA);

	err = tbl_parse("4test", 5, &callbacks, &result);
	mu_assert("no : seperator", err == TBL_E_INVALID_DATA);

	result.str = "lolwat";
	err = tbl_parse("4:test", 6, &callbacks, &result);
	mu_assert("cancel by user", err == TBL_E_CANCELED_BY_USER);

	result.len = 0;
	result.str = "";
	err = tbl_parse("0:", 2, &callbacks, &result);
	mu_assert("emtpy", err == TBL_E_NONE);

	return NULL;
}

static char *test_list()
{
	tbl_error_t err;

	err = tbl_parse("le", 2, &callbacks, NULL);
	mu_assert("emtpy list", err == TBL_E_NONE);

	return NULL;
}

static char *test_dict()
{
	tbl_error_t err;

	err = tbl_parse("de", 2, &callbacks, NULL);
	mu_assert("emtpy dictionary", err == TBL_E_NONE);

	return NULL;
}

static char *all_tests()
{
	mu_run_test(test_common);
	mu_run_test(test_integer);
	mu_run_test(test_string);
	mu_run_test(test_list);
	mu_run_test(test_dict);

	return NULL;
}

int main(int argc, char *argv[])
{
	char *result = all_tests();
	if (result != 0) {
		printf("error: %s\n", result);
	} else {
		printf("ALL TEST PASSED\n");
	}
	printf("Tests run: %d\n", tests_run);

	return result != NULL;
}
