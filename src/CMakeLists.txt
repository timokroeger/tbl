set(libDir ${CMAKE_CURRENT_BINARY_DIR}/../tbl/lib)
set(incDir ${CMAKE_CURRENT_BINARY_DIR}/../tbl/include)

set(LIBRARY_OUTPUT_PATH ${libDir})

add_library(bencode SHARED tbl.c)
add_library(bencode_s STATIC tbl.c)

file(COPY tbl.h DESTINATION ${incDir})
