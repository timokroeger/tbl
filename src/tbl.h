/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

#ifndef TBL_H
#define TBL_H

#include <stddef.h> /* size_t */

#if defined(_WIN32) && defined(TBL_SHARED)
#  ifdef TBL_BUILD
#    define TBL_API __declspec(dllexport)
#  else
#    define TBL_API __declspec(dllimport)
#  endif
#else
#  define TBL_API
#endif

#ifdef __cplusplus
extern C {
#endif

typedef enum {
	TBL_E_NONE,
	TBL_E_INVALID_DATA,
	TBL_E_CANCELED_BY_USER,
	TBL_E_UNKNOWN
} tbl_error_t;

typedef struct tbl_handle tbl_handle_t;

/* parsing is stopped if a callback returns something else than 0 */
typedef struct tbl_callbacks {
	int (*tbl_integer)   (void *ctx, long value);
	int (*tbl_string)    (void *ctx, char *value, size_t length);

	int (*tbl_list_start)(void *ctx);
	int (*tbl_list_end)  (void *ctx);

	int (*tbl_dict_start)(void *ctx);
	int (*tbl_dict_key)  (void *ctx, char *key, size_t length);
	int (*tbl_dict_end)  (void *ctx);
} tbl_callbacks_t;

TBL_API tbl_error_t tbl_parse(const tbl_callbacks_t *callbacks,
                              void                  *ctx,
                              const char            *buf,
                              const char            *bufend);

/* only prototypes; nothing of this is implemented yet */
/*
TBL_API int tbl_gen_integer(tbl_handle_t *handle, long value);
TBL_API int tbl_gen_string(tbl_handle_t *handle, const char *str, size_t len);
TBL_API int tbl_gen_dict_open(tbl_handle_t *handle);
TBL_API int tbl_gen_dict_close(tbl_handle_t *handle);
TBL_API int tbl_gen_list_open(tbl_handle_t *handle);
TBL_API int tbl_gen_list_close(tbl_handle_t *handle);
*/

#ifdef __cplusplus
}
#endif

#endif /* TBL_H */
