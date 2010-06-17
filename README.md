tbl - tiny bencode library
==========================
*tbl* is a tiny event driven [bencode](http://en.wikipedia.org/wiki/Bencode)
library written in C. API design is taken from
[yajl](http://github.com/lloyd/yajl).

features
--------
* fast
* small
* easy to use

build instructions
------------------

	./waf configure
	./waf

### build variants

default variant is called default

	./waf --debug
	./waf --release

### run tests

	./build/<variant>/test

licence
-------
*tbl* is released under WTFPL so you can do what the fuck you want.

