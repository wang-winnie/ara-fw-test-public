/**
 * Copyright (c) 2016 Google, Inc.
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
#include <stdlib.h>
#include <getopt.h>
#include <limits.h>

#include <libfwtest.h>

#include "uart-task.h"

/**
 * @brief Print the usage information
 *
 * @return None.
 */
static void print_usage(void)
{
    printf("\nUsage: uarttest [-c case_id] \n");
    printf("    -c: Testrail test id, supports 7687, 7688, 7689 and 7690.\n\n");

    printf("case 7687, UART configuration test.\n");
    printf("case 7688, UART data transfer test.\n");
    printf("case 7689, UART control line status test.\n");
    printf("case 7690, UART break control test.\n\n");
}

/**
 * @brief Preparing resource for test application
 *
 * @param info The global information of test.
 * @return 0 for success, -errno for failures.
 */
static int app_prepare(struct uart_test_info *info)
{
    int ret;
    char *buf;

    info->target = malloc(sizeof(*info->target));
    if (!info->target) {
        return -ENOMEM;
    }

    info->pair = malloc(sizeof(*info->pair));
    if  (!info->pair) {
        goto err_free_target_mem;
    }

    info->pattern = malloc(BUFFER_SIZE);
    if (!info->pattern) {
        goto err_free_pattern_mem;
    }

    info->compare = malloc(BUFFER_SIZE);
    if (!info->compare) {
        goto err_free_compare_mem;
    }

    return 0;

err_free_compare_mem:
    free(info->compare);
err_free_pattern_mem:
    free(info->pattern);
err_free_target_mem:
    free(info->target);
    return -ENOMEM;
}

/**
 * @brief Release resource of test application.
 *
 * @param info The global information of test.
 * @return 0 for success, -errno for failures.
 */
static int app_complete(struct uart_test_info *info)
{
    free(info->target);
    free(info->pair);
    free(info->pattern);
    free(info->compare);

    return 0;
}

/**
 * @brief The main function of test
 *
 * @param argc The count of arguments.
 * @param argv The pointer to argument string.
 * @return 0 for success, -errno for failures.
 */
int main(int argc, char *argv[])
{
    int ret = 1, opt;
    struct uart_test_info info;

    /* parse options. */
    info.case_id = 0;
    while (ret) {
        opt = getopt(argc, argv, "c:");

        switch (opt) {
        case 'c':
            info.case_id = atoi(optarg);
            break;
        case -1: /* exit parse options */
            if (info.case_id) {
                /* parameter valided, go the test procedure */
                ret = 0;
                break;
            }
        case '?':
        default:
            print_usage();
            return 0; /* exit to shell */
        }
    }

    /* memory allocate etc. */
    ret = app_prepare(&info);
    if (ret) {
        goto err_app_prepare;
    }

    /* allocate resource for test */
    ret = uart_prepare(&info);
    if (ret) {
        goto err_app_complete;
    }

    /* start test of case id */
    switch (info.case_id) {
    case 7687: /* Line coding test */
        ret = uart_line_coding(&info);
        break;
    case 7688: /* Data tranfer test */
        ret = uart_data_transfer(&info);
        break;
    case 7689: /* Control line status test */
        ret = uart_control_line(&info);
        break;
    case 7690: /* Break control test */
        ret = uart_break_control(&info);
        break;
    default:
        ret = -EINVAL;
        break;
    }

    /* gracefully leave device driver */
    uart_complete(&info);

    /* release system resource */
    app_complete(&info);

    /* print out the test result */
    if (-EINVAL == ret) {
        print_usage();
    } else if(!ret ) {
        print_test_case_result_only(info.case_id, ret);
    } else {
        print_test_case_result(APP_NAME, info.case_id, ret, strerror(-ret));
    }

    return 0;

err_app_complete:
    app_complete(&info);

err_app_prepare:
    print_test_case_result(APP_NAME, info.case_id, ret, strerror(-ret));

    return ret;
}
