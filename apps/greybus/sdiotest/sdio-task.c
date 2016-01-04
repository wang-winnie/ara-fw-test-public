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
#include <time.h>

#include "sdio-task.h"

#define SDIO_FILE_PATH          "/system/sdiotest"
#define SDIO_FILE_MOD           0666
#define SDIO_BLOCK_NAME         "mmcblk1"
#define SDIO_DEFAULT_FILE       "test.txt"
#define SDIO_DEFAULT_PARTITION  "data"

/**
 * @brief Mount the SD card.
 *
 * @param info The gb_sdio_test_info info.
 *
 * @return 0 on success, error code on failure.
 */
int sdio_dev_precondition(struct gb_sdio_test_info *info)
{
    int ret;
    char cmd[MAXLENGTH];
    char debugstr[MAXLENGTH];

    if (info->blkname) {
        snprintf(cmd, sizeof(cmd), "/dev/block/%s", info->blkname);
    } else {
        snprintf(debugstr, sizeof(debugstr), "The sdio block is not set, use "
                           "default value %s ", SDIO_BLOCK_NAME);
        print_test_case_log(APP_NAME, info->caseid, debugstr);
        snprintf(cmd, sizeof(cmd), "/dev/block/%s", SDIO_BLOCK_NAME);
    }

    mkdir(SDIO_FILE_PATH, SDIO_FILE_MOD);

    ret = mount(cmd, SDIO_FILE_PATH, "vfat", 0, NULL);

    if (ret) {
        snprintf(debugstr, sizeof(debugstr), "mount SD card fail, result is %d"
                                           , ret);
        print_test_case_log(APP_NAME, info->caseid, debugstr);
    }

    return ret;
}

/**
 * @brief Unmount the SD card.
 */
void sdio_dev_postcondition()
{
    umount(SDIO_FILE_PATH);
    sleep(DELAY_UMOUNT_TIME);
    rmdir(SDIO_FILE_PATH);
}

/**
 * @brief write and read the data to SD card.
 *
 * @param info The gb_sdio_test_info info.
 *
 * @return 0 on success, -error code on failure.
 */
int C108X_sdiowr(struct gb_sdio_test_info *info)
{
    int fd = -1;
    int node = 0;
    int ret = OPSUCCESS;
    char *buf = (char *) zalloc((info->datalength) * sizeof(char));
    char file[LENGTH];
    char debugstr[MAXLENGTH];

    if (info->datalength == 0) {
        print_test_case_log(APP_NAME, info->caseid, "The is no input data ");
        ret = -ENOINPUT;
        goto exit;
    } else {
        snprintf(debugstr, sizeof(debugstr), "transfer %d ", info->datalength);
        print_test_case_log(APP_NAME, info->caseid, debugstr);
    }

    if (info->filename) {
        snprintf(file, sizeof(file), "%s/%s", SDIO_FILE_PATH, info->filename);
    } else {
        snprintf(debugstr, sizeof(debugstr), "The sdio file name is not set, "
                           "use default value %s ", SDIO_DEFAULT_FILE);
        print_test_case_log(APP_NAME, info->caseid, debugstr);
        snprintf(file, sizeof(file), "%s/%s", SDIO_FILE_PATH,
                                              SDIO_DEFAULT_FILE);
    }

    fd = open(file, info->flags);
    if (fd < 0) {
        snprintf(debugstr, sizeof(debugstr), "open fail, fd = %d", fd);
        print_test_case_log(APP_NAME, info->caseid, debugstr);
        ret = -ENOENT;
        goto exit;
    }

    node = write(fd, info->inputdata, info->datalength);
    close(fd);
    fd = 0;

    if (node < 0) {
        snprintf(debugstr, sizeof(debugstr), "write fail, node = %d", node);
        print_test_case_log(debugstr);
        ret = -ENOENT;
        goto exit;
    }

    fd = open(file, O_RDWR);
    if (fd < 0) {
        snprintf(debugstr, sizeof(debugstr), "open fails, fd = %d", fd);
        print_test_case_log(APP_NAME, info->caseid, debugstr);
        ret = -ENOENT;
        goto exit;
    }

    node = read(fd, buf, node);
    close(fd);

    if (node < 0) {
        snprintf(debugstr, sizeof(debugstr), "read fail, node = %d", node);
        print_test_case_log(APP_NAME, info->caseid, debugstr);
        ret = -ENOENT;
        goto exit;
    }

    if (strcmp(buf, info->inputdata)) {
        ret = -EPERM;
    }

exit:
    if (buf != NULL) {
        free(buf);
    }
    return ret;
}

/**
 * @brief copy file from data partition to SD card.
 *
 * @param info The gb_sdio_test_info info.
 *
 * @return 0 on success, -error code on failure.
 */
int C1084_sdiocp(struct gb_sdio_test_info *info)
{
    int fd = -1;
    int count = 0;
    char cmd[MAXLENGTH];
    char file[LENGTH];
    char debugstr[MAXLENGTH];

    if (info->filename == NULL) {
        snprintf(debugstr, sizeof(debugstr), "The sdio file name is not set, "
                           "use default value %s ", SDIO_DEFAULT_FILE);
        print_test_case_log(APP_NAME, info->caseid, debugstr);
        info->filename = SDIO_DEFAULT_FILE;
    }

    if (info->filepath == NULL) {
        snprintf(debugstr, sizeof(debugstr), "The sdio file name is not set, "
                           "use default value %s ", SDIO_DEFAULT_PARTITION);
        print_test_case_log(APP_NAME, info->caseid, debugstr);
        info->filepath = SDIO_DEFAULT_PARTITION;
    }

    snprintf(cmd, sizeof(cmd), "cp /%s/%s %s", info->filepath, info->filename,
                                               SDIO_FILE_PATH);
    system(cmd);

    snprintf(file, sizeof(file), "%s/%s", SDIO_FILE_PATH, info->filename);

    while (fd < 0 && count < RETRY_COUNT) {
        sleep(DELAY_TIME);
        fd = open(file, O_RDWR);
        count ++;
    }

    if (fd < 0) {
        print_test_case_log(APP_NAME, info->caseid, "open file fail");
        return -EPERM;
    } else {
        close(fd);
        return OPSUCCESS;
    }
}

/**
 * @brief check the SDIO is work during the timer.
 *
 * @param info The gb_sdio_test_info info.
 *
 * @return 0 on success, -error code on failure.
 */
int C1085_sdiotimer(struct gb_sdio_test_info *info)
{
    time_t t1;
    time_t t2;
    int fd = -1;
    int node = 0;
    char file[LENGTH];
    char debugstr[MAXLENGTH];

    if (info->filename) {
        snprintf(file, sizeof(file), "%s/%s", SDIO_FILE_PATH, info->filename);
    } else {
        snprintf(debugstr, sizeof(debugstr), "The sdio file name is not set, "
                           "use default value %s ", SDIO_DEFAULT_FILE);
        print_test_case_log(APP_NAME, info->caseid, debugstr);
        snprintf(file, sizeof(file), "%s/%s", SDIO_FILE_PATH,
                                              SDIO_DEFAULT_FILE);
    }

    t1 = time(NULL);
    t2 = t1;

    while ((info->timer) > (t2 - t1)) {
        fd = open(file, info->flags);
        if (fd < 0) {
            snprintf(debugstr, sizeof(debugstr), "open fail from %d to %d sec,"
                                                 "fd = %d", t1, t2, fd);
            print_test_case_log(APP_NAME, info->caseid, debugstr);
            return -ENOENT;
        }

        node = write(fd, info->inputdata, info->datalength);
        close(fd);
        fd = 0;

        if (node < 0) {
            snprintf(debugstr, sizeof(debugstr), "write fail from %d to %d sec,"
                                                 "node = %d", t1, t2, node);
            print_test_case_log(APP_NAME, info->caseid, debugstr);
            return -ENOENT;
        }

        t2 = time(NULL);
    }

    snprintf(debugstr, sizeof(debugstr), "test time from %d to %d sec", t1, t2);
    print_test_case_log(APP_NAME, info->caseid, debugstr);
    return OPSUCCESS;
}
