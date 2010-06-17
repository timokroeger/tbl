#!/usr/bin/env python

VERSION = '0.1'
APPNAME = 'tbl'

top = '.'
out = 'build'

import Options

def set_options(opt):
    opt.tool_options('compiler_cc')
    opt.add_option(
        '--debug',
        action = 'store_true',
        default = False,
        help = 'Build debug variant [Default: False]',
        dest = 'debug'
    )
    opt.add_option(
        '--release',
        action = 'store_true',
        default = False,
        help = 'Build release variant [Default: False]',
        dest = 'release'
    )

def configure(conf):
    conf.check_tool('compiler_cc')
    if conf.check(function_name = 'strtoll', header_name = 'stdlib.h'):
        conf.env.append_value('CCFLAGS', '-DHAVE_STRTOLL')

    debug_env = conf.env.copy()
    debug_env.set_variant('debug')
    debug_env.append_value('CCFLAGS', ['-DDEBUG', '-g', '-O0', '-Wall', '-Wextra'])
    conf.set_env_name('debug', debug_env)

    release_env = conf.env.copy()
    release_env.set_variant('release')
    release_env.append_value('CCFLAGS', ['-DNDEBUG', '-O3'])
    conf.set_env_name('release', release_env)

def build(bld):
    tbl = bld(
        features = ['cc', 'cstaticlib'],
        target   = 'tbl',
        source   = 'src/tbl.c',
    )

    test = bld(
        features     = ['cc', 'cprogram'],
        target       = 'test',
        source       = 'tests/tbl.c',
        includes     = 'src',
        uselib_local = 'tbl'
    )

    if Options.options.debug:
        tbl.clone('debug')
        test.clone('debug')
    if Options.options.release:
        tbl.clone('release')
        test.clone('release')

