include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../tbl/include)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../tbl/lib)

set(binDir ${CMAKE_CURRENT_BINARY_DIR}/../../tbl/bin)
file(MAKE_DIRECTORY ${binDir})

add_executable(bencode_to_json bencode_to_json.c)
target_link_libraries(bencode_to_json tbl yajl)

get_target_property(binPath bencode_to_json LOCATION)
add_custom_command(TARGET bencode_to_json POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${binPath} ${binDir})
