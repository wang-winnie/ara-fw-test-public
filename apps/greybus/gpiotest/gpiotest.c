/*
 * Copyright (c) 2015 Google, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>

#include <libfwtest.h>
#include "commsteps.h"

struct gpio_app_info {
    uint16_t    case_id;
    uint16_t    base_pin;
    uint16_t    max_count;
    uint16_t    gpio_pin1;
    uint16_t    gpio_pin2;
    uint16_t    gpio_pin3;
    char        num_type[2];
};

/**
 * @brief Print usage of this GPIO test application
 *
 * @return None
 */
static void print_usage()
{
    printf("\nUsage: gpiotest [-c case-id] [-t number-type] [-1 gpio-pin1]"
           "[-2 gpio-pin2] [-3 gpio-pin3]\n");
    printf("    -c: Testrail test case ID.\n");
    printf("    -t: 's' for Single pin test or 'm' for Multiple pins test.\n");
    printf("    -1: GPIO pin1 number for single pin or multiple pins test\n");
    printf("    -2: GPIO pin2 number for multiple pins test\n");
    printf("    -3: GPIO pin3 number for multiple pins test\n");
    printf("Example : case C1031 use SDB board, GPIO had 3 pins can\n");
    printf("     test(GPIO0 GPIO8 GPIO9)\n");
    printf("     ./gpiotest -c 1031 -t m -1 0 -2 8 -3 9\n");
 }

/**
 * @brief Set SDIO default parameters
 *
 * This is function to set GPIO parameters using in this application to default
 * value
 *
 * @param info The GPIO info from user
 * @return None
 */
static void default_params(struct gpio_app_info *info)
{
    info->case_id = 0;
    info->base_pin = 0;
    info->max_count = 0;
    snprintf(info->num_type, sizeof("") + 1, "%s", "");
    info->gpio_pin1 = 0;
    info->gpio_pin2 = 0;
    info->gpio_pin3 = 0;
}

/**
 * @brief Command parser
 *
 * Parse the command input from console
 *
 * @param info The GPIO info from user
 * @param argc Indicate how many input parameters
 * @param argv Commands input from console
 * @return 0 on success, negative errno on error
 */
static int command_parse(struct gpio_app_info *info, int argc, char **argv)
{
    int option;

    while ((option = getopt(argc, argv, "c:C:t:T:1:2:3:")) != ERROR) {
        switch(option) {
            case 'c':
                info->case_id = (uint16_t)atoi(optarg);
                break;
            case 'C':
                info->case_id = (uint16_t)atoi(optarg);
                break;
            case 't':
                snprintf(info->num_type, strlen(optarg) + 1, "%s", optarg);
                break;
            case 'T':
                snprintf(info->num_type, strlen(optarg) + 1, "%s", optarg);
                break;
            case '1':
                info->gpio_pin1 = (uint16_t)atoi(optarg);
                break;
            case '2':
                info->gpio_pin2 = (uint16_t)atoi(optarg);
                break;
            case '3':
                info->gpio_pin3 = (uint16_t)atoi(optarg);
                break;
            default:
                print_usage();
                return -EINVAL;
        }
    }

    return 0;
}

/**
 * @brief Testrail test case C1028
 *
 * C1028: GPIO line count response contains the number of GPIO lines used by
 * the GPIO Controller. This test case verifies that the GPIO Line Count
 * Response payload contains a one-byte value corresponding to the number of
 * lines managed by the GPIO Controller
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_1028_get_count(struct gpio_app_info *info)
{
    int ret = 0;
    char gpiostr[PATH_MAX];
    /* Get debugfs ngpio buffer string */
    char countbuf[4];

    /* Read debugfs "ngpio to set GPIO_MAX_COUNT" */
    ret = get_greybus_gpio_count(info->base_pin, countbuf,
                                 sizeof(countbuf));
    info->max_count = atoi(countbuf);

    snprintf(gpiostr, sizeof(gpiostr), "GPIO count: %d", info->max_count);
    print_test_case_log(LOG_TAG, info->case_id, gpiostr);

    print_test_result(info->case_id, ret);

    return ret;
}

/**
 * @brief Testrail test case C1029
 *
 * C1029: Generate multiple GPIO activate Request. This test case verifies
 * that multiple GPIO Activate Request operations can be executed successfully
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_1029_multiple_activate(struct gpio_app_info *info)
{
    int ret = 0;

    /* Activate multiple GPIO pins */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = activate_gpio_multiple_pin(info->case_id,
                                        (info->base_pin + info->gpio_pin1),
                                        (info->base_pin + info->gpio_pin2),
                                        (info->base_pin + info->gpio_pin3));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id,
                               (info->base_pin + info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status.*/
    /* Deactivate GPIO multiple pins */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = deactivate_gpio_multiple_pin(info->case_id,
                                          (info->base_pin + info->gpio_pin1),
                                          (info->base_pin + info->gpio_pin2),
                                          (info->base_pin + info->gpio_pin3));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id,
                                 (info->base_pin + info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief Testrail test case C1030
 *
 * C1030: Generate multiple GPIO Deactivate Request. This test case verifies
 * that multiple GPIO Deactivate Request operations can be executed successfully
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_1030_multiple_deactivate(struct gpio_app_info *info)
{
    int ret = 0;

    /* Activate multiple GPIO pins */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = activate_gpio_multiple_pin(info->case_id,
                                        (info->base_pin + info->gpio_pin1),
                                        (info->base_pin + info->gpio_pin2),
                                        (info->base_pin + info->gpio_pin3));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id,
                               (info->base_pin + info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Deactivate multiple GPIO pins */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = deactivate_gpio_multiple_pin(info->case_id,
                                          (info->base_pin + info->gpio_pin1),
                                          (info->base_pin + info->gpio_pin2),
                                          (info->base_pin + info->gpio_pin3));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id,
                                 (info->base_pin + info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    return ret;
}

/**
 * @brief Testrail test case C1031
 *
 * C1031: Generate multiple GPIO Direction Request. This test case verifies
 * that multiple GPIO Direction Request operations can be executed successfully
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_1031_multiple_direction(struct gpio_app_info *info)
{
    int ret = 0;
    /* Get GPIO direction buffer */
    char directbuf[4];

    /* Activate multiple GPIO pins */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = activate_gpio_multiple_pin(info->case_id,
                                        (info->base_pin + info->gpio_pin1),
                                        (info->base_pin + info->gpio_pin2),
                                        (info->base_pin + info->gpio_pin3));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id,
                               (info->base_pin + info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get multiple GPIO direction */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = get_gpio_direction(info->case_id,
                                (info->base_pin + info->gpio_pin1),
                                 directbuf, sizeof(directbuf));
        ret = get_gpio_direction(info->case_id,
                                (info->base_pin + info->gpio_pin2),
                                 directbuf, sizeof(directbuf));
        ret = get_gpio_direction(info->case_id,
                                (info->base_pin + info->gpio_pin3),
                                 directbuf, sizeof(directbuf));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_direction(info->case_id,
                                (info->base_pin + info->gpio_pin1),
                                     directbuf, sizeof(directbuf));
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status.*/
    /* Deactivate multiple GPIO pins */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = deactivate_gpio_multiple_pin(info->case_id,
                                          (info->base_pin + info->gpio_pin1),
                                          (info->base_pin + info->gpio_pin2),
                                          (info->base_pin + info->gpio_pin3));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id,
                                 (info->base_pin + info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief Testrail test case C1032
 *
 * C1032: GPIO Direction Request multiple times for the same GPIO line. This
 * test case verifies that multiple GPIO Direction Request operations for the
 * same GPIO line do not generate an error message
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_1032_multiple_times_direction(struct gpio_app_info *info)
{
    int ret = 0, i = 0;
    /* Get GPIO direction buffer */
    char directbuf[4];
    /* Testrail test case test 10 times */
    int test_times = 10;

    /* Activate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Read GPIO direction 10 times */
    for(i = 0; i < test_times; i++) {
        if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
            ret = get_gpio_direction(info->case_id, (info->base_pin +
                                     info->gpio_pin1), directbuf,
                                     sizeof(directbuf));
        } else {
            ret = -EINVAL;
        }
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief Testrail test case C1033
 *
 * C1033: GPIO Direction Request for all the GPIO lines. This test case
 * verifies that GPIO Direction Request Operations can be initiated for all the
 * GPIO lines including lines that have not been activated
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_1033_all_direction(struct gpio_app_info *info)
{
    int ret = 0, i = 0;
    /* Get GPIO direction buffer */
    char directbuf[4];

    /* Activate all GPIO pins */
    if (!(strncasecmp(info->num_type, "a", strlen("a") + 1))) {
        for (i = 0; i < info->max_count; i++) {
            ret = activate_gpio_pin(info->case_id, (info->base_pin + i));
        }
    } else if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = activate_gpio_multiple_pin(info->case_id,
                                        (info->base_pin + info->gpio_pin1),
                                        (info->base_pin + info->gpio_pin2),
                                        (info->base_pin + info->gpio_pin3));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id,
                               (info->base_pin + info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO direction */
    if (!(strncasecmp(info->num_type, "a", strlen("a") + 1))) {
        for (i = 0; i < info->max_count; i++) {
            ret = get_gpio_direction(info->case_id, (info->base_pin + i),
                                     directbuf, sizeof(directbuf));
        }
    } else if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = get_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), directbuf,
                                 sizeof(directbuf));
        ret = get_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin2), directbuf,
                                 sizeof(directbuf));
        ret = get_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin3), directbuf,
                                 sizeof(directbuf));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), directbuf,
                                 sizeof(directbuf));
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status.*/
    /* Deactivate all GPIO pins */
    if (!(strncasecmp(info->num_type, "a", strlen("a") + 1))) {
        for (i = 0; i < info->max_count; i++) {
            ret = deactivate_gpio_pin(info->case_id, (info->base_pin + i));
        }
    } else if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = deactivate_gpio_multiple_pin(info->case_id,
                                          (info->base_pin + info->gpio_pin1),
                                          (info->base_pin + info->gpio_pin2),
                                          (info->base_pin + info->gpio_pin3));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief Testrail test case C1034
 *
 * C1034: Generate multiple GPIO Direction Input Request. This test case
 * verifies that multiple GPIO Direction Input Request operations can be
 * executed successfully
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_1034_multiple_input(struct gpio_app_info *info)
{
    int ret = 0;
    /* GPIO debugfs buffer */
    char buf[4];
    /* Set GPIO direction string */
    char inputGPIODir[] = "in";

    /* Activate multiple GPIO pins */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = activate_gpio_multiple_pin(info->case_id,
                                        (info->base_pin + info->gpio_pin1),
                                        (info->base_pin + info->gpio_pin2),
                                        (info->base_pin + info->gpio_pin3));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id,
                               (info->base_pin + info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is input */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), inputGPIODir,
                                 sizeof(inputGPIODir));
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin2), inputGPIODir,
                                 sizeof(inputGPIODir));
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin3), inputGPIODir,
                                 sizeof(inputGPIODir));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), inputGPIODir,
                                 sizeof(inputGPIODir));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO direction, and verify GPIO direction is input */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = get_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, inputGPIODir);
        }

        ret = get_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin2), buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, inputGPIODir);
        }

        ret = get_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin3), buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, inputGPIODir);
        }
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, inputGPIODir);
        }
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate multiple GPIO pins */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = deactivate_gpio_multiple_pin(info->case_id,
                                          (info->base_pin + info->gpio_pin1),
                                          (info->base_pin + info->gpio_pin2),
                                          (info->base_pin + info->gpio_pin3));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id,
                                 (info->base_pin + info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief Testrail test case C1035
 *
 * C1035: GPIO direction input request multiple times for the same GPIO line.
 * This test case verifies that multiple GPIO Direction Input Request operations
 * for the same GPIO line do not generate an error message
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_1035_multiple_times_input(struct gpio_app_info *info)
{
    int ret = 0, i = 0;
    /* GPIO debugfs buffer */
    char buf[4];
    /* Set GPIO direction string */
    char inputGPIODir[] = "in";
    /* Testrail test case test 10 times */
    int test_times = 10;

    /* Activate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is input and set 10 times. */
    for (i = 0; i < test_times; i++) {
        if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
            ret = set_gpio_direction(info->case_id, (info->base_pin +
                                     info->gpio_pin1), inputGPIODir,
                                     sizeof(inputGPIODir));
        } else {
            ret = -EINVAL;
        }
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO direction, and verify GPIO direction is input */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, inputGPIODir);
        }
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief Testrail test case C1036
 *
 * C1036: Generate multiple GPIO direction output request. This test case
 * verifies that multiple GPIO Direction Output Request operations can be
 * executed successfully.
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_1036_multiple_output(struct gpio_app_info *info)
{
    int ret = 0, i = 0;
    /* GPIO debugfs buffer */
    char buf[4];
    /* Set GPIO direction string */
    char outputGPIODir[] = "out";

    /* Activate multiple GPIO pins */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = activate_gpio_multiple_pin(info->case_id,
                                        (info->base_pin + info->gpio_pin1),
                                        (info->base_pin + info->gpio_pin2),
                                        (info->base_pin + info->gpio_pin3));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id,
                               (info->base_pin + info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is output */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), outputGPIODir,
                                 sizeof(outputGPIODir));
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin2), outputGPIODir,
                                 sizeof(outputGPIODir));
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin3), outputGPIODir,
                                 sizeof(outputGPIODir));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), outputGPIODir,
                                 sizeof(outputGPIODir));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO direction, and verify GPIO direction is output */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = get_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, outputGPIODir);
        }

        ret = get_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin2), buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, outputGPIODir);
        }

        ret = get_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin3), buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, outputGPIODir);
        }
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, outputGPIODir);
        }
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate multiple GPIO pins */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = deactivate_gpio_multiple_pin(info->case_id,
                                          (info->base_pin + info->gpio_pin1),
                                          (info->base_pin + info->gpio_pin2),
                                          (info->base_pin + info->gpio_pin3));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id,
                                 (info->base_pin + info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief Testrail test case C1037
 *
 * C1037: GPIO direction output request multiple times for the same line. This
 * test case verifies that multiple GPIO Direction Output Request operations for
 * the same GPIO line do not generate an error message
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_1037_multiple_times_output(struct gpio_app_info *info)
{
    int ret = 0, i = 0;
    /* GPIO debugfs buffer */
    char buf[4];
    /* Set GPIO direction string */
    char outputGPIODir[] = "out";
    /* Testrail test case test 10 times */
    int test_times = 10;

    /* Activate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is output and set 10 times. */
    for (i = 0; i < test_times; i++) {
        if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
            ret = set_gpio_direction(info->case_id, (info->base_pin +
                                     info->gpio_pin1), outputGPIODir,
                                     sizeof(outputGPIODir));
        } else {
            ret = -EINVAL;
        }
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO direction, and verify GPIO direction is output */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, outputGPIODir);
        }
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief Testrail test case C1038
 *
 * C1038: GPIO get response payload returns GPIO line current value. This test
 * case verifies that the GPIO Get Response payload contains a 1-byte value
 * indicating the GPIO Line Value
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_1038_get_value(struct gpio_app_info *info)
{
    int ret = 0;
    /* GPIO debugfs buffer */
    char buf[4];
    /* Set GPIO direction string */
    char inputGPIODir[] = "in";

    /* activate multiple GPIO pins */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = activate_gpio_multiple_pin(info->case_id,
                                        (info->base_pin + info->gpio_pin1),
                                        (info->base_pin + info->gpio_pin2),
                                        (info->base_pin + info->gpio_pin3));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id,
                               (info->base_pin + info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is input */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), inputGPIODir,
                                 sizeof(inputGPIODir));
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin2), inputGPIODir,
                                 sizeof(inputGPIODir));
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin3), inputGPIODir,
                                 sizeof(inputGPIODir));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), inputGPIODir,
                                 sizeof(inputGPIODir));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO value */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = get_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             buf, sizeof(buf));
        ret = get_gpio_value(info->case_id, (info->base_pin + info->gpio_pin2),
                             buf, sizeof(buf));
        ret = get_gpio_value(info->case_id, (info->base_pin + info->gpio_pin3),
                             buf, sizeof(buf));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate multiple GPIO pins */
    if (!(strncasecmp(info->num_type, "m", strlen("m") + 1))) {
        ret = deactivate_gpio_multiple_pin(info->case_id,
                                          (info->base_pin + info->gpio_pin1),
                                          (info->base_pin + info->gpio_pin2),
                                          (info->base_pin + info->gpio_pin3));
    } else if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id,
                                 (info->base_pin + info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief Testrail test case C1039
 *
 * C1039: Set GPIO line to high. This test case verifies that a given GPIO
 * Line can be set to HIGH using the GPIO Set Request operation.
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_1039_set_value_high(struct gpio_app_info *info)
{
    int ret = 0;
    /* GPIO debugfs buffer */
    char buf[4];
    /* Set GPIO direction string */
    char outputGPIODir[] = "out";
    /* Set GPIO value string */
    char highGPIOValue[] = "1";

    /* activate GPIO pins */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is output */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), outputGPIODir,
                                 sizeof(outputGPIODir));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO value is 1(high) */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             highGPIOValue, sizeof(highGPIOValue));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO direction, and verify GPIO direction is output */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, outputGPIODir);
        }
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO value, and verify GPIO value is 1 */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, highGPIOValue);
        }
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief Testrail test case C1040
 *
 * C1040: Set GPIO line to low. This test case verifies that a given GPIO Line
 * can be set to LOW using the GPIO Set Request operation
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_1040_set_value_low(struct gpio_app_info *info)
{
    int ret = 0;
    /* GPIO debugfs buffer */
    char buf[4];
    /* Set GPIO direction string */
    char outputGPIODir[] = "out";
    /* Set GPIO value string */
    char lowGPIOValue[] = "0";

    /* activate GPIO pins */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is output */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), outputGPIODir,
                                 sizeof(outputGPIODir));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO value is 0(low) */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             lowGPIOValue, sizeof(lowGPIOValue));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO direction, and verify GPIO direction is output */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, outputGPIODir);
        }
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

   /* Get GPIO value, and verify GPIO value is 0 */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, lowGPIOValue);
        }
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief Testrail test case C1041
 *
 * C1041: GPIO IRQ type can be set to EDGE_RISING. This test case verifies
 * that the GPIO IRQ Type Response doesn't return an error when setting the GPIO
 * IRQ Type is set to IRQ_TYPE_EDGE_RISING
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_1041_set_edge_rising(struct gpio_app_info *info)
{
    int ret = 0;
    /* GPIO debugfs buffer */
    char buf[8];
    /* Set GPIO direction string */
    char outputGPIODir[] = "out";
    /* Set GPIO value string */
    char highGPIOValue[] = "1";
    /* Set GPIO edge string */
    char risGPIOEdge[] = "rising";

    /* Activate GPIO pins */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is output */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), outputGPIODir,
                                 sizeof(outputGPIODir));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO value is 1(high) */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             highGPIOValue, sizeof(highGPIOValue));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO edge is rising */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            risGPIOEdge, sizeof(risGPIOEdge));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO edge, and verify GPIO edge is rising */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, risGPIOEdge);
        }
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief Testrail test case C1042
 *
 * C1042: GPIO IRQ type can be set to EDGE_FALLING. This test case verifies
 * that the GPIO IRQ Type Response doesn't return an error when setting the GPIO
 * IRQ Type is set to IRQ_TYPE_EDGE_FALLING.
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_1042_set_edge_falling(struct gpio_app_info *info)
{
    int ret = 0;
    /* GPIO debugfs buffer */
    char buf[8];
    /* Set GPIO direction string */
    char outputGPIODir[] = "out";
    /* Set GPIO value string */
    char highGPIOValue[] = "1";
    /* Set GPIO edge string */
    char fallGPIOEdge[] = "falling";

    /* Activate GPIO pins */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is output */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), outputGPIODir,
                                 sizeof(outputGPIODir));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO value is 1(high) */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             highGPIOValue, sizeof(highGPIOValue));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO edge is falling */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            fallGPIOEdge, sizeof(fallGPIOEdge));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO edge, and verify GPIO edge is falling */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, fallGPIOEdge);
        }
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief Testrail test case C1043
 *
 * C1043: GPIO IRQ type can be set to EDGE_BOTH. This test case verifies that
 * the GPIO IRQ Type Response doesn't return an error when setting the GPIO IRQ
 * Type is set to IRQ_TYPE_EDGE_BOTH.
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_1043_set_edge_both(struct gpio_app_info *info)
{
    int ret = 0;
    /* GPIO debugfs buffer */
    char buf[8];
    /* Set GPIO direction string */
    char outputGPIODir[] = "out";
    /* Set GPIO value string */
    char highGPIOValue[] = "1";
    /* Set GPIO edge string */
    char bothGPIOEdge[] = "both";

    /* activate GPIO pins */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is output */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), outputGPIODir,
                                 sizeof(outputGPIODir));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO value is 1(high) */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             highGPIOValue, sizeof(highGPIOValue));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO edge is both */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            bothGPIOEdge, sizeof(bothGPIOEdge));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO edge, and verify GPIO edge is both */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, bothGPIOEdge);
        }
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief Testrail test case C1044
 *
 * C1044: Change input line to output line. This test case verifies that a
 * previously configure GPIO line as an Input Line can be reconfigured to an
 * Output Line
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_1044_input_to_output(struct gpio_app_info *info)
{
    int ret = 0;
    /* GPIO debugfs buffer */
    char buf[4];
    /* Set GPIO direction string */
    char inputGPIODir[] = "in";
    /* Set GPIO direction string */
    char outputGPIODir[] = "out";
    /* Set GPIO value string */
    char highGPIOValue[] = "1";

    /* activate GPIO pins */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is input */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), inputGPIODir,
                                 sizeof(inputGPIODir));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO value */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* activate GPIO pins */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is output */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), outputGPIODir,
                                 sizeof(outputGPIODir));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO value is 1(high) */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             highGPIOValue, sizeof(highGPIOValue));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO value, and verify GPIO value is 1 */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, highGPIOValue);
        }
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief Testrail test case C1045
 *
 * C1045: Change output line to input line. This test case verifies that a
 * previously configure GPIO line as an output line can be reconfigured to an
 * input line
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_1045_output_to_input(struct gpio_app_info *info)
{
    int ret = 0;
    /* GPIO debugfs buffer */
    char buf[4];
    /* Set GPIO direction string */
    char outputGPIODir[] = "out";
    /* Set GPIO direction string */
    char inputGPIODir[] = "in";
    /* Set GPIO value string */
    char highGPIOValue[] = "1";

    /* activate GPIO pins */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is output */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), outputGPIODir,
                                 sizeof(outputGPIODir));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO value is 1(high) */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             highGPIOValue, sizeof(highGPIOValue));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* activate GPIO pins */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is input */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), inputGPIODir,
                                 sizeof(inputGPIODir));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO value*/
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             buf, sizeof(buf));
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief Testrail test case C1046
 *
 * C1046: Change IRQ type from falling edge to rising edge. This test case
 * verifies that the IRQ type can be changed from IRQ_TYPE_EDGE_FALLIN to
 * IRQ_TYPE_EDGE_RISING
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_1046_falling_to_rising(struct gpio_app_info *info)
{
    int ret = 0;
    /* GPIO debugfs buffer */
    char buf[8];
    /* Set GPIO direction string */
    char outputGPIODir[] = "out";
    /* Set GPIO value string */
    char highGPIOValue[] = "1";
    /* Set GPIO edge string */
    char fallGPIOEdge[] = "falling";
    /* Set GPIO edge string */
    char risGPIOEdge[] = "rising";

    /* activate GPIO pins */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is output */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), outputGPIODir,
                                 sizeof(outputGPIODir));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO value is 1(high) */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             highGPIOValue, sizeof(highGPIOValue));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO edge is falling */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            fallGPIOEdge, sizeof(fallGPIOEdge));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO edge, and verify GPIO edge is falling */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, fallGPIOEdge);
        }
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO edge is rising */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            risGPIOEdge, sizeof(risGPIOEdge));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO edge, and verify GPIO edge is rising */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, risGPIOEdge);
        }
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief Testrail test case C1047
 *
 * C1047: Change IRQ type from rising edge to falling edge. This test case
 * verifies that the IRQ type can be changed from IRQ_TYPE_EDGE_RISING to
 * IRQ_TYPE_EDGE_FALLING
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_1047_rising_to_falling(struct gpio_app_info *info)
{
    int ret = 0;
    /* GPIO debugfs buffer */
    char buf[8];
    /* Set GPIO direction string */
    char outputGPIODir[] = "out";
    /* Set GPIO value string */
    char highGPIOValue[] = "1";
    /* Set GPIO edge string */
    char risGPIOEdge[] = "rising";
    /* Set GPIO edge string */
    char fallGPIOEdge[] = "falling";

    /* activate GPIO pins */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is output */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), outputGPIODir,
                                 sizeof(outputGPIODir));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO value is 1(high) */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             highGPIOValue, sizeof(highGPIOValue));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO edge is rising */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            risGPIOEdge, sizeof(risGPIOEdge));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO edge, and verify GPIO edge is rising */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, risGPIOEdge);
        }
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO edge is falling */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            fallGPIOEdge, sizeof(fallGPIOEdge));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO edge, and verify GPIO edge is falling */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, fallGPIOEdge);
        }
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief Testrail test case C1048
 *
 * C1048: Change IRQ type from rising edge to falling edge triggered. This
 * test case verifies that the IRQ type can be changed from IRQ_TYPE_EDGE_RISING
 * to IRQ_TYPE_EDGE_BOTH
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_1048_rising_to_both(struct gpio_app_info *info)
{
    int ret = 0;
    /* GPIO debugfs buffer */
    char buf[8];
    /* Set GPIO direction string */
    char outputGPIODir[] = "out";
    /* Set GPIO value string */
    char highGPIOValue[] = "1";
    /* Set GPIO edge string */
    char risGPIOEdge[] = "rising";
    /* Set GPIO edge string */
    char bothGPIOEdge[] = "both";

    /* activate GPIO pins */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is output */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), outputGPIODir,
                                 sizeof(outputGPIODir));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO value is 1(high) */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             highGPIOValue, sizeof(highGPIOValue));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO edge is rising */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            risGPIOEdge, sizeof(risGPIOEdge));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO edge, and verify GPIO edge is rising */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, risGPIOEdge);
        }
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO edge is both */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            bothGPIOEdge, sizeof(bothGPIOEdge));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO edge, and verify GPIO edge is both */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, bothGPIOEdge);
        }
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief Testrail test case C1049
 *
 * C1049: Change IRQ type from none to rising and falling edge. This test case
 * verifies that the IRQ type can be changed from IRQ_TYPE_NONE to
 * IRQ_TYPE_EDGE_BOTH
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_1049_none_to_both(struct gpio_app_info *info)
{
    int ret = 0;
    /* GPIO debugfs buffer */
    char buf[8];
    /* Set GPIO direction string */
    char outputGPIODir[] = "out";
    /* Set GPIO value string */
    char highGPIOValue[] = "1";
    /* Set GPIO edge string */
    char noneGPIOEdge[] = "none";
    /* Set GPIO edge string */
    char bothGPIOEdge[] = "both";

    /* activate GPIO pins */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is output */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), outputGPIODir,
                                 sizeof(outputGPIODir));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO value is 1(high) */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             highGPIOValue, sizeof(highGPIOValue));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO edge is none */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            noneGPIOEdge, sizeof(noneGPIOEdge));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO edge, and verify GPIO edge is none */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, noneGPIOEdge);
        }
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO edge is both */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            bothGPIOEdge, sizeof(bothGPIOEdge));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO edge, and verify GPIO edge is both */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, bothGPIOEdge);
        }
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief Testrail test case C1050
 *
 * C1050: Change IRQ type from rising and falling edge to none. This test case
 * verifies that the IRQ type can be changed from IRQ_TYPE_EDGE_BOTH to
 * IRQ_TYPE_NONE.
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_1050_both_to_none(struct gpio_app_info *info)
{
    int ret = 0;
    /* GPIO debugfs buffer */
    char buf[8];
    /* Set GPIO direction string */
    char outputGPIODir[] = "out";
    /* Set GPIO value string */
    char highGPIOValue[] = "1";
    /* Set GPIO edge string */
    char bothGPIOEdge[] = "both";
    /* Set GPIO edge string */
    char noneGPIOEdge[] = "none";

    /* Activate GPIO pins */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO direction is output */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), outputGPIODir,
                                 sizeof(outputGPIODir));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO value is 1(high) */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             highGPIOValue, sizeof(highGPIOValue));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO edge is both */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            bothGPIOEdge, sizeof(bothGPIOEdge));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO edge, and verify GPIO edge is both */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, bothGPIOEdge);
        }
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIO edge is none */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = set_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            noneGPIOEdge, sizeof(noneGPIOEdge));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIO edge, and verify GPIO edge is none */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = get_gpio_edge(info->case_id, (info->base_pin + info->gpio_pin1),
                            buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, noneGPIOEdge);
        }
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "s", strlen("s") + 1))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief Testrail test case C2542
 *
 * C2542: GPIO loopback test.
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_2542_loopback(struct gpio_app_info *info)
{
    int ret = 0;
    /* GPIO debugfs buffer */
    char buf[8];
    /* Set GPIO direction string */
    char inputGPIODir[] = "in";
    /* Set GPIO direction string */
    char outputGPIODir[] = "out";
    /* Set GPIO value string */
    char lowGPIOValue[] = "0";
    /* Set GPIO value string */
    char highGPIOValue[] = "1";

    /* Activate GPIO pinA */
    if (!(strncasecmp(info->num_type, "m", strlen("m")))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Activate GPIO pinB */
    if (!(strncasecmp(info->num_type, "m", strlen("m")))) {
        ret = activate_gpio_pin(info->case_id, (info->base_pin +
                                info->gpio_pin2));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIOA direction is output */
    if (!(strncasecmp(info->num_type, "m", strlen("m")))) {
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin1), outputGPIODir,
                                 sizeof(outputGPIODir));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIOB direction is input */
    if (!(strncasecmp(info->num_type, "m", strlen("m")))) {
        ret = set_gpio_direction(info->case_id, (info->base_pin +
                                 info->gpio_pin2), inputGPIODir,
                                 sizeof(inputGPIODir));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIOA value is 0(Low) */
    if (!(strncasecmp(info->num_type, "m", strlen("m")))) {
        ret = set_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             lowGPIOValue, sizeof(lowGPIOValue));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIOB value, and verify GPIOB value is 0 */
    if (!(strncasecmp(info->num_type, "m", strlen("m")))) {
        ret = get_gpio_value(info->case_id, (info->base_pin + info->gpio_pin2),
                             buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, lowGPIOValue);
        }
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Set GPIOA value is 1(High) */
    if (!(strncasecmp(info->num_type, "m", strlen("m")))) {
        ret = set_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             highGPIOValue, sizeof(highGPIOValue));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Get GPIOB value, and verify GPIOB value is 1 */
    if (!(strncasecmp(info->num_type, "m", strlen("m")))) {
        ret = get_gpio_value(info->case_id, (info->base_pin + info->gpio_pin2),
                             buf, sizeof(buf));
        if (!ret) {
            ret = strcmp(buf, highGPIOValue);
        }
    } else {
        ret = -EINVAL;
    }

    print_test_result(info->case_id, ret);

    /* Post-condition: Recover pre-test status */
    /* Set GPIOA value is 0(Low) */
    if (!(strncasecmp(info->num_type, "m", strlen("m")))) {
        ret = set_gpio_value(info->case_id, (info->base_pin + info->gpio_pin1),
                             lowGPIOValue, sizeof(lowGPIOValue));
    } else {
        ret = -EINVAL;
    }

    check_step_result(info->case_id, ret);

    /* Deactivate GPIO pin */
    if (!(strncasecmp(info->num_type, "m", strlen("m")))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin1));
    } else {
        ret = -EINVAL;
    }

    if (!(strncasecmp(info->num_type, "m", strlen("m")))) {
        ret = deactivate_gpio_pin(info->case_id, (info->base_pin +
                                  info->gpio_pin2));
    } else {
        ret = -EINVAL;
    }

    return ret;
}

/**
 * @brief Switch case number and check case number is correct.
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int switch_case_number(struct gpio_app_info *info)
{
        switch (info->case_id) {
            case 1028:
                return ARA_1028_get_count(info);
            case 1029:
                return ARA_1029_multiple_activate(info);
            case 1030:
                return ARA_1030_multiple_deactivate(info);
            case 1031:
                return ARA_1031_multiple_direction(info);
            case 1032:
                return ARA_1032_multiple_times_direction(info);
            case 1033:
                return ARA_1033_all_direction(info);
            case 1034:
                return ARA_1034_multiple_input(info);
            case 1035:
                return ARA_1035_multiple_times_input(info);
            case 1036:
                return ARA_1036_multiple_output(info);
            case 1037:
                return ARA_1037_multiple_times_output(info);
            case 1038:
                return ARA_1038_get_value(info);
            case 1039:
                return ARA_1039_set_value_high(info);
            case 1040:
                return ARA_1040_set_value_low(info);
            case 1041:
                return ARA_1041_set_edge_rising(info);
            case 1042:
                return ARA_1042_set_edge_falling(info);
            case 1043:
                return ARA_1043_set_edge_both(info);
            case 1044:
                return ARA_1044_input_to_output(info);
            case 1045:
                return ARA_1045_output_to_input(info);
            case 1046:
                return ARA_1046_falling_to_rising(info);
            case 1047:
                return ARA_1047_rising_to_falling(info);
            case 1048:
                return ARA_1048_rising_to_both(info);
            case 1049:
                return ARA_1049_none_to_both(info);
            case 1050:
                return ARA_1050_both_to_none(info);
            case 2542:
                return ARA_2542_loopback(info);
            default:
                print_test_case_log(LOG_TAG, 0,
                                    "Error: The command had error case_id.");
                return -EINVAL;
        }
}

/**
 * @brief The gpiotest main function
 *
 * @param argc The gpiotest main arguments count
 * @param argv The gpiotest main arguments data
 * @return 0 on success, error code on failure
 */
int main(int argc, char **argv)
{
    struct gpio_app_info info;
    int ret = 0, base_pin = 0, max_count;

    if (argc < 3) {
        print_usage();
        ret = -EINVAL;
    }

    default_params(&info);

    if (!ret) {
        if (command_parse(&info, argc, argv)) {
            ret = -EINVAL;
        }
    }

    /* 1. Check Greybus GPIO controller */
    if (!ret) {
        ret = check_greybus_gpio(&base_pin, &max_count);
        info.base_pin = base_pin;
        info.max_count = max_count;
        check_step_result(info.case_id, ret);
    }

    /* 2. Switch test case */
    if (!ret) {
        ret = switch_case_number(&info);
        check_step_result(info.case_id, ret);
    }

    return ret;
}
