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

static int tbl_integer(void *ctx, long long value)
{
	if (*(long long *)ctx != value)
		return -1;
	return 0;
}

static int tbl_string(void *ctx, char *value, size_t length)
{
	str_t *str = (str_t *)ctx;
	if (str->len != length)
		return -1;
	else if (strncmp(str->str, value, length))
		return -1;
	return 0;
}

static tbl_callbacks_t callbacks = {
	tbl_integer,
	tbl_string,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

static char *test_common()
{
	char *ptr;
	long result;
	tbl_error_t err;

	err = tbl_parse(ptr, 0, &callbacks, &result);
	mu_assert("empty buffer", err == TBL_E_NONE);

	err = tbl_parse(ptr, 19, NULL, &result);
	mu_assert("no callbacks", err == TBL_E_NO_CALLBACKS);

	return NULL;
}

static char *test_integer()
{
	char buf[32];
	long long result;
	tbl_error_t err;

	sprintf(buf, "i1234e");
	result = 1234;
	err = tbl_parse(buf, 6, &callbacks, &result);
	mu_assert("positive integer", err == TBL_E_NONE);

	sprintf(buf, "i-123e");
	result = -123;
	err = tbl_parse(buf, 6, &callbacks, &result);
	mu_assert("nagtive integer", err == TBL_E_NONE);

	sprintf(buf, "i123456789123e");
	result = 123456789123;
	err = tbl_parse(buf, 14, &callbacks, &result);
	mu_assert("big integer", err == TBL_E_NONE);

	sprintf(buf, "i123456789123456789123456789e");
	err = tbl_parse(buf, 14, &callbacks, &result);
	mu_assert("too big integer", err == TBL_E_INVALID_DATA);

	sprintf(buf, "i0e");
	result = 0;
	err = tbl_parse(buf, 3, &callbacks, &result);
	mu_assert("zero", err != TBL_E_INVALID_DATA);

	sprintf(buf, "i0012e");
	err = tbl_parse(buf, 6, &callbacks, &result);
	mu_assert("no preceding zeroes allowed", err == TBL_E_INVALID_DATA);

	sprintf(buf, "ia28ze");
	err = tbl_parse(buf, 6, &callbacks, &result);
	mu_assert("malformed integer", err == TBL_E_INVALID_DATA);

	sprintf(buf, "i12345");
	err = tbl_parse(buf, 6, &callbacks, &result);
	mu_assert("missing 'e'", err == TBL_E_INVALID_DATA);

	return NULL;
}

static char *test_string()
{
	str_t result;
	int err;

	result.len = 4;
	result.str = "test";
	err = tbl_parse("4:test", 6, &callbacks, &result);
	mu_assert("simple string", err == TBL_E_NONE);

	err = tbl_parse("4:test", 5, &callbacks, &result);
	mu_assert("string too long", err == TBL_E_INVALID_DATA);

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
		printf("%s\n", result);
	} else {
		printf("ALL TEST PASSED\n");
	}
	printf("Tests run: %d\n", tests_run);

	return result != NULL;
}

