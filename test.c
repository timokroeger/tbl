/*
 * Copyright (c) 2011 tbl developers
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tbl.h"

#define mu_assert(message, test) do { if (!(test)) return message; } while (0)
#define mu_run_test(test) do { char *message = test(); tests_run++; \
                               if (message) return message; } while (0)

static int tests_run;

typedef struct str {
  size_t len;
  char *str;
} str_t;

int pass(void *ctx) { return 0; };
int fail(void *ctx) { return 1; };

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

static struct tbl_callbacks callbacks;

static char *test_common()
{
  char *ptr = NULL;
  int err;

  err = tbl_parse(ptr, 0, &callbacks, NULL);
  mu_assert("empty buffer", err == TBL_E_NONE);

  err = tbl_parse(ptr, 8, NULL, NULL);
  mu_assert("no callbacks", err == TBL_E_NO_CALLBACKS);

  err = tbl_parse("ok", 2, &callbacks, NULL);
  mu_assert("malformed bencode", err == TBL_E_INVALID_DATA);

  return NULL;
}

static char *test_integer()
{
  long long result;
  int err;

  err = tbl_parse("i1234e", 6, &callbacks, &result);
  mu_assert("no callback triggered", err == TBL_E_NONE);

  callbacks.integer = verify_integer;

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
  int err;

  err = tbl_parse("4:test", 6, &callbacks, &result);
  mu_assert("no callback triggered", err == TBL_E_NONE);

  callbacks.string = verify_string;

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
  int err;

  callbacks.integer = NULL;
  callbacks.string = NULL;

  err = tbl_parse("le", 2, &callbacks, NULL);
  mu_assert("no start callback", err == TBL_E_NONE);

  err = tbl_parse("le", 2, &callbacks, NULL);
  mu_assert("no end callback", err == TBL_E_NONE);

  callbacks.list_start = fail;
  err = tbl_parse("le", 2, &callbacks, NULL);
  mu_assert("cancel start by user", err == TBL_E_CANCELED_BY_USER);
  callbacks.list_start = pass;

  callbacks.list_end = fail;
  err = tbl_parse("le", 2, &callbacks, NULL);
  mu_assert("cancel end by user", err == TBL_E_CANCELED_BY_USER);
  callbacks.list_end = pass;

  err = tbl_parse("le", 2, &callbacks, NULL);
  mu_assert("emtpy list", err == TBL_E_NONE);

  err = tbl_parse("l4:testi1234ee", 14, &callbacks, NULL);
  mu_assert("valid list", err == TBL_E_NONE);

  err = tbl_parse("lli2eel4:testee", 15, &callbacks, NULL);
  mu_assert("nested list", err == TBL_E_NONE);

  err = tbl_parse("l4:testi1234ee", 10, &callbacks, NULL);
  mu_assert("list overflows", err == TBL_E_INVALID_DATA);

  return NULL;
}

static char *test_dict()
{
  int err;

  err = tbl_parse("de", 2, &callbacks, NULL);
  mu_assert("no start callback", err == TBL_E_NONE);

  err = tbl_parse("de", 2, &callbacks, NULL);
  mu_assert("no end callback", err == TBL_E_NONE);

  callbacks.dict_start = fail;
  err = tbl_parse("de", 2, &callbacks, NULL);
  mu_assert("cancel start by user", err == TBL_E_CANCELED_BY_USER);
  callbacks.dict_start = pass;

  callbacks.dict_end = fail;
  err = tbl_parse("de", 2, &callbacks, NULL);
  mu_assert("cancel end by user", err == TBL_E_CANCELED_BY_USER);
  callbacks.dict_end = pass;

  err = tbl_parse("de", 2, &callbacks, NULL);
  mu_assert("emtpy dict", err == TBL_E_NONE);

  err = tbl_parse("d4:testi1234ee", 14, &callbacks, NULL);
  mu_assert("valid dict", err == TBL_E_NONE);

  err = tbl_parse("di23ei1234ee", 12, &callbacks, NULL);
  mu_assert("invalid key", err == TBL_E_INVALID_DATA);

  err = tbl_parse("d3:food3:bari34eee", 18, &callbacks, NULL);
  mu_assert("nested dict", err == TBL_E_NONE);

  err = tbl_parse("d4:testi1234ee", 10, &callbacks, NULL);
  mu_assert("dict overflows", err == TBL_E_INVALID_DATA);

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
