set(libDir ${CMAKE_CURRENT_BINARY_DIR}/../tbl/lib)
set(incDir ${CMAKE_CURRENT_BINARY_DIR}/../tbl/include)

set(LIBRARY_OUTPUT_PATH ${libDir})

add_library(tbl tbl.c)
add_library(tbl_s STATIC tbl.c)

file(COPY tbl.h DESTINATION ${incDir})
