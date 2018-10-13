#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

#include "sample.h"


int sample_success(int argc, char* argv[])
{
    return EXIT_SUCCESS;
}

int sample_failure(int argc, char* argv[])
{
    return EXIT_FAILURE;
}

int sample_segfault(int argc, char* argv[])
{
    *((int *)0) = 0;   // segfault!
    return EXIT_SUCCESS;
}

int sample_alarm(int argc, char *argv[])
{
    alarm(1);
    pause(); // wait signal
    return EXIT_SUCCESS;
}

int sample_assert(int argc, char* argv[])
{
    assert(1 == 2);
    return EXIT_SUCCESS;
}

int sample_sleep(int argc, char* argv[])
{
    sleep(10);         // 10 seconds
    return EXIT_SUCCESS;
}


int othersample_negret(int argc, char* argv[])
{
    return -1;
}

int othersample_posret(int argc, char* argv[])
{
    return 2;
}
