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
    struct testfw *fw = testfw_init(argv[0], TIMEOUT, LOGFILE, SILENT);
    testfw_register_func(fw, "test", "first", test_first);
    testfw_register_symb(fw, "test", "second");
    testfw_register_prefix(fw, "anothertest");
    testfw_run_all(fw, NULL, argc - 1, argv + 1, FORK);
    testfw_free(fw);
    return EXIT_SUCCESS;
}