PROJECT("TESTFW" C)
cmake_minimum_required(VERSION 3.2)

include(CTest)
enable_testing()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -rdynamic")

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
set(tests "sample.success" "sample.failure" "sample.segfault" "sample.assert" "sample.sleep")
foreach(test ${tests})
add_test(${test} sample -n -r ${test})
set_tests_properties (${test} PROPERTIES TIMEOUT 2)
endforeach()

# launch tests within TESTFW
add_test(sample_all sample -R sample -x)
add_test(sample_main sample_main)


