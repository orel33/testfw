// Simple Test Framework (testfw)

#ifndef TESTFW_H
#define TESTFW_H

#include <stdbool.h>

#define TESTFW_PREFIX "test"

/* ********** TEST FRAMEWORK API ********** */

enum testfw_mode_t
{
    TESTFW_FORK,
    TESTFW_THREAD, // TODO: not yet implemented
    TESTFW_NOFORK
};
typedef int (*test_func)(int argc, char *argv[]);
struct testfw_test_t
{
    char *prefix;
    char *name;
    test_func func;
};

struct testfw_t; /* forward decalaration */

struct testfw_t *testfw_init(char *program, int timeout, char *logfile, bool silent);
void testfw_free(struct testfw_t *fw);
int testfw_run_one(struct testfw_t *fw, char *prefix, char *testname, int testargc, char *testargv[], enum testfw_mode_t mode);
int testfw_run_all(struct testfw_t *fw, char *prefix, int testargc, char *testargv[], enum testfw_mode_t mode); /* prefix may be NULL */
struct testfw_test_t *testfw_register_func(struct testfw_t *fw, char *prefix, char *testname, test_func func);
struct testfw_test_t *testfw_register_symb(struct testfw_t *fw, char *prefix, char *testname);
void testfw_register_prefix(struct testfw_t *fw, char *prefix);
void testfw_iterate_all(struct testfw_t *fw, char *prefix, void (*callback)(struct testfw_test_t *, void *), void *data); /* prefix may be NULL */

#endif