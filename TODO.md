# TODO

* in nofork mode, catch SIGALRM if timeout and return exit status 124
* in testfw_main.c, need to support multiple registrations with -r/-R options
* run a test suite in parallel (using SIGCHILD)
* implement thread mode
* add a CMake function to automatically add all TESTFW tests as CTest tests
* use property WILL_FAIL for add_test() in CMakeLists.txt
