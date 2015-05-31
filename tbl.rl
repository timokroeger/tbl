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

#include "tbl.h"

#include <stdbool.h>

%% machine bencode;
%% write data;

static int parse(char *buf, size_t length,
                 const struct tbl_callbacks *callbacks, void *ctx)
{
  bool integer_negative = false;
  int64_t integer_value = 0;

  int cs, top, stack[TBL_STACK_SIZE];
  char *p = buf;
  char *pe = p + length;
  char *eof = pe;

%%{
  prepush {
    if (top >= TBL_STACK_SIZE) {
      return TBL_E_STACK_OVERFLOW;
    }
  }

  action error {
    return TBL_E_INVALID_DATA;
  }

  action update {
    integer_value = integer_value * 10 + (fc - '0');
  }

  action integer_parsed {
    if (integer_negative) {
      integer_value = -integer_value;
    }

    if (callbacks->integer && callbacks->integer(ctx, integer_value) != 0) {
      return TBL_E_CANCELED_BY_USER;
    }
  }

  action reset_integer {
    integer_negative = false;
    integer_value = 0;
  }

  action check_string_size {
    // String larger than remaining buffer.
    if (integer_value >= pe - p) {
      return TBL_E_INVALID_DATA;
    }
  }

  action string_parsed {
    if (callbacks->string
        && callbacks->string(ctx, p + 1, (size_t)integer_value) != 0) {
      return TBL_E_CANCELED_BY_USER;
    }
  }

  action key_parsed {
    if (callbacks->dict_key
        && callbacks->dict_key(ctx, p + 1, (size_t)integer_value) != 0) {
      return TBL_E_CANCELED_BY_USER;
    }
  }

  action skip_string {
    // Advance parser pointer by string lenght.
    p += integer_value;
  }

  action list_start {
    if (callbacks->list_start && callbacks->list_start(ctx) != 0) {
      return TBL_E_CANCELED_BY_USER;
    }
  }

  action list_end {
    if (callbacks->list_end && callbacks->list_end(ctx) != 0) {
      return TBL_E_CANCELED_BY_USER;
    }
  }

  action dict_start {
    if (callbacks->dict_start && callbacks->dict_start(ctx) != 0) {
      return TBL_E_CANCELED_BY_USER;
    }
  }

  action dict_end {
    if (callbacks->dict_end && callbacks->dict_end(ctx) != 0) {
      return TBL_E_CANCELED_BY_USER;
    }
  }

  integer = 'i'
            ( '0'
            | ('-'? @{ integer_negative = true; }
              [1-9] @update ([0-9]@update)*)
            )
            'e' @integer_parsed @reset_integer;

  string = digit+ @update
           ':' @check_string_size @string_parsed @skip_string @reset_integer;

  dict_key = digit+ @update
             ':' @check_string_size @key_parsed @skip_string @reset_integer;

  list = 'l' @list_start @{ fcall list_parser; };

  dict = 'd' @dict_start @{ fcall dict_parser; };

  bencode = (integer | string | list | dict);

  list_parser := (bencode* 'e' @list_end @{ fret; }) $!error;

  dict_parser := ((dict_key bencode)* 'e' @dict_end @{ fret; }) $!error;

  main := bencode? $!error;

  write init;
  write exec;
}%%

  return TBL_E_NONE;
}

int tbl_parse(char *buf, size_t length, const struct tbl_callbacks *callbacks,
              void *ctx)
{
  if (!callbacks) {
    return TBL_E_NO_CALLBACKS;
  }

  return parse(buf, length, callbacks, ctx);
}
