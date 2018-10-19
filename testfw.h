// Simple Test Framework (testfw)

#ifndef TESTFW_H
#define TESTFW_H

#include <stdbool.h>

/* ********** TEST FRAMEWORK API ********** */

#define TESTFW_VERSION_MAJOR 0
#define TESTFW_VERSION_MINOR 2
#define TESTFW_EXIT_SUCCESS EXIT_SUCCESS
#define TESTFW_EXIT_FAILURE EXIT_FAILURE
#define TESTFW_EXIT_TIMEOUT 124

/**
 * @brief 
 * 
 */
enum testfw_mode_t
{
    TESTFW_FORKS, /**> sequential test execution with process fork */
    TESTFW_FORKP, /**> parallel test execution with process fork */
    TESTFW_NOFORK /**>  sequential test execution without process fork */
};

/**
 * @brief 
 * 
 */
typedef int (*testfw_func_t)(int argc, char *argv[]);
/**
 * @brief 
 * 
 */
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
 * @param cmd
 * @param silent 
 * @param verbose
 * @return struct testfw_t* 
 */
struct testfw_t *testfw_init(char *program, int timeout, char *logfile, char *cmd, bool silent, bool verbose);

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
 * @param argc 
 * @param argv 
 * @param cmd 
 * @param testfw_mode_t 
 * @return int 
 */

int testfw_run_all(struct testfw_t *fw, int argc, char *argv[], enum testfw_mode_t mode);

#endif
