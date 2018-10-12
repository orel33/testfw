#include <stdlib.h>
#include <stdbool.h>
#include "testfw.h"

#define TIMEOUT 2
#define LOGFILE "test.log"
#define SILENT false

int test_first(int argc, char *argv[])
{
    /* ... */
    return EXIT_SUCCESS;
}

int test_second(int argc, char *argv[])
{
    /* ... */
    return EXIT_SUCCESS;
}

int anothertest_first(int argc, char *argv[])
{
    /* ... */
    return EXIT_SUCCESS;
}

int anothertest_second(int argc, char *argv[])
{
    /* ... */
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    struct testfw *fw = init_tests(argv[0], TIMEOUT, LOGFILE, SILENT);
    register_test_func(fw, "test", "first", test_first);
    register_test_symb(fw, "test", "second");
    register_all_tests(fw, "anothertest");
    run_all_tests(fw, argc-1, argv+1, FORK);
    free_tests(fw);
    return EXIT_SUCCESS;
}