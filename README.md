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

Adding a test is really simple, you just need to edit a new file *tests.c* and to include some test functions (with a "test_" suite) as follows:

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

Then, launching the main routine provide you some helpful commands to run your tests. Usage:

```text
$ ./sample -h
Usage: ./sample [options] [actions] [-- <testargs> ...]
Actions:
  -x: execute all registered tests
  -l: list all registered tests
Options:
  -r <suite.testname>: register a function "suite_testname()" as a test
  -R <suite>: register all functions "suite_*()" as a test suite
  -o <logfile>: redirect test stdout & stderr to a log file
  -O: redirect test stdout & stderr to /dev/null
  -t <timeout>: set time limits for each test (in sec.) [default 1]
  -T: no timeout
  -c: return the total number of test failures
  -s: silent mode
  -n: disable fork mode (no fork)
  -S: full silent mode (not only tests)
  -h: print this help message
```

Run the test suite "test" with some options (timeout = 2 seconds, log file = test.log):

```bash
$ ./sample -R sample -t 2 -O -x
[KILLED] run test "sample.assert" in 0.57 ms (signal "Aborted", wstatus 6)
[FAILURE] run test "sample.failure" in 0.48 ms (status 1, wstatus 256)
[KILLED] run test "sample.segfault" in 0.45 ms (signal "Segmentation fault", wstatus 11)
[TIMEOUT] run test "sample.sleep" in 1000.63 ms (signal "Alarm clock", wstatus 14)
[SUCCESS] run test "sample.success" in 0.59 ms (status 0, wstatus 0)
```

Run a single test in *fork* mode (default):

```bash
$ ./sample -r sample.failure -x
[FAILURE] run test "sample.failure" in 0.53 ms (status 1, wstatus 256)
$ echo $?
0
```

In the *fork* mode, each test is runned separately in child process... The failure of a test will not affect the execution of following tests.

Run a single test (in *nofork* mode):

```bash
$ ./sample -r sample.failure -n -x
[FAILURE] run test "sample.failure" in 0.53 ms (status 1, wstatus 256)
$ echo $?
1
```

In the *nofork* mode, each test is runned *directly* as function call (without fork). As a consequence, the first test that fails will interrupt all the following.  It is especially useful when running all tests one by one within another test framework as CTest. See [CMakeLists.txt](CMakeLists.txt).

And running tests.

```bash
$ make && make test
Running tests...
Test project /home/orel/Documents/Teaching/ProjetTechno/GIT/testfw/build
    Start 1: sample.success
1/5 Test #1: sample.success ...................   Passed    0.00 sec
    Start 2: sample.failure
2/5 Test #2: sample.failure ...................***Failed    0.00 sec
    Start 3: sample.segfault
3/5 Test #3: sample.segfault ..................***Exception: SegFault  0.00 sec
    Start 4: sample.assert
4/5 Test #4: sample.assert ....................***Exception: Child aborted  0.00 sec
    Start 5: sample.sleep
5/5 Test #5: sample.sleep .....................***Timeout   2.01 sec
```

## Main Routine

A *main()* routine is already provided for convenience in the *libtestfw_main.a* library, but it could be useful in certain case to write your own *main()* routine based on the [testfw.h](testfw.h) API. See [sample_main.c](sample_main.c).

```c
#include <stdlib.h>
#include <stdbool.h>
#include "testfw.h"
#include "sample.h"

#define TIMEOUT 2
#define LOGFILE "test.log"
#define SILENT false

int main(int argc, char *argv[])
{
    struct testfw_t *fw = testfw_init(argv[0], TIMEOUT, LOGFILE, SILENT);
    testfw_register_func(fw, "sample", "success", sample_success);
    testfw_register_symb(fw, "sample", "failure");
    testfw_register_suite(fw, "othersample");
    testfw_run_all(fw, argc - 1, argv + 1, TESTFW_FORK);
    testfw_free(fw);
    return EXIT_SUCCESS;
}
```

Compiling and running this test will produce the following results.

```bash
$ gcc -std=c99 -rdynamic -Wall sample.c sample_main.c -o sample_main -ltestfw -ldl -L.
$ ./sample_main
[SUCCESS] run test "sample.success" in 0.41 ms (status 0, wstatus 0)
[FAILURE] run test "sample.failure" in 0.52 ms (status 1, wstatus 256)
[FAILURE] run test "othersample.negret" in 0.52 ms (status 255, wstatus 65280)
[FAILURE] run test "othersample.posret" in 0.52 ms (status 2, wstatus 512)
```

---

aurelien.esnard@u-bordeaux.fr