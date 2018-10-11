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
    THREAD,
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

struct testfw *init_tests(char *program, int timeout, char *logfile, bool silent);
void free_tests(struct testfw *fw);
int run_one_test(struct testfw *fw, char* prefix, char *testname, int testargc, char *testargv[], enum runmode_t mode);
int run_all_tests(struct testfw *fw, int testargc, char *testargv[], enum runmode_t mode);
struct test *register_test_func(struct testfw *fw, char *prefix, char *testname, test_func func);
struct test *register_test_symb(struct testfw *fw, char *prefix, char *testname);
void register_all_tests(struct testfw *fw, char *prefix);
void iterate_all_tests(struct testfw *fw, void (*callback)(struct test *, void *), void *data);

#endif