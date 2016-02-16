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

#include "sdio-task.h"
#include <libfwtest.h>

/**
 * print usage.
 */
void print_usage(void)
{
    printf("\nUsage: sdiotest [-c case_id] [-d input_data] [-t timer] \n"
           "         [<-p file_path>] [<-b block_name>] [<-f input_file>]\n");
    printf("    -c: Testrail test id, support 1080, 1081, 1082, 1083, 1084 "
                    "and 1085.\n");
    printf("    -d: input test data.\n");
    printf("    -t: test timer in seconds.\n");
    printf("    -p: input test file located. Optional, copy the file on this "
                    "partition to SD card. Optional, the default is data.\n");
    printf("    -b: input block name. Optional, the default is mmcblk1.\n");
    printf("    -f: input test file name. Optional, the default value is "
                    "test.txt.\n\n");

    printf("case 1080, small data transfer in existed file. \n"
           "           sdiotest -c 1080 [-d data] [<-b block_name> "
                                       "<-f input_file>]\n");
    printf("case 1081, large data transfer in existed file. \n"
           "           sdiotest -c 1081 [-d data] [<-b block_name> "
                                       "<-f input_file>]\n");
    printf("case 1082, small data transfer by create file. \n"
           "           sdiotest -c 1082 [-d data] [<-b block_name> "
                                           "<-f input_file>]\n");
    printf("case 1083, large data transfer by create file. \n"
           "           sdiotest -c 1083 [-d data] [<-b block_name> "
                                       "<-f input_file>]\n");
    printf("case 1084, copy file from data partition to SD card. \n"
           "           sdiotest -c 1084 [<-f input_file>] [<-p file_path>]"
                                       "[<-b block_name>]\n");
    printf("case 1085, SDIO stream transfer for 1 mins. \n"
           "           sdiotest -c 1085 [-d data] [-t timer] [<-b block_name> "
                                       "<-f input_file>]\n\n");
}

/**
 * main function.
 */
int main(int argc, char *argv[])
{
    int ret = 0;
    struct gb_sdio_test_info sdio_info;
    int options = 0;

    /* init param. */
    memset(&sdio_info, 0x00, sizeof(sdio_info));

    /* parse options. */
    while ((options = getopt(argc, argv, "c:t:f:d:b:p:")) != OPERROR) {
        switch (options) {
        case 'c':
            sdio_info.caseid = atoi(optarg);
            break;
        case 'b':
            sdio_info.blkname = optarg;
            break;
        case 't':
            sdio_info.timer = atoi(optarg);
            break;
        case 'f':
            sdio_info.filename = optarg;
            break;
        case 'p':
            sdio_info.filepath = optarg;
            break;
        case 'd':
            sdio_info.inputdata = optarg;
            sdio_info.datalength = strlen(sdio_info.inputdata);
            break;
        case '?':
        default:
            print_usage();
            return 0;
        }
    }

    if (sdio_info.caseid == 0) {
        print_usage();
        return 0;
    }

    /* check the SD card. */
    if (sdio_dev_precondition(&sdio_info)) {

        print_test_case_result_only(sdio_info.caseid, OPERROR);
        print_usage();
        sdio_dev_postcondition();
        return 0;
    }

    switch (sdio_info.caseid) {
    case 1080:
    case 1081:
        sdio_info.flags = O_RDWR;
        ret = C108X_sdiowr(&sdio_info);
        break;
    case 1082:
    case 1083:
        sdio_info.flags = O_RDWR | O_CREAT;
        ret = C108X_sdiowr(&sdio_info);
        break;
    case 1084:
        sdio_info.flags = O_RDWR;
        ret = C1084_sdiocp(&sdio_info);
        break;
    case 1085:
        sdio_info.flags = O_RDWR | O_CREAT;
        ret = C1085_sdiotimer(&sdio_info);
        break;
    default:
        ret = -ENOINPUT;
        break;
    }

    sdio_dev_postcondition();

    if (-ENOINPUT == ret) {
        print_usage();
    } else if(OPSUCCESS == ret ) {
        print_test_case_result_only(sdio_info.caseid, ret);
    } else {
       print_test_case_result(APP_NAME, sdio_info.caseid, ret, strerror(-ret));
   }

    return 0;
}
