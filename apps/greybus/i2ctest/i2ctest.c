/**
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
#include <stdlib.h>
#include <getopt.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "i2c-task.h"
#include <libfwtest.h>

#define APP_NAME "I2C"

/**
 * print usage.
 */
void print_usage(void)
{
    printf("\nUsage: i2ctest [-c case_id] [-b bus_id] [-a device_address] "
           "[-i index] [-d data]\n");
    printf("    -c: Testrail test id, support 1001 and 1002.\n");
    printf("    -b: bus number in decimal integer.\n");
    printf("    -a: device address in decimal integer.\n");
    printf("    -i: access the byte address in decimal integer.\n");
    printf("    -d: data need to input in test case.\n");
    printf("        For case 1001, it is support functions.\n");
    printf("        For case 1002, it is the decimal integer that read "
           "from the byte address.\n");

    printf("For case 1001, i2ctest -c 1001 [-b bus_id][-d data] \n");
    printf("For case 1002, i2ctest -c 1002 [-b bus_id] [-a device_address] "
           "[-i index] [-d data] \n\n");
}

/**
 * main function.
 */
int main(int argc, char *argv[])
{
    int ret = 0;
    int caseid = 0;
    struct gb_i2c_info i2c_info;
    char *data = NULL;
    int options = 0;

    /* init param. */
    i2c_info.busid = -EINVAL;
    i2c_info.devaddress = -EINVAL;
    i2c_info.addr = -EINVAL;
    i2c_info.buf = -EINVAL;
    memset(i2c_info.functionality, 0x00, sizeof(i2c_info.functionality));

    /* parse options. */
    while ((options = getopt (argc, argv, "a:b:c:d:i:")) != OPERROR) {
        switch (options)
        {
            case 'a':
                i2c_info.devaddress = atoi(optarg);
                break;
            case 'b':
                i2c_info.busid = atoi(optarg);
                break;
            case 'c':
                caseid = atoi(optarg);
                break;
             case 'd':
                data = optarg;
                break;
             case 'i':
                i2c_info.addr = atoi(optarg);
                break;
             case '?':
             default:
                print_usage();
                return 0;
        }
    }

    switch (caseid)
    {
        case 1001:
            if (data)
                snprintf(i2c_info.functionality, sizeof(i2c_info.functionality), "%s", data);
            ret = ARA_1001_i2cgetfunsupport(&i2c_info);
            break;
        case 1002:
            if (data)
                i2c_info.buf = atoi(data);
            ret = ARA_1002_i2creaddata(&i2c_info);
            break;
        default:
            ret = -ENOINPUT;
            break;
    }

    if (-ENOINPUT == ret) {
        print_usage();
    } else if(OPSUCCESS == ret ) {
        print_test_case_result_only(caseid, ret);
    } else {
       print_test_case_result(APP_NAME, caseid, ret, strerror(errno));
   }

    return 0;
}
