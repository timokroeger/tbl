/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "tbl.h"

#define RET_ERR(errtype) do { handle->err = errtype; return; } while(0)

struct tbl_handle {
	tbl_error_t err;
	const char *ptr;
	const char *end;
	void       *ctx;
};

/* "low level parsing", the callback is passed so parse_string can call either
 * the string callback or the dict key callback */
static void parse_integer(int (*event_fn)(void *ctx, long long value),
                          tbl_handle_t *handle);
static void parse_string(int (*event_fn)(void *ctx, char *value, size_t length),
                         tbl_handle_t *handle);
/* functions to parse container types */
static void parse_list(const tbl_callbacks_t *callbacks, tbl_handle_t *handle);
static void parse_dict(const tbl_callbacks_t *callbacks, tbl_handle_t *handle);
/* gets the first char of the buffer to decide which type has to be parsed */
static void parse_next(const tbl_callbacks_t *callbacks, tbl_handle_t *handle);

void parse_integer(int (*event_fn)(void *ctx, long long value),
                   tbl_handle_t *handle)
{
	long long value;
	void *ptr;
#ifndef WIN32
	char *endptr;
#endif

	ptr = memchr(handle->ptr, 'e', handle->end - handle->ptr);
	if (!ptr)
		RET_ERR(TBL_E_INVALID_DATA);

#ifdef WIN32
	value = _atoi64(handle->ptr);
#else
	value = strtoll(handle->ptr, &endptr, 10);
	if (endptr != ptr)
		RET_ERR(TBL_E_INVALID_DATA);
#endif
	if (value && (*handle->ptr == '0')) /* i0e is still valid */
		RET_ERR(TBL_E_INVALID_DATA);
	if (event_fn && event_fn(handle->ctx, value))
		RET_ERR(TBL_E_CANCELED_BY_USER);

	handle->ptr = (char *)ptr + 1; /* skip 'e' */
}

void parse_string(int (*event_fn)(void *ctx, char *value, size_t length),
                  tbl_handle_t *handle)
{
	size_t len;
	void *ptr;
	char *endptr;

	ptr = memchr(handle->ptr--, ':', handle->end - handle->ptr);
	if (!ptr)
		RET_ERR(TBL_E_INVALID_DATA);

	len = strtol(handle->ptr, &endptr, 10);
	if (endptr++ != ptr)
		RET_ERR(TBL_E_INVALID_DATA);
	if (endptr + len > handle->end)
		RET_ERR(TBL_E_INVALID_DATA);
	if (event_fn && event_fn(handle->ctx, endptr, len))
		RET_ERR(TBL_E_CANCELED_BY_USER);

	handle->ptr = endptr + len; /* jump to next token */
}

void parse_list(const tbl_callbacks_t *callbacks, tbl_handle_t  *handle)
{
	/* list start */
	if (callbacks->tbl_list_start)
		if (callbacks->tbl_list_start(handle->ctx))
			RET_ERR(TBL_E_CANCELED_BY_USER);
	/* entries */
	while(*handle->ptr != 'e') {
		parse_next(callbacks, handle);
		if (handle->err != TBL_E_NONE)
			return;
	}
	/* list end */
	if (callbacks->tbl_list_end && callbacks->tbl_list_end(handle->ctx))
		RET_ERR(TBL_E_CANCELED_BY_USER);

	handle->ptr++; /* skip 'e' */
}

void parse_dict(const tbl_callbacks_t *callbacks, tbl_handle_t  *handle)
{
	/* dict start */
	if (callbacks->tbl_dict_start && callbacks->tbl_dict_start(handle->ctx))
		RET_ERR(TBL_E_CANCELED_BY_USER);

	/* keys + entries */
	while(*handle->ptr != 'e') {
		parse_string(callbacks->tbl_dict_key, handle);
		if (handle->err != TBL_E_NONE)
			return;
		parse_next(callbacks, handle);
		if (handle->err != TBL_E_NONE)
			return;
	}
	/* dict end */
	if (callbacks->tbl_dict_end && callbacks->tbl_dict_end(handle->ctx))
		RET_ERR(TBL_E_CANCELED_BY_USER);

	handle->ptr++; /* skip 'e' */
}

void parse_next(const tbl_callbacks_t *callbacks, tbl_handle_t  *handle)
{
	char c = *handle->ptr++;

	if ((handle->ptr >= handle->end) || (!callbacks))
		RET_ERR(TBL_E_INVALID_DATA);

	/* get type of next entry */
	if (c == 'i')
		parse_integer(callbacks->tbl_integer, handle);
	else if (isdigit(c) != 0)
		parse_string(callbacks->tbl_string, handle);
	else if (c == 'l')
		parse_list(callbacks, handle);
	else if (c == 'd')
		parse_dict(callbacks, handle);
	else
		handle->err = TBL_E_INVALID_DATA;
}

tbl_error_t tbl_parse(const char *buf, const char *bufend,
                      const tbl_callbacks_t *callbacks, void *ctx)
{
	tbl_handle_t handle = { TBL_E_NONE, buf, bufend, ctx };
	if (!callbacks)
		return TBL_E_NO_CALLBACKS;

	while (handle.ptr < handle.end && handle.err == TBL_E_NONE)
		parse_next(callbacks, &handle);

	return handle.err;
}

