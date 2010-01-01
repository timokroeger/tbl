tbl - tiny bencode library
==========================
*tbl* is a tiny event driven [bencode] library written in c. Its api is
similar to the one of [yajl].

It's supposed to be

* fast
* small
* easy to use

Compilation
-----------
To compile the bencode_to_json example you need [yajl].

	mkdir build
	cd build
	cmake ..
	make

Licence
-------
*tbl* is released under WTFPL so you can do what the fuck you want.

[bencode]: (http://en.wikipedia.org/wiki/Bencode)
[yajl]: (http://github.com/lloyd/yajl)
