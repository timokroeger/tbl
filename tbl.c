/*
 * Copyright (c) 2010-2013 somemetricprefix <somemetricprefix+code@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
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

#include <ctype.h>
#include <errno.h>
#include <setjmp.h>
#include <string.h>

#include "tbl.h"

struct tbl_handle {
  jmp_buf    *err;
  const char *ptr;
  const char *end;
  void       *ctx;
};

/* the callback is passed so parse_string can call either the string callback or
 * the dict key callback */
static void parse_string(int (*event_fn)(void *ctx, char *value, size_t length),
                         struct tbl_handle *handle);
/* functions to parse container types */
static void parse_integer(const struct tbl_callbacks *callbacks, struct tbl_handle *handle);
static void parse_list(const struct tbl_callbacks *callbacks, struct tbl_handle *handle);
static void parse_dict(const struct tbl_callbacks *callbacks, struct tbl_handle *handle);
/* gets the first char of the buffer to decide which type has to be parsed */
static void parse_next(const struct tbl_callbacks *callbacks, struct tbl_handle *handle);

static void parse_integer(const struct tbl_callbacks *callbacks, struct tbl_handle *handle)
{
  long long value = 0;
  const char *p, *q;
  int sign;

  p = handle->ptr;
  q = (const char *)memchr(handle->ptr, 'e', handle->end - handle->ptr);
  if (!q)
    longjmp(*handle->err, TBL_E_INVALID_DATA);

  /* watch out for negative numbers */
  sign = *p == '-' ? -1 : 1;
  *p == '-' && p++;

  while (p < q && isdigit(*p)) {
    value *= 10;
	value += *p - '0';
	p++;
  }
  value *= sign;

  if (p != q)
    longjmp(*handle->err, TBL_E_INVALID_DATA);
  /* preceding 0 arent't allowed and i0e is still valid */
  if (value && *handle->ptr == '0')
    longjmp(*handle->err, TBL_E_INVALID_DATA);
  if (callbacks->integer && callbacks->integer(handle->ctx, value))
    longjmp(*handle->err, TBL_E_CANCELED_BY_USER);

  handle->ptr = q + 1; /* skip e */
}

void parse_string(int (*event_fn)(void *ctx, char *value, size_t length),
                  struct tbl_handle *handle)
{
  size_t len;
  char *ptr, *endptr;

  ptr = (char *)memchr(handle->ptr, ':', handle->end - handle->ptr);
  if (!ptr)
    longjmp(*handle->err, TBL_E_INVALID_DATA);

  len = strtol(handle->ptr, &endptr, 10);
  if (errno == ERANGE || endptr != ptr || ++endptr + len > handle->end)
    longjmp(*handle->err, TBL_E_INVALID_DATA);
  if (event_fn && event_fn(handle->ctx, endptr, len))
    longjmp(*handle->err, TBL_E_CANCELED_BY_USER);

  handle->ptr = endptr + len; /* jump to next token */
}

void parse_list(const struct tbl_callbacks *callbacks, struct tbl_handle *handle)
{
  /* list start */
  if (callbacks->list_start && callbacks->list_start(handle->ctx))
    longjmp(*handle->err, TBL_E_CANCELED_BY_USER);
  /* entries */
  while (*handle->ptr != 'e')
    parse_next(callbacks, handle);
  /* list end */
  if (callbacks->list_end && callbacks->list_end(handle->ctx))
    longjmp(*handle->err, TBL_E_CANCELED_BY_USER);

  handle->ptr++; /* skip 'e' */
}

void parse_dict(const struct tbl_callbacks *callbacks, struct tbl_handle *handle)
{
  /* dict start */
  if (callbacks->dict_start && callbacks->dict_start(handle->ctx))
    longjmp(*handle->err, TBL_E_CANCELED_BY_USER);

  /* keys + entries */
  while (*handle->ptr != 'e') {
    parse_string(callbacks->dict_key, handle);
    parse_next(callbacks, handle);
  }
  /* dict end */
  if (callbacks->dict_end && callbacks->dict_end(handle->ctx))
    longjmp(*handle->err, TBL_E_CANCELED_BY_USER);

  handle->ptr++; /* skip 'e' */
}

void parse_next(const struct tbl_callbacks *callbacks, struct tbl_handle *handle)
{
  char c = *handle->ptr++;

  if (handle->ptr >= handle->end)
    longjmp(*handle->err, TBL_E_INVALID_DATA);

  /* get type of next entry */
  if (c == 'i')
    parse_integer(callbacks, handle);
  else if (isdigit(c) != 0) {
    handle->ptr--; /* string has no prefix like i d or l to be skipped */
    parse_string(callbacks->string, handle);
  }
  else if (c == 'l')
    parse_list(callbacks, handle);
  else if (c == 'd')
    parse_dict(callbacks, handle);
  else
    longjmp(*handle->err, TBL_E_INVALID_DATA);
}

int tbl_parse(const char *buf, size_t lenght,
              const struct tbl_callbacks *callbacks, void *ctx)
{
  jmp_buf env;
  int err;
  struct tbl_handle handle = { &env, buf, buf + lenght, ctx };

  if (!callbacks)
    return TBL_E_NO_CALLBACKS;

  err = setjmp(env);
  if (err == TBL_E_NONE && handle.ptr < handle.end)
    parse_next(callbacks, &handle);

  return err;
}
