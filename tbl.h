/*
 * Copyright (c) 2010-2015 somemetricprefix <somemetricprefix+code@gmail.com>
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

#ifndef TBL_H
#define TBL_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Specifies how many levels of nesting are supported.
#define TBL_STACK_SIZE 256

enum tbl_error {
  TBL_E_NONE = 0,
  TBL_E_INVALID_DATA,
  TBL_E_CANCELED_BY_USER,
  TBL_E_NO_CALLBACKS,
  TBL_E_STACK_OVERFLOW,
  TBL_E_UNKNOWN
};

// Parsing is stopped if a callback does not return 0.
struct tbl_callbacks {
  int (*integer)   (void *ctx, int64_t value);
  int (*string)    (void *ctx, char *value, size_t length);

  int (*list_start)(void *ctx);
  int (*list_end)  (void *ctx);

  int (*dict_start)(void *ctx);
  int (*dict_key)  (void *ctx, char *key, size_t length);
  int (*dict_end)  (void *ctx);
};

// Parse bencode encoded string.
// Returns an error code. A return value TBL_E_NONE indicates sucess.
int tbl_parse(char *buf, size_t length, const struct tbl_callbacks *callbacks,
              void *ctx);

#ifdef __cplusplus
}
#endif

#endif  // TBL_H
