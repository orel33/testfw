// Simple Test Framework (testfw)

#ifndef TESTFW_H
#define TESTFW_H

#include <stdbool.h>

#define PREFIX "test"
#define TIMEOUT 2 // default timeout (in sec.), 0 if no timeout

/* ********** TEST FRAMEWORK API ********** */

enum runmode_t
{
    FORK,
    THREAD, // not yet implemented
    NOFORK
};
typedef int (*test_func)(int argc, char *argv[]);
struct test
{
    char *prefix;
    char *name;
    test_func func;
};

struct testfw; /* forward decalaration */

struct testfw *testfw_init(char *program, int timeout, char *logfile, bool silent);
void testfw_free(struct testfw *fw);
int testfw_run_one(struct testfw *fw, char *prefix, char *testname, int testargc, char *testargv[], enum runmode_t mode);
int testfw_run_all(struct testfw *fw, char *prefix, int testargc, char *testargv[], enum runmode_t mode); /* prefix may be NULL */
struct test *testfw_register_func(struct testfw *fw, char *prefix, char *testname, test_func func);
struct test *testfw_register_symb(struct testfw *fw, char *prefix, char *testname);
void testfw_register_prefix(struct testfw *fw, char *prefix);
void testfw_iterate_all(struct testfw *fw, char *prefix, void (*callback)(struct test *, void *), void *data); /* prefix may be NULL */

#endif