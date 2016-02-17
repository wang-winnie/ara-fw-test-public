/*
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
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <libfwtest.h>
#include "hid-task.h"

/**
 * @brief Print usage of this HID test application
 *
 * @return None
 */
static void print_usage(void)
{
    printf("\nUsage: hidtest [-c case_id]\n");
    printf("    -c: Testrail test id, support 5853, 5854, 5855, and 5856.\n");
    printf("    -h: print usage.\n");
    printf("Example: Run test case 585x\n");
    printf("    ./hidtest -c 5853 -v 18d1 -p 1234\n");
    printf("    ./hidtest -c 5854 -v 18d1 -p 1234 \"5 1 9 6 a1 1 5 ...\"\n");
    printf("    ./hidtest -c 5855 -v 18d1 -p 1234\n");
    printf("    ./hidtest -c 5856 -v 18d1 -p 1234\n");
    printf("Note that C5854 needs an extra parameter: expected descriptor.\n");
}

/**
 * @brief Hex text to ASCII array converter
 *
 * @param array Destination char array
 * @param str Source string with Hex text values
 * @param max_len Length of array
 * @return On success, the total number of Hex converted is returned
 *         On failure, a negative number is returned
 */
static int strtoarray(char *array, char *str, int max_len)
{
    char *pch;
    int count = 0;

    pch = strtok(str, " ");
    while (pch != NULL) {
        if (count < max_len) {
            sscanf(pch, "%x", (unsigned int*)&array[count++]);
        } else {
            return -EINVAL;
        }
        pch = strtok(NULL, " ");
    }

    return count;
}

int main(int argc, char **argv)
{
    int fd;
    struct hid_test_info *info;
    int options = 0;
    int ret = 0;

    info = calloc(1, sizeof(*info));

    /* Parse arguments */
    while ((options = getopt(argc, argv, "c:p:v:h")) != OPERROR) {
        switch (options) {
        case 'c':
            info->case_id = (uint16_t)atoi(optarg);
            break;
        case 'p':
            sscanf(optarg, "%x", (unsigned int*)&info->pid);
            break;
        case 'v':
            sscanf(optarg, "%x", (unsigned int*)&info->vid);
            break;
        case 'h':
        default:
            print_usage();
            goto free_info;
        }
    }

    switch (info->case_id) {
    case 5853:
        ret = ARA_5853_get_hid_descriptor(info);
        break;
    case 5854:
        /* parse extra parameter: expected descriptor for C5854 */
        if (argc >= optind + 1) {
            info->desc_len = strtoarray(info->desc, argv[optind++],
                                        sizeof(info->desc));
            if (info->desc_len > 0)
                ret = ARA_5854_get_hid_report_descriptor(info);
            else
                ret = -EINVAL;
        } else {
            printf("Not enough parameters for C%d\n", info->case_id);
            ret = -ENOINPUT;
        }
        break;
    case 5855:
        ret = ARA_5855_test_feature(info);
        break;
    case 5856:
        ret = ARA_5856_test_report(info);
        break;
    default:
        ret = -ENOINPUT;
        break;
    }

    if (ret == -ENOINPUT) {
        print_usage();
    } else if (ret == OPSUCCESS) {
        print_test_case_result_only(info->case_id, ret);
    } else {
        print_test_case_result(LOG_TAG, info->case_id, ret, strerror(-ret));
    }

free_info:
    free(info);

    return 0;
}

