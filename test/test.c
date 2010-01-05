/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "tbl.h"

typedef struct str { size_t len; char *str; } str_t;

static int tbl_integer(void *ctx, long value)
{
	if (*(long *)ctx != value)
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

static void test_integer()
{
	char buf[8];
	long result;
	tbl_error_t err;

	/* positive integer */
	sprintf(buf, "i1234e");
	result = 1234;
	err = tbl_parse(&callbacks, &result, buf, buf + 6);
	assert(err == TBL_E_NONE);

	/* negative integer */
	sprintf(buf, "i-123e");
	result = -123;
	err = tbl_parse(&callbacks, &result, buf, buf + 6);
	assert(err == TBL_E_NONE);
	
	/* zero */
	sprintf(buf, "i0e");
	result = 0;
	err = tbl_parse(&callbacks, &result, buf, buf + 3);
	assert(err == TBL_E_NONE);

	/* no leading zeroes allowed */
	sprintf(buf, "i0012e");
	err = tbl_parse(&callbacks, &result, buf, buf + 6);
	assert(err != TBL_E_NONE);

	/* bad integers */
	sprintf(buf, "ia28ze");
	err = tbl_parse(&callbacks, &result, buf, buf + 6);
	assert(err != TBL_E_NONE);
	sprintf(buf, "i28e9e");
	err = tbl_parse(&callbacks, &result, buf, buf + 6);
	assert(err != TBL_E_NONE);

	/* missing 'e' */
	sprintf(buf, "i12345");
	err = tbl_parse(&callbacks, &result, buf, buf + 6);
	assert(err != TBL_E_NONE);

	/* empty buffer */
	err = tbl_parse(&callbacks, &result, buf, buf);
	assert(err != TBL_E_NONE);
}

static void test_string()
{
	char buf[8];
	str_t result;
	int err;

	/* simple string */
	sprintf(buf, "4:test");
	result.len = 4;
	result.str = "test";
	err = tbl_parse(&callbacks, &result, buf, buf + 6);
	assert(err == TBL_E_NONE);

	/* string too long */
	err = tbl_parse(&callbacks, &result, buf, buf + 5);
	assert(err != TBL_E_NONE);

	/* empty */
	sprintf(buf, "0:");
	result.len = 0;
	result.str = "";
	err = tbl_parse(&callbacks, &result, buf, buf + 2);
	assert(err == TBL_E_NONE);
}

static void test_list()
{
	/* TODO */
}

static void test_dict()
{
	/* TODO */
}

int main(void)
{
	test_integer();
	test_string();
	test_list();
	test_dict();
	return 0;
}
