# TODO

* in nofork mode, catch SIGALRM if timeout and return exit status 124
* in testfw_main.c, need to support multiple registrations with -r/-R options
* run test or test suite in parallel
* implement thread support
* add a CMake function to automatically add all TESTFW tests as CTest tests
* print final status à la CTest : "86% tests passed, 1 tests failed out of 7"
