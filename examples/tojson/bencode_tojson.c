/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

#include <stdio.h>
#include <stdlib.h>
#include <yajl/yajl_gen.h>

#include "tbl.h"

typedef struct str { size_t len; char *str; } str_t;

static int tbl_integer(void *ctx, long value)
{
	return 0;
}

static int tbl_string(void *ctx, char *value, size_t length)
{
	return 0;
}

static int tbl_list_start(void *ctx)
{
	return 0;
}

static int tbl_list_end(void *ctx)
{
	return 0;
}

static int tbl_dict_start(void *ctx)
{
	return 0;
}

static int tbl_dict_key(void *ctx, char *value, size_t length)
{
	return 0;
}

static int tbl_dict_end(void *ctx)
{
	return 0;
}


static tbl_callbacks_t callbacks = {
	tbl_integer,
	tbl_string,
	tbl_list_start,
	tbl_list_end,
	tbl_dict_start,
	tbl_dict_key,
	tbl_dict_end
};

int main(int argc, char *argv[])
{
	FILE *file;
	size_t filesize;
	char *buf;
	yajl_gen gen;
	yajl_gen_config config = { 1, "  " };
	
	if (argc != 2)
		return -1;
	
	fseek(file, 0, SEEK_END);
	filesize = ftell(file);
	rewind(file);
	
	buf = malloc(filesize);
	if (!buf)
		return -1;
	if (fread(buf, filesize, 1, file) != 1)
		return -1;
	
	gen = yajl_gen_alloc(&config, NULL);
	return tbl_parse(&callbacks, gen, buf, buf + filesize);
	
	return 0;
}
