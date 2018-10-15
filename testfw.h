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
    TESTFW_THREAD,
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

/**
 * @brief initialize test framework
 * 
 * @param program 
 * @param timeout 
 * @param logfile 
 * @param silent 
 * @return struct testfw_t* 
 */
struct testfw_t *testfw_init(char *program, int timeout, char *logfile, bool silent);

/**
 * @brief finalize test framework
 * 
 * @param fw 
 */
void testfw_free(struct testfw_t *fw);

/**
 * @brief get number of registered tests
 * 
 * @param fw 
 * @return int 
 */
int testfw_length(struct testfw_t *fw);

/**
 * @brief get a registered test
 * 
 * @param fw 
 * @param k 
 * @return struct test_t* 
 */
struct test_t *testfw_get(struct testfw_t *fw, int k);

/**
 * @brief register a single test function
 * 
 * @param fw 
 * @param suite 
 * @param name 
 * @param func 
 * @return struct test_t* 
 */
struct test_t *testfw_register_func(struct testfw_t *fw, char *suite, char *name, testfw_func_t func);

/**
 * @brief register a single test function named "<suite>_<name>""
 * 
 * @param fw 
 * @param suite 
 * @param name 
 * @return struct test_t* 
 */
struct test_t *testfw_register_symb(struct testfw_t *fw, char *suite, char *name);

/**
 * @brief register all test functions named "<suite>_*"
 * 
 * @param fw 
 * @param suite 
 * @return int 
 */
int testfw_register_suite(struct testfw_t *fw, char *suite);

/**
 * @brief run all registered tests
 * 
 * @param fw 
 * @param testargc 
 * @param testargv 
 * @param testfw_mode_t 
 * @return int 
 */
int testfw_run_all(struct testfw_t *fw, int testargc, char *testargv[], enum testfw_mode_t mode);

#endif
