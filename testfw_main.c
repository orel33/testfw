// Simple Test Framework (testfw)
#define _GNU_SOURCE
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "testfw.h"

enum action_t
{
    RUNALL,
    RUNSUITE,
    RUNONE,
    LIST
};

/* ********** USAGE ********** */

void usage(int argc, char *argv[])
{
    printf("Usage: %s [options] [actions] [-- <testargs> ...]\n", argv[0]);
    printf("Actions:\n");
    printf("  -a: run all tests one by one [default]\n");
    printf("  -r <testname>: run a single test\n");
    printf("  -R <prefix>: run a test suite\n");
    printf("  -l: list all available tests (depending on current prefix)\n");
    printf("Options:\n");
    printf("  -p <testprefix>: set test prefix (default is \"%s\")\n", TESTFW_PREFIX);
    printf("  -o <logfile>: redirect test stdout & stderr to a log file\n");
    printf("  -O: redirect test stdout & stderr to /dev/null\n");
    printf("  -t <timeout>: set time limits for each test (in sec.)\n");
    printf("  -T: no timeout [default]\n");
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
    printf("%s\n", test->name);
}

/* ********** MAIN ********** */

int main(int argc, char *argv[])
{
    int opt;
    char *logfile = NULL;
    int timeout = 0; // no timeout (default)
    char *testname = NULL;
    bool count = false;
    bool silent = false; // silent mode
    char *prefix = TESTFW_PREFIX;
    enum testfw_mode_t mode = TESTFW_FORK; // default mode
    enum action_t action = RUNALL; // default action

    while ((opt = getopt(argc, argv, "t:Tnalp:sScto:Or:R:h?")) != -1)
    {
        switch (opt)
        {
            // actions
        case 'a':
            action = RUNALL;
            break;
        case 'l':
            action = LIST;
            break;
        case 'r':
            action = RUNONE;
            testname = optarg;
            break;
        case 'R':
            action = RUNSUITE;
            testname = optarg;
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
        case 'p':
            prefix = optarg;
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

    int nfailures = 0;
    if (action == LIST)
    {
        testfw_register_prefix(fw, prefix);
        testfw_iterate_all(fw, prefix, callback_list, NULL);
    }
    else if (action == RUNONE)
    {
        testfw_register_symb(fw, prefix, testname);
        nfailures = testfw_run_one(fw, prefix, testname, testargc, testargv, mode);
    }
    else if (action == RUNSUITE)
    {
        testfw_register_prefix(fw, prefix);
        nfailures =testfw_run_all(fw, testname, testargc, testargv, mode);
    }
    else if (action == RUNALL)
    {
        testfw_register_prefix(fw, prefix);
        nfailures =testfw_run_all(fw, NULL, testargc, testargv, mode);
    }
    else
        usage(argc, argv);

    /* free tests */
    testfw_free(fw);

    if (count || mode == TESTFW_NOFORK)
        return nfailures;

    return EXIT_SUCCESS;
}
