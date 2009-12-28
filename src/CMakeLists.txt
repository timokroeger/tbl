set(libDir ${CMAKE_CURRENT_BINARY_DIR}/../tbl/lib)
set(incDir ${CMAKE_CURRENT_BINARY_DIR}/../tbl/include)

set(LIBRARY_OUTPUT_PATH ${libDir})

add_library(tbl SHARED tbl.c tbl.h)
add_library(tbl_s STATIC tbl.c tbl.h)

file(COPY tbl.h DESTINATION ${incDir})
