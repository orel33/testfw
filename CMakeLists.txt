PROJECT("TESTFW" C)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_VERBOSE_MAKEFILE ON)

include(CTest)
enable_testing()

set(CMAKE_C_FLAGS "-Wall -fPIC -std=c99")
set(CMAKE_LD_FLAGS "-rdynamic")

add_library(testfw testfw.c testfw.h)
target_link_libraries(testfw dl)

add_library(testfw_main testfw_main.c testfw.h)
target_link_libraries(testfw_main testfw)

add_executable(hello hello.c)
target_link_libraries(hello testfw_main testfw)

add_executable(sample sample.c sample.h)
target_link_libraries(sample testfw_main testfw)

add_executable(sample_main sample_main.c sample.c sample.h)
target_link_libraries(sample_main testfw)

# launch test directly using CTest
set(tests "test.success" "test.failure" "test.segfault" "test.assert" "test.sleep" "test.alarm" "test.args" "test.infiniteloop")
foreach(test ${tests})
add_test(${test} sample -t 2 -r ${test} -x)
set_tests_properties (${test} PROPERTIES TIMEOUT 4)
endforeach()

# launch tests within TESTFW
add_test(sample_all sample -t 2 -x)
add_test(sample_main sample_main)

# launch test hello
file(COPY hello.expected DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
add_test(hello hello -x)
add_test(hello_grep hello -x -g hello)
add_test(hello_diff hello -x -d hello.expected WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

