#include <stdlib.h>
#include <stdbool.h>
#include "testfw.h"
#include "sample.h"

#define TIMEOUT 2
#define LOGFILE "test.log"
#define SILENT false

int main(int argc, char *argv[])
{
    struct testfw_t *fw = testfw_init(argv[0], TIMEOUT, LOGFILE, SILENT);
    testfw_register_func(fw, "sample", "success", sample_success);
    testfw_register_symb(fw, "sample", "failure");
    testfw_register_suite(fw, "othersample");
    testfw_run_all(fw, argc - 1, argv + 1, TESTFW_FORK);
    testfw_free(fw);
    return EXIT_SUCCESS;
}