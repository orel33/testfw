#ifndef SAMPLE_H
#define SAMPLE_H

/**
 * @brief return success
 */
int sample_success(int argc, char *argv[]);

/**
 * @brief return failure
 */
int sample_failure(int argc, char *argv[]);

/**
 * @brief test segmentation fault signal
 */
int sample_segfault(int argc, char *argv[]);

/**
 * @brief test alarm signal
 */
int sample_alarm(int argc, char *argv[]);

/**
 * @brief test assert
 */
int sample_assert(int argc, char *argv[]);

/**
 * @brief test sleep (timeout error)
 */
int sample_sleep(int argc, char *argv[]);

/**
 * @brief return negative value
 */
int othersample_negret(int argc, char *argv[]);

/**
 * @brief return positive value > 0
 */
int othersample_posret(int argc, char *argv[]);

#endif