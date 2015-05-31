
#line 1 "tbl.rl"
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


#line 22 "tbl.rl"

#line 22 "tbl.c"
static const int bencode_start = 12;
static const int bencode_first_final = 12;
static const int bencode_error = 0;

static const int bencode_en_list_parser = 6;
static const int bencode_en_main = 12;


#line 23 "tbl.rl"

static int parse(char *buf, size_t length,
                 const struct tbl_callbacks *callbacks, void *ctx)
{
  bool integer_negative = false;
  int64_t integer_value = 0;

  int cs, top, stack[TBL_STACK_SIZE];
  char *p = buf;
  char *pe = p + length;
  char *eof = pe;


#line 41 "tbl.c"
	{
	cs = bencode_start;
	top = 0;
	}

#line 45 "tbl.c"
	{
	if ( p == pe )
		goto _test_eof;
	goto _resume;

_again:
	switch ( cs ) {
		case 12: goto st12;
		case 0: goto st0;
		case 1: goto st1;
		case 2: goto st2;
		case 3: goto st3;
		case 4: goto st4;
		case 5: goto st5;
		case 6: goto st6;
		case 7: goto st7;
		case 13: goto st13;
		case 8: goto st8;
		case 9: goto st9;
		case 10: goto st10;
		case 11: goto st11;
	default: break;
	}

	if ( ++p == pe )
		goto _test_eof;
_resume:
	switch ( cs )
	{
tr2:
#line 65 "tbl.rl"
	{
    // String larger than remaining buffer.
    if (integer_value >= pe - p) {
      return TBL_E_INVALID_DATA;
    }

    if (callbacks->string
        && callbacks->string(ctx, p + 1, (size_t)integer_value) != 0) {
      return TBL_E_CANCELED_BY_USER;
    }

    // Advance parser pointer by string lenght.
    p += integer_value;
  }
#line 60 "tbl.rl"
	{
    integer_negative = false;
    integer_value = 0;
  }
	goto st12;
tr6:
#line 50 "tbl.rl"
	{
    if (integer_negative) {
      integer_value = -integer_value;
    }

    if (callbacks->integer && callbacks->integer(ctx, integer_value) != 0) {
      return TBL_E_CANCELED_BY_USER;
    }
  }
#line 60 "tbl.rl"
	{
    integer_negative = false;
    integer_value = 0;
  }
	goto st12;
tr17:
#line 80 "tbl.rl"
	{
    if (callbacks->list_start && callbacks->list_start(ctx) != 0) {
      return TBL_E_CANCELED_BY_USER;
    }
  }
#line 101 "tbl.rl"
	{ {
    if (top >= TBL_STACK_SIZE) {
      return TBL_E_STACK_OVERFLOW;
    }
  {stack[top++] = 12; goto st6;}} }
	goto st12;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
#line 125 "tbl.c"
	switch( (*p) ) {
		case 105: goto st2;
		case 108: goto tr17;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr1;
	goto tr0;
tr0:
#line 42 "tbl.rl"
	{
    return TBL_E_INVALID_DATA;
  }
	goto st0;
#line 137 "tbl.c"
st0:
cs = 0;
	goto _out;
tr1:
#line 46 "tbl.rl"
	{
    integer_value = integer_value * 10 + ((*p) - '0');
  }
	goto st1;
st1:
	if ( ++p == pe )
		goto _test_eof1;
case 1:
#line 149 "tbl.c"
	if ( (*p) == 58 )
		goto tr2;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr1;
	goto tr0;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
	switch( (*p) ) {
		case 45: goto tr3;
		case 48: goto st5;
	}
	if ( 49 <= (*p) && (*p) <= 57 )
		goto tr5;
	goto tr0;
tr3:
#line 94 "tbl.rl"
	{ integer_negative = true; }
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 172 "tbl.c"
	if ( 49 <= (*p) && (*p) <= 57 )
		goto tr5;
	goto tr0;
tr5:
#line 46 "tbl.rl"
	{
    integer_value = integer_value * 10 + ((*p) - '0');
  }
	goto st4;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
#line 184 "tbl.c"
	if ( (*p) == 101 )
		goto tr6;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr5;
	goto tr0;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
	if ( (*p) == 101 )
		goto tr6;
	goto tr0;
tr11:
#line 65 "tbl.rl"
	{
    // String larger than remaining buffer.
    if (integer_value >= pe - p) {
      return TBL_E_INVALID_DATA;
    }

    if (callbacks->string
        && callbacks->string(ctx, p + 1, (size_t)integer_value) != 0) {
      return TBL_E_CANCELED_BY_USER;
    }

    // Advance parser pointer by string lenght.
    p += integer_value;
  }
#line 60 "tbl.rl"
	{
    integer_negative = false;
    integer_value = 0;
  }
	goto st6;
tr15:
#line 50 "tbl.rl"
	{
    if (integer_negative) {
      integer_value = -integer_value;
    }

    if (callbacks->integer && callbacks->integer(ctx, integer_value) != 0) {
      return TBL_E_CANCELED_BY_USER;
    }
  }
#line 60 "tbl.rl"
	{
    integer_negative = false;
    integer_value = 0;
  }
	goto st6;
tr10:
#line 80 "tbl.rl"
	{
    if (callbacks->list_start && callbacks->list_start(ctx) != 0) {
      return TBL_E_CANCELED_BY_USER;
    }
  }
#line 101 "tbl.rl"
	{ {
    if (top >= TBL_STACK_SIZE) {
      return TBL_E_STACK_OVERFLOW;
    }
  {stack[top++] = 6; goto st6;}} }
	goto st6;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
#line 247 "tbl.c"
	switch( (*p) ) {
		case 101: goto tr8;
		case 105: goto st8;
		case 108: goto tr10;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr7;
	goto tr0;
tr7:
#line 46 "tbl.rl"
	{
    integer_value = integer_value * 10 + ((*p) - '0');
  }
	goto st7;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
#line 264 "tbl.c"
	if ( (*p) == 58 )
		goto tr11;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr7;
	goto tr0;
tr8:
#line 86 "tbl.rl"
	{
    if (callbacks->list_end && callbacks->list_end(ctx) != 0) {
      return TBL_E_CANCELED_BY_USER;
    }
  }
#line 105 "tbl.rl"
	{ {cs = stack[--top];goto _again;} }
	goto st13;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
#line 281 "tbl.c"
	goto tr0;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
	switch( (*p) ) {
		case 45: goto tr12;
		case 48: goto st11;
	}
	if ( 49 <= (*p) && (*p) <= 57 )
		goto tr14;
	goto tr0;
tr12:
#line 94 "tbl.rl"
	{ integer_negative = true; }
	goto st9;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
#line 300 "tbl.c"
	if ( 49 <= (*p) && (*p) <= 57 )
		goto tr14;
	goto tr0;
tr14:
#line 46 "tbl.rl"
	{
    integer_value = integer_value * 10 + ((*p) - '0');
  }
	goto st10;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
#line 312 "tbl.c"
	if ( (*p) == 101 )
		goto tr15;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr14;
	goto tr0;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
	if ( (*p) == 101 )
		goto tr15;
	goto tr0;
	}
	_test_eof12: cs = 12; goto _test_eof; 
	_test_eof1: cs = 1; goto _test_eof; 
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
	_test_eof7: cs = 7; goto _test_eof; 
	_test_eof13: cs = 13; goto _test_eof; 
	_test_eof8: cs = 8; goto _test_eof; 
	_test_eof9: cs = 9; goto _test_eof; 
	_test_eof10: cs = 10; goto _test_eof; 
	_test_eof11: cs = 11; goto _test_eof; 

	_test_eof: {}
	if ( p == eof )
	{
	switch ( cs ) {
	case 1: 
	case 2: 
	case 3: 
	case 4: 
	case 5: 
	case 6: 
	case 7: 
	case 8: 
	case 9: 
	case 10: 
	case 11: 
#line 42 "tbl.rl"
	{
    return TBL_E_INVALID_DATA;
  }
	break;
#line 358 "tbl.c"
	}
	}

	_out: {}
	}

#line 111 "tbl.rl"


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
