# A Simple Test Framework

A simple test framework for language C, inspired by [Check](https://libcheck.github.io/check/) & [Google Test](https://github.com/google/googletest).

The main features are:

* test framework for C/C++ projects
* a simple framework written in C, with few files to include in your project
* framework developed in C on Linux (based on POSIX standard)
* easy way to integrate your test with the TEST() macro, following the same philosophy as GoogleTest
* support test with *argv* arguments
* test executed in a child process (fork mode)
* start the tests one by one or all at once

## Compilation

Our test framework is made up of two libraries:

* *libtestfw.a*: all the basic routines to discover, register and run tests (see API in [testfw.h](testfw.h)).
* *libtestfw_main.a*: a *main()* routine to launch tests easily (optionnal).

```bash
gcc -std=c99 -Wall -g   -c -o testfw.o testfw.c
ar rcs libtestfw.a testfw.o
gcc -std=c99 -Wall -g   -c -o testfw_main.o testfw_main.c
ar rcs libtestfw_main.a testfw_main.o
```

## Adding a First Test

Adding a test is really simple, you just need to edit a new file *tests.c* and to include some test functions (with a "test_" prefix) as follows:

```c
#include <stdio.h>
#include <stdlib.h>
#include "testfw.h"

int test_hello(int argc, char* argv[])
{
    printf("hello world\n");
    return EXIT_SUCCESS;
}
```

And that's all!

A success test should return EXIT_SUCCESS. All other cases are considered as a failure.

More precisely, running a test returns one of the following status:

* SUCCESS: return EXIT_SUCCESS or 0 (normal exit)
* FAILURE: return EXIT_FAILURE (or any value different of EXIT_SUCCESS)
* KILLED: killed by any signal (SIGSEGV, SIGABRT, ...) except SIGALRM
* TIMEOUT: killed by SIGALRM after timeout

## Running Tests

Let's consider the code [sample.c](sample.c). To run all this tests, you need first to compile it and then to link it against our both libraries.

```bash
gcc -std=c99 -Wall -g -c sample.c
gcc sample.o -o sample -rdynamic -ltestfw_main -ltestfw -ldl -L.
```

The '-rdynamic' option is required to load all symbols in the dynamic symbol table (ELF linker).


Usage:

```text
$ ./sample -h
Usage: ./sample [options] [actions] [-- <testargs> ...]
Actions:
  -a: run all tests one by one [default]
  -r <testname>: run a single test
  -l: list all available tests (depending on current prefix)
Options:
  -p <testprefix>: set test prefix (default is "test")
  -o <logfile>: redirect test stdout & stderr to a log file
  -O: redirect test stdout & stderr to /dev/null
  -t <timeout>: set time limits for each test (default is 2s)
  -c: return the total number of test failures
  -s: silent mode
  -n: disable fork mode (no fork)
  -S: full silent mode (not only tests)
  -h: print this help message
```

Run all tests one by one:

```bash
$ ./sample -O
[KILLED] run test "test.assert" in 0.48 ms (signal "Aborted", wstatus 6)
[FAILURE] run test "test.failure" in 0.36 ms (status 1, wstatus 256)
[KILLED] run test "test.segfault" in 0.37 ms (signal "Segmentation fault", wstatus 11)
[TIMEOUT] run test "test.sleep" in 2000.30 ms (signal "Alarm clock", wstatus 14)
[SUCCESS] run test "test.success" in 0.40 ms (status 0, wstatus 0)
```

Run a single test (fork mode):

```bash
$ ./sample -r failure
[FAILURE] run failure in 0.63 ms (status 1)
```

Run a single test (nofork mode):

```bash
$ ./sample -n -r failure
[FAILURE] run failure in 0.63 ms (status 1)
$ echo $?
1
```

This mode launches a single test function *directly* (without fork) and it is especially useful to integrate it into another test framework as CTest. See [CMakeLists.txt](CMakeLists.txt).

Adding tests for CMake:

```cmake
set(cases "success" "failure" "segfault" "assert" "sleep")
foreach(case ${cases})
add_test(sample_${case} sample -n -r ${case})
set_tests_properties (sample_${case} PROPERTIES TIMEOUT 2)
endforeach()
```

And running tests.

```bash
$ make && make test
Running tests...
Test project /home/orel/Documents/Teaching/ProjetTechno/GIT/testfw/build
    Start 1: sample_success
1/5 Test #1: sample_success ...................   Passed    0.00 sec
    Start 2: sample_failure
2/5 Test #2: sample_failure ...................***Failed    0.00 sec
    Start 3: sample_segfault
3/5 Test #3: sample_segfault ..................***Exception: SegFault  0.00 sec
    Start 4: sample_assert
4/5 Test #4: sample_assert ....................***Exception: Child aborted  0.00 sec
    Start 5: sample_sleep
5/5 Test #5: sample_sleep .....................***Timeout   2.01 sec
```

## Main Routine

A *main()* routine is already provided for convenience in the *libtestfw_main.a* library, but it could be useful in certain case to write your own *main()* routine based on the [testfw.h](testfw.h) API.

```c
#include <stdlib.h>
#include <stdbool.h>
#include "testfw.h"

#define TIMEOUT 2
#define LOGFILE "test.log"
#define SILENT true

int test_first(int argc, char* argv[]) { /* ... */ }
int test_second(int argc, char* argv[]) { /* ... */ }
int anothertest_first(int argc, char* argv[]) { /* ... */ }
int anothertest_second(int argc, char* argv[]) { /* ... */ }

int main(int argc, char* argv[])
{
  struct testfw *fw = init_tests(argv[0], TIMEOUT, LOGFILE, SILENT);
  register_test_func(fw, "test", "first", test_first);
  register_test_symb(fw, "test", "second");
  register_all_tests(fw, "anothertest");
  run_all_tests(fw, argc-1, argv+1, FORK);
  free_tests(fw);
  return EXIT_SUCCESS;
}
```
Compiling and running this test [mymain.c](mymain.c) will produce the following results.

```bash
$ ./mymain 
[SUCCESS] run test "test.first" in 0.51 ms (status 0, wstatus 0)
[SUCCESS] run test "test.second" in 0.51 ms (status 0, wstatus 0)
[SUCCESS] run test "anothertest.first" in 0.56 ms (status 0, wstatus 0)
[SUCCESS] run test "anothertest.second" in 0.64 ms (status 0, wstatus 0)
```



---

aurelien.esnard@u-bordeaux.fr