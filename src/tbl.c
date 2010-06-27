#include <ctype.h>
#include <errno.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

#include "tbl.h"

struct tbl_handle {
	jmp_buf    *err;
	const char *ptr;
	const char *end;
	void       *ctx;
};

/* the callback is passed so parse_string can call either the string callback or the dict key callback */
static void parse_string(int (*event_fn)(void *ctx, char *value, size_t length),
                         tbl_handle_t *handle);
/* functions to parse container types */
static void parse_integer(const tbl_callbacks_t *callbacks, tbl_handle_t *handle);
static void parse_list(const tbl_callbacks_t *callbacks, tbl_handle_t *handle);
static void parse_dict(const tbl_callbacks_t *callbacks, tbl_handle_t *handle);
/* gets the first char of the buffer to decide which type has to be parsed */
static void parse_next(const tbl_callbacks_t *callbacks, tbl_handle_t *handle);

static void parse_integer(const tbl_callbacks_t *callbacks, tbl_handle_t *handle)
{
	long long value;
	char *p, *q;

	q = memchr(handle->ptr, 'e', handle->end - handle->ptr);
	if (!q)
		longjmp(*handle->err, TBL_E_INVALID_DATA);

#ifdef HAVE_STRTOLL
	value = strtoll(handle->ptr, &p, 10);
#else
	p = (char *)handle->ptr;
	value = _atoi64(handle->ptr);
	/* dirty hack to look for the end of the number */
	while (*p == '-' || isdigit(*p))
		p++;
#endif

	if (p != q || errno == ERANGE)
		longjmp(*handle->err, TBL_E_INVALID_DATA);
	/* preceding 0 arent't allowed and i0e is still valid */
	if (value && *handle->ptr == '0')
		longjmp(*handle->err, TBL_E_INVALID_DATA);
	if (callbacks->tbl_integer && callbacks->tbl_integer(handle->ctx, value))
		longjmp(*handle->err, TBL_E_CANCELED_BY_USER);

	handle->ptr = q + 1; /* skip e */
}

void parse_string(int (*event_fn)(void *ctx, char *value, size_t length),
                  tbl_handle_t *handle)
{
	size_t len;
	char *ptr, *endptr;

	ptr = memchr(handle->ptr, ':', handle->end - handle->ptr);
	if (!ptr)
		longjmp(*handle->err, TBL_E_INVALID_DATA);

	len = strtol(handle->ptr, &endptr, 10);
	if (errno == ERANGE || endptr != ptr || ++endptr + len > handle->end)
		longjmp(*handle->err, TBL_E_INVALID_DATA);
	if (event_fn && event_fn(handle->ctx, endptr, len))
		longjmp(*handle->err, TBL_E_CANCELED_BY_USER);

	handle->ptr = endptr + len; /* jump to next token */
}

void parse_list(const tbl_callbacks_t *callbacks, tbl_handle_t *handle)
{
	/* list start */
	if (callbacks->tbl_list_start && callbacks->tbl_list_start(handle->ctx))
		longjmp(*handle->err, TBL_E_CANCELED_BY_USER);
	/* entries */
	while (*handle->ptr != 'e')
		parse_next(callbacks, handle);
	/* list end */
	if (callbacks->tbl_list_end && callbacks->tbl_list_end(handle->ctx))
		longjmp(*handle->err, TBL_E_CANCELED_BY_USER);

	handle->ptr++; /* skip 'e' */
}

void parse_dict(const tbl_callbacks_t *callbacks, tbl_handle_t *handle)
{
	/* dict start */
	if (callbacks->tbl_dict_start && callbacks->tbl_dict_start(handle->ctx))
		longjmp(*handle->err, TBL_E_CANCELED_BY_USER);

	/* keys + entries */
	while (*handle->ptr != 'e') {
		parse_string(callbacks->tbl_dict_key, handle);
		parse_next(callbacks, handle);
	}
	/* dict end */
	if (callbacks->tbl_dict_end && callbacks->tbl_dict_end(handle->ctx))
		longjmp(*handle->err, TBL_E_CANCELED_BY_USER);

	handle->ptr++; /* skip 'e' */
}

void parse_next(const tbl_callbacks_t *callbacks, tbl_handle_t *handle)
{
	char c = *handle->ptr++;

	if (handle->ptr >= handle->end)
		longjmp(*handle->err, TBL_E_INVALID_DATA);

	/* get type of next entry */
	if (c == 'i')
		parse_integer(callbacks, handle);
	else if (isdigit(c) != 0) {
		handle->ptr--; /* string has no prefix like i d or l to be skipped */
		parse_string(callbacks->tbl_string, handle);
	}
	else if (c == 'l')
		parse_list(callbacks, handle);
	else if (c == 'd')
		parse_dict(callbacks, handle);
	else
		longjmp(*handle->err, TBL_E_INVALID_DATA);
}

tbl_error_t tbl_parse(const char *buf, size_t lenght,
                      const tbl_callbacks_t *callbacks, void *ctx)
{
	jmp_buf env;
	tbl_error_t err;
	tbl_handle_t handle = { &env, buf, buf + lenght, ctx };

	if (!callbacks)
		return TBL_E_NO_CALLBACKS;

	err = setjmp(env);
	if (err == TBL_E_NONE && handle.ptr < handle.end)
		parse_next(callbacks, &handle);

	return err;
}

