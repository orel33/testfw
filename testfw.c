#define _GNU_SOURCE
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <assert.h>
#include <dlfcn.h>
#include <setjmp.h>
#include <signal.h>

#include "testfw.h"

#define GREEN "\033[0;32m" // Green Color
#define RED "\033[0;31m"   // Red Color
#define NC "\033[0m"       // No Color

/* ********** STRUCTURES ********** */

struct testfw_t
{
    char *program;
    int timeout;
    char *logfile;
    char *cmd;
    bool silent;
    bool verbose;
    int size;
    int capacity;
    struct test_t *tests;
};

/* ********** FRAMEWORK ROUTINES ********** */

struct testfw_t *testfw_init(char *program, int timeout, char *logfile, char *cmd, bool silent, bool verbose)
{
    assert(program);
    struct testfw_t *fw = malloc(sizeof(struct testfw_t));
    assert(fw);
    fw->program = strdup(program);
    fw->timeout = timeout;
    fw->logfile = logfile ? strdup(logfile) : NULL;
    fw->cmd = cmd ? strdup(cmd) : NULL;
    fw->silent = silent;
    fw->verbose = verbose;
    fw->size = 0;
    fw->capacity = 10;
    fw->tests = malloc(fw->capacity * sizeof(struct test_t));
    assert(fw->tests);
    return fw;
}

void testfw_free(struct testfw_t *fw)
{
    assert(fw);
    free(fw->program);
    free(fw->logfile);
    free(fw->cmd);
    for (int i = 0; i < fw->size; i++)
        free(fw->tests[i].name);
    free(fw->tests);
    free(fw);
}

int testfw_length(struct testfw_t *fw)
{
    assert(fw);
    return fw->size;
}

struct test_t *testfw_get(struct testfw_t *fw, int k)
{
    assert(fw);
    assert(k >= 0 && k < fw->size);
    return fw->tests + k;
}
static struct test_t *add_test(struct testfw_t *fw, char *suite, char *name, testfw_func_t func)
{
    assert(fw && fw->size <= fw->capacity);
    assert(suite && name && func);

    if (fw->size == fw->capacity)
    {
        fw->capacity *= 2;
        fw->tests = realloc(fw->tests, fw->capacity * sizeof(struct test_t));
        assert(fw->tests);
    }
    struct test_t *t = &(fw->tests[fw->size]);
    t->suite = strdup(suite);
    t->name = strdup(name);
    t->func = func;
    fw->size++;
    return t;
}

/* ********** REGISTER TEST ********** */

struct test_t *testfw_register_func(struct testfw_t *fw, char *suite, char *name, testfw_func_t func)
{
    assert(fw);
    assert(name);
    assert(func);
    struct test_t *t = add_test(fw, suite, name, func);
    return t;
}

static testfw_func_t lookup_symb(void *handle, char *funcname)
{
    assert(handle);
    testfw_func_t func = (testfw_func_t)dlsym(handle, funcname);
    if (!func)
    {
        fprintf(stderr, "Error: symbol \"%s\" not found!\n", funcname);
        exit(EXIT_FAILURE);
    }
    return func;
}

static char *test2func(char *suite, char *name)
{
    char *funcname = malloc((strlen(name) + 1 + strlen(suite) + 1) * sizeof(char));
    assert(funcname);
    *funcname = 0;
    strcat(funcname, suite);
    strcat(funcname, "_");
    strcat(funcname, name);
    return funcname;
}

struct test_t *testfw_register_symb(struct testfw_t *fw, char *suite, char *name)
{
    assert(fw);
    assert(suite && name);
    void *handle = dlopen(NULL, RTLD_LAZY); /* if NULL, then the returned handle is for the main program */
    assert(handle);
    char *funcname = test2func(suite, name);
    testfw_func_t func = lookup_symb(handle, funcname);
    struct test_t *t = add_test(fw, suite, name, func);
    free(funcname);
    dlclose(handle);
    return t;
}

static char **discover_all_tests(struct testfw_t *fw, char *suite)
{
    assert(fw);
    void *handle = dlopen(NULL, RTLD_LAZY); /* if NULL, then the returned handle is for the main program */
    assert(handle);
    int nbtests = 0;
    int maxtests = 10;
    char **names = malloc((maxtests + 1) * sizeof(char *));
    assert(names);
    names[0] = NULL;

    char *cmdline = NULL;

    char *prefix_ = malloc(strlen(suite) + 2); /* adding a trailing '_' to suite */
    assert(prefix_);
#if defined(__APPLE__) && defined(__MACH__)
    strcpy(prefix_, "_");
    strcat(prefix_, suite);
    strcat(prefix_, "_");
#else
    strcpy(prefix_, suite);
    strcat(prefix_, "_");
#endif

    /* TODO: inspect symbol table instead of using nm external command */
    asprintf(&cmdline, "nm --defined-only %s | cut -d ' ' -f 3 | grep ^%s ", fw->program, prefix_);
    // printf("cmdline: %s\n", cmdline);
    assert(cmdline);
    FILE *stream = popen(cmdline, "r");
    char *funcname = NULL;
    size_t size = 0;
    while (getline(&funcname, &size, stream) > 0)
    {
        if (nbtests == maxtests)
        {
            maxtests *= 2;
            names = realloc(names, (maxtests + 1) * sizeof(char *));
            assert(names);
        }
        funcname[strlen(funcname) - 1] = 0; /* remove trailing \n */
        char *name = strstr(funcname, prefix_);
        assert(name);
        name += strlen(prefix_);
        // printf("discover test: %s\n", name);
        names[nbtests] = strdup(name);
        names[nbtests + 1] = NULL;
        nbtests++;
    }
    free(funcname);
    free(cmdline);
    pclose(stream);
    dlclose(handle);
    return names;
}

int testfw_register_suite(struct testfw_t *fw, char *suite)
{
    assert(fw);
    assert(suite);
    char **names = discover_all_tests(fw, suite);
    void *handle = dlopen(NULL, RTLD_LAZY); /* if NULL, then the returned handle is for the main program */
    assert(handle);
    char **t = names;
    int k = 0;
    while (*t)
    {
        char *name = *t;
        char *funcname = test2func(suite, name);
        testfw_func_t func = lookup_symb(handle, funcname);
        add_test(fw, suite, name, func);
        free(name);
        free(funcname);
        t++;
        k++;
    }
    dlclose(handle);
    free(names);
    return k;
}

/* ********** DIAGNOSTIC ********** */

static void print_diag_test(FILE *stream, struct test_t *t, int wstatus, double mtime)
{
    assert(stream);
    assert(t);

    if (WIFEXITED(wstatus))
    {
        int status = WEXITSTATUS(wstatus);
        if (status == TESTFW_EXIT_SUCCESS)
            fprintf(stream, "%s[SUCCESS]%s run test \"%s.%s\" in %.2f ms (status %d)\n", GREEN, NC, t->suite, t->name, mtime, status);
        else if (status == TESTFW_EXIT_TIMEOUT)
            fprintf(stream, "%s[TIMEOUT]%s run test \"%s.%s\" in %.2f ms (status %d)\n", RED, NC, t->suite, t->name, mtime, status);
        else
            fprintf(stream, "%s[FAILURE]%s run test \"%s.%s\" in %.2f ms (status %d)\n", RED, NC, t->suite, t->name, mtime, status);
    }
    else if (WIFSIGNALED(wstatus))
    {
        int sig = WTERMSIG(wstatus);
        fprintf(stream, "%s[KILLED]%s run test \"%s.%s\" in %.2f ms (signal \"%s\")\n", RED, NC, t->suite, t->name, mtime, strsignal(sig));
    }
    else
        assert(0); // you should not be here?
}

/* ********** RUN TEST (FORK MODE) ********** */

sigjmp_buf alarm_env;

static void alarm_handler(int s)
{
    // printf("received signal %s!\n", strsignal(s));
    siglongjmp(alarm_env, 1);
}

static int run_test_forks(struct testfw_t *fw, struct test_t *t, int argc, char *argv[])
{
    assert(fw);
    assert(t);
    int wstatus = 0;

    int fd = -1;
    FILE *stream = NULL;

    /* open log file */
    if (fw->logfile)
    {
        fd = open(fw->logfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
    }
    /* pipe to an external command */
    else if (fw->cmd)
    {
        stream = popen(fw->cmd, "w"); // fork exec pipe
        assert(stream);
        fd = fileno(stream);
    }

    struct timeval tv_start, tv_end;
    gettimeofday(&tv_start, NULL);

    sigset_t sigset;
    sigemptyset(&sigset);
    if (fw->timeout > 0)
    {
        sigaddset(&sigset, SIGALRM);
        sigprocmask(SIG_BLOCK, &sigset, NULL);
    }
    /* run test */
    pid_t pid = fork();
    // setpgid(0, 0); // set the PGID of a process to its own PID

    if (pid == 0)
    {
        if (fw->logfile || fw->cmd)
        {
            dup2(fd, STDOUT_FILENO);
            dup2(fd, STDERR_FILENO);
            if (fw->logfile)
                close(fd);
        }
        fflush(stdout);
        fflush(stderr);

        if (fw->timeout > 0)
            sigprocmask(SIG_UNBLOCK, &sigset, NULL); // unblock SIGALRM

        /* execution */
        int status = t->func(argc, argv);
        exit(status);
    }

    /* timeout management */
    if (fw->timeout > 0)
    {
        struct sigaction act;
        act.sa_flags = 0;
        sigemptyset(&act.sa_mask);
        act.sa_handler = alarm_handler;
        sigaction(SIGALRM, &act, NULL);
        sigprocmask(SIG_UNBLOCK, &sigset, NULL);
        alarm(fw->timeout);
    }

    if (sigsetjmp(alarm_env, 1) == 0)
    {
        int r = waitpid(pid, &wstatus, 0); // may be interrupted by timeout
        assert(r == pid);
        alarm(0); // cancel pending alarm if any!
    }
    else
    {
        // printf("timeout: sigalarm interrupt waitpid()!\n");
        kill(pid, SIGKILL);
        int r = waitpid(pid, &wstatus, 0); // may be interrupted by timeout
        assert(r == pid);
        assert(WIFSIGNALED(wstatus) && WTERMSIG(wstatus) == SIGKILL);
        int status = TESTFW_EXIT_TIMEOUT;
        wstatus = (status << 8) & 0xFF00; // TODO: use __W_EXITCODE() instead? is this portable?
    }

    gettimeofday(&tv_end, NULL);
    double mtime = (tv_end.tv_sec - tv_start.tv_sec) * 1000.0 + (tv_end.tv_usec - tv_start.tv_usec) / 1000.0; // in ms

    if (fw->logfile)
        close(fd);
    else if (fw->cmd)
    {
        int pwstatus = pclose(stream);
        // printf("pclose return %d\n", WEXITSTATUS(pwstatus));
        if (wstatus == 0)
            wstatus = pwstatus;
    }

    if (!fw->silent)
        print_diag_test(stdout, t, wstatus, mtime);

    return (WIFEXITED(wstatus) && !WEXITSTATUS(wstatus)) ? 0 : 1; // if failure, return 1, else 0
}

/* ********** RUN TEST (PARALLEL FORK MODE) ********** */

static int run_test_forkp(struct testfw_t *fw, struct test_t *t, int argc, char *argv[])
{
    if (fork() == 0)
    {
        int r = run_test_forks(fw, t, argc, argv);
        exit(r); // 0 ou 1
    }
    return EXIT_SUCCESS;
}

/* ********** RUN TEST (NOFORK MODE) ********** */

static int run_test_nofork(struct testfw_t *fw, struct test_t *t, int argc, char *argv[])
{
    assert(t);
    int wstatus = 0;

    /* open log file */
    int fd = -1;
    int fdout = -1;
    int fderr = -1;
    if (fw->logfile)
    {
        fd = open(fw->logfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
        fdout = dup(1);
        fderr = dup(2);
        dup2(fd, 1);
        dup2(fd, 2);
        close(fd);
    }

    if (fw->timeout > 0)
        alarm(fw->timeout); // TODO: should be catch to return status 124

    struct timeval tv_start, tv_end;
    gettimeofday(&tv_start, NULL);
    fflush(stdout);
    fflush(stderr);

    int status = t->func(argc, argv);
    wstatus = (status << 8) & 0xFF00; // TODO: is this portable?

    gettimeofday(&tv_end, NULL);
    double mtime = (tv_end.tv_sec - tv_start.tv_sec) * 1000.0 + (tv_end.tv_usec - tv_start.tv_usec) / 1000.0; // in ms

    /* cancel alarm */
    if (fw->timeout > 0)
        alarm(0);

    /* restore standard out & err */
    if (fw->logfile)
    {
        dup2(fdout, 1);
        dup2(fderr, 2);
    }
    if (!fw->silent)
        print_diag_test(stdout, t, wstatus, mtime);

    return (status == 0) ? 0 : 1;
}

/* ********** RUN TEST  ********** */

static int run_test(struct testfw_t *fw, struct test_t *t, int argc, char *argv[], enum testfw_mode_t mode)
{
    if (!fw->silent && fw->verbose)
        printf("******************** RUN TEST \"%s.%s\" ********************\n", t->suite, t->name);

    switch (mode)
    {
    case TESTFW_FORKS:
        return run_test_forks(fw, t, argc, argv);
    case TESTFW_FORKP:
        return run_test_forkp(fw, t, argc, argv);
    case TESTFW_NOFORK:
        return run_test_nofork(fw, t, argc, argv);
    default:
        fprintf(stderr, "Error: invalid execution mode (%d)!\n", mode);
        exit(EXIT_FAILURE);
    }

    return EXIT_FAILURE;
}

static int wait_all_tests_forkp(struct testfw_t *fw)
{
    assert(fw);
    int nfailures = 0;
    for (int i = 0; i < fw->size; i++)
    {
        int wstatus = 0;
        int r = wait(&wstatus);
        assert(r > 0);
        nfailures += ((WIFEXITED(wstatus) && !WEXITSTATUS(wstatus)) ? 0 : 1);
    }
    return nfailures;
}

int testfw_run_all(struct testfw_t *fw, int argc, char *argv[], enum testfw_mode_t mode)
{
    assert(fw);
    int nfailures = 0;
    for (int i = 0; i < fw->size; i++)
    {
        struct test_t *t = &fw->tests[i];
        assert(t);
        nfailures += run_test(fw, t, argc, argv, mode);
    }

    if (mode == TESTFW_FORKP)
        nfailures = wait_all_tests_forkp(fw);

    return nfailures;
}
