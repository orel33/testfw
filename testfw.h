// Simple Test Framework (testfw)

#ifndef TESTFW_H
#define TESTFW_H

#include <stdbool.h>

/* ********** TEST FRAMEWORK API ********** */

#define TESTFW_VERSION_MAJOR 0
#define TESTFW_VERSION_MINOR 1
# define TESTFW_EXIT_SUCCESS EXIT_SUCCESS
# define TESTFW_EXIT_FAILURE EXIT_FAILURE
# define TESTFW_EXIT_TIMEOUT 124

enum testfw_mode_t
{
    TESTFW_FORK,
    TESTFW_THREAD, // TODO: not yet implemented
    TESTFW_NOFORK
};
typedef int (*testfw_func_t)(int argc, char *argv[]);
struct test_t
{
    char *suite;
    char *name;
    testfw_func_t func;
};

struct testfw_t; /* forward decalaration */

struct testfw_t *testfw_init(char *program, int timeout, char *logfile, bool silent);
void testfw_free(struct testfw_t *fw);
int testfw_length(struct testfw_t *fw);
struct test_t *testfw_get(struct testfw_t *fw, int k);
struct test_t *testfw_register_func(struct testfw_t *fw, char *suite, char *name, testfw_func_t func);
struct test_t *testfw_register_symb(struct testfw_t *fw, char *suite, char *name);
int testfw_register_suite(struct testfw_t *fw, char *suite);
int testfw_run_all(struct testfw_t *fw, int testargc, char *testargv[], enum testfw_mode_t mode);


#endif