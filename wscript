#!/usr/bin/env python

VERSION = '0.1'
APPNAME = 'tbl'

top = '.'
out = 'build'

def set_options(opt):
    opt.tool_options('compiler_cc')

def configure(conf):
    conf.check_tool('compiler_cc')

def build(bld):
    bld(
        features = ['cc', 'cstaticlib'],
        target   = 'tbl',
        source   = 'src/tbl.c',
    )

    bld(
        features     = ['cc', 'cprogram'],
        target       = 'test',
        source       = 'tests/tbl.c',
        includes     = 'src',
        uselib_local = 'tbl'
    )

