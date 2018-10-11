#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "testfw.h"


int test_success(int argc, char* argv[])
{
    printf("hello world\n");
    return EXIT_SUCCESS;
}

/**
 * @brief return failure
 */
int test_failure(int argc, char* argv[])
{
    return EXIT_FAILURE;
}

/**
 * @brief return negative value
 */
// int test_negret(int argc, char* argv[])
// {
//     return -1;
// }

/**
 * @brief return positive value > 0
 */
// int test_posret(int argc, char* argv[])
// {
//     return 2;
// }

/**
 * @brief test segmentation fault (memory access violation)
 */
int test_segfault(int argc, char* argv[])
{
    *((int *)0) = 0;   // segfault!
    return EXIT_SUCCESS;
}

/**
 * @brief test assert
 */
int test_assert(int argc, char* argv[])
{
    assert(1 == 2);
    return EXIT_SUCCESS;
}

/**
 * @brief test sleep (timeout error)
 */
int test_sleep(int argc, char* argv[])
{
    sleep(10);         // 10 seconds
    return EXIT_SUCCESS;
}

