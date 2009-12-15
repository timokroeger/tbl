include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../tbl/include)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../tbl/lib)

set(binDir ${CMAKE_CURRENT_BINARY_DIR}/../../tbl/bin)
file(MAKE_DIRECTORY ${binDir})

add_executable(bencode_tojson bencode_tojson.c)
target_link_libraries(bencode_tojson tbl yajl)

get_target_property(binPath bencode_tojson LOCATION)
add_custom_command(TARGET bencode_tojson POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${binPath} ${binDir})
