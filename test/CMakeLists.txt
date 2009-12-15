include_directories(${CMAKE_CURRENT_BINARY_DIR}/../tbl/include)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../tbl/lib)

add_executable(test1 test.c)
target_link_libraries(test1 tbl_s)

enable_testing()

add_test(test1 test1)
