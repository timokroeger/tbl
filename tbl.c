
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
static const int bencode_start = 5;
static const int bencode_first_final = 5;
static const int bencode_error = 0;

static const int bencode_en_main = 5;


#line 23 "tbl.rl"

static int parse(const char *buf, size_t length,
                 const struct tbl_callbacks *callbacks, void *ctx)
{
  bool integer_negative = false;
  int64_t integer_value = 0;

  int cs;
  const char *p = buf;
  const char *pe = p + length;
  const char *eof = pe;


#line 40 "tbl.c"
	{
	cs = bencode_start;
	}

#line 43 "tbl.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
tr4:
#line 44 "tbl.rl"
	{
    if (integer_negative) {
      integer_value = -integer_value;
    }

    if (callbacks->integer && callbacks->integer(ctx, integer_value) != 0) {
      return TBL_E_CANCELED_BY_USER;
    }
  }
	goto st5;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
#line 63 "tbl.c"
	if ( (*p) == 105 )
		goto st1;
	goto tr0;
tr0:
#line 36 "tbl.rl"
	{
    return TBL_E_INVALID_DATA;
  }
	goto st0;
#line 71 "tbl.c"
st0:
cs = 0;
	goto _out;
st1:
	if ( ++p == pe )
		goto _test_eof1;
case 1:
	switch( (*p) ) {
		case 45: goto tr1;
		case 48: goto st4;
	}
	if ( 49 <= (*p) && (*p) <= 57 )
		goto tr3;
	goto tr0;
tr1:
#line 56 "tbl.rl"
	{ integer_negative = true; }
	goto st2;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
#line 92 "tbl.c"
	if ( 49 <= (*p) && (*p) <= 57 )
		goto tr3;
	goto tr0;
tr3:
#line 40 "tbl.rl"
	{
    integer_value = integer_value * 10 + ((*p) - '0');
  }
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 104 "tbl.c"
	if ( (*p) == 101 )
		goto tr4;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr3;
	goto tr0;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
	if ( (*p) == 101 )
		goto tr4;
	goto tr0;
	}
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof1: cs = 1; goto _test_eof; 
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 

	_test_eof: {}
	if ( p == eof )
	{
	switch ( cs ) {
	case 1: 
	case 2: 
	case 3: 
	case 4: 
#line 36 "tbl.rl"
	{
    return TBL_E_INVALID_DATA;
  }
	break;
#line 135 "tbl.c"
	}
	}

	_out: {}
	}

#line 65 "tbl.rl"


  return TBL_E_NONE;
}

int tbl_parse(const char *buf, size_t length,
              const struct tbl_callbacks *callbacks, void *ctx)
{
  if (!callbacks) {
    return TBL_E_NO_CALLBACKS;
  }

  return parse(buf, length, callbacks, ctx);
}
