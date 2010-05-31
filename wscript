#!/usr/bin/env python

srcdir = '.'
blddir = 'build'

def set_options(opt):
	opt.tool_options('compiler_cc')

def configure(conf):
	conf.check_tool('compiler_cc')
	if not conf.env.CC: conf.fatal('C compiler not found')

def build(bld):
	bld(
		features = ['cc', 'cstaticlib'],
		target   = 'tbl',
		source   = 'src/tbl.c',
	)

	bld(
		features     = ['cc', 'cprogram'],
		target       = 'test',
		source       = 'src/tbl_test.c',
		uselib_local = 'tbl'
	)
	
