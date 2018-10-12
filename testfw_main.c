// Simple Test Framework (testfw)
#define _GNU_SOURCE
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>

#include "testfw.h"

enum action_t
{
    EXECUTE,
    LIST
};

/* ********** USAGE ********** */

void usage(int argc, char *argv[])
{
    printf("Usage: %s [options] [actions] [-- <testargs> ...]\n", argv[0]);
    printf("Actions:\n");
    printf("  -x: execute all registered tests\n");
    printf("  -l: list all registered tests\n");
    printf("Options:\n");
    printf("  -r <suite.testname>: register a function \"suite_testname()\" as a test\n");
    printf("  -R <suite>: register all functions \"suite_*()\" as a test suite\n");
    printf("  -o <logfile>: redirect test stdout & stderr to a log file\n");
    printf("  -O: redirect test stdout & stderr to /dev/null\n");
    printf("  -t <timeout>: set time limits for each test (in sec.) [default %d]\n", TESTFW_DEFAULT_TIMEOUT);
    printf("  -T: no timeout\n");
    printf("  -c: return the total number of test failures\n");
    printf("  -s: silent mode\n");
    printf("  -n: disable fork mode (no fork)\n");
    printf("  -S: full silent mode (not only tests)\n");
    printf("  -h: print this help message\n");
    exit(EXIT_FAILURE);
}

/* ********** LIST ********** */

void callback_list(struct test_t *test, void *data)
{
    printf("%s.%s\n", test->suite, test->name);
}

/* ********** MAIN ********** */

int main(int argc, char *argv[])
{
    int opt;

    char *logfile = NULL;                  // test log (no log by default)
    int timeout = TESTFW_DEFAULT_TIMEOUT;  // timeout (in sec.)
    bool count = false;                    // return nb failures
    bool silent = false;                   // silent mode
    enum testfw_mode_t mode = TESTFW_FORK; // default mode
    enum action_t action = EXECUTE;        // default action
    char *suite = TESTFW_DEFAULT_SUITE;    // default suite
    char *testname = NULL;

    while ((opt = getopt(argc, argv, "r:R:t:TnsSco:Olxh?")) != -1)
    {
        switch (opt)
        {
        // register tests
        case 'r':
        {
            char *sep = strchr(optarg, '.');
            if(!sep) {
                fprintf(stderr, "Error: invalid test name %s\n", optarg);
                exit(EXIT_FAILURE);
            }
            *sep = 0;
            suite = optarg;
            testname = sep + 1;
            break;
        }
        case 'R':
            suite = optarg;
            testname = NULL;
            break;
        // actions
        case 'x':
            action = EXECUTE;
            break;
        case 'l':
            action = LIST;
            break;
        // options
        case 's':
            silent = true;
            break;
        case 'c':
            count = true;
            break;
        case 'o':
            logfile = optarg;
            break;
        case 'O':
            logfile = "/dev/null";
            break;
        case 'S':
            silent = true;
            logfile = "/dev/null";
            break;
        case 't':
            timeout = atoi(optarg);
            break;
        case 'T':
            timeout = 0; // no timeout
            break;
        case 'n':
            mode = TESTFW_NOFORK;
            break;
        case '?':
        case 'h':
        default:
            usage(argc, argv);
            break;
        }
    }

    int testargc = argc - optind;
    char **testargv = argv + optind;
    struct testfw_t *fw = testfw_init(argv[0], timeout, logfile, silent);

    // register tests
    if (suite && testname)
        testfw_register_symb(fw, suite, testname);
    else if (suite)
        testfw_register_suite(fw, suite);

    /* actions */
    int nfailures = 0;
    if (action == LIST)
    {
        testfw_iterate_all(fw, callback_list, NULL);
    }
    else if (action == EXECUTE)
    {
        // nfailures = testfw_run_one(fw, suite, testname, testargc, testargv, mode);
        nfailures = testfw_run_all(fw, testargc, testargv, mode);
    }
    else
        usage(argc, argv);

    /* free tests */
    testfw_free(fw);

    if (count || mode == TESTFW_NOFORK)
        return nfailures;

    return EXIT_SUCCESS;
}
