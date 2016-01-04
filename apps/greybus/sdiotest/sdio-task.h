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

#ifndef SDIO_TASK_H_FILE
#define SDIO_TASK_H_FILE

#ifndef SDIO_TASK_H_FILE
#define SDIO_TASK_H_FILE

#include <stdint.h>
#include <fcntl.h>

#define APP_NAME "SDIO"

#define OPSUCCESS 0
#define OPERROR -1
#define ENOINPUT 255

#define LENGTH 32
#define MAXLENGTH 128

#define RETRY_COUNT 3
#define DELAY_TIME 5
#define DELAY_UMOUNT_TIME 1

struct gb_sdio_test_info {
    /** SDIO test case id */
    int caseid;
    /** SDIO test block id */
    char *blkname;
    /** SDIO copy file location */
    char *filepath;
    /** SDIO read/write file name */
    char *filename;
    /** SDIO open file flag */
    int flags;
    /** SDIO test run timer */
    time_t timer;
    /** SDIO input data */
    char *inputdata;
    /** SDIO input data length*/
    int datalength;
};

int sdio_dev_precondition(struct gb_sdio_test_info *info);
void sdio_dev_postcondition();
int C108X_sdiowr(struct gb_sdio_test_info *info);
int C1084_sdiocp(struct gb_sdio_test_info *info);
int C1085_sdiotimer(struct gb_sdio_test_info *info);

#endif /* SDIO_TASK_H_FILE */
