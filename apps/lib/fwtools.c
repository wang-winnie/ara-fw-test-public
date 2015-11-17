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
#include <sys/time.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/limits.h>
#include <ctype.h>

#include "./include/libfwtest.h"

/* Define ASCII tokens */
#define carriage_return '\r'
#define new_line        '\n'
#define null_byte       '\0'

/**
 * @brief Get value from debugfs
 *
 * @param class_path Class path string
 * @param attr The class attribute
 * @param value The value is read from debugfs
 * @param len The value buffer size
 * @return 0 on success, error code on failure
 */
int debugfs_get_attr(char *class_path, const char *attr, char *value, int len)
{
    char sysbuf[PATH_MAX];
    int fd = 0, valuelen = 0;
    int nread = 0;

    if (class_path == NULL || *class_path == null_byte || attr == NULL ||
        value == NULL || len < 1) {
        return -EINVAL;
    }

    snprintf(sysbuf, sizeof(sysbuf), "%s/%s", class_path, attr);
    sysbuf[strlen(sysbuf) + 1] = null_byte;

    fd = open(sysbuf, O_RDONLY);
    if (fd < 0) {
        return -ENOENT;
    }

    if ((nread = read(fd, value, len)) < 0) {
        close(fd);
        return -ENOENT;
    }

    value[nread] = null_byte;
    close(fd);
    valuelen = strlen(value);

    while (valuelen > 0 && (value[valuelen - 1] == new_line ||
           value[valuelen -1] == carriage_return)) {
        value[valuelen - 1] = null_byte;
        valuelen = valuelen - 1;
    }

    return 0;
}

/**
 * @brief Set value to debugfs
 *
 * @param class_path Class path string
 * @param attr The class attribute
 * @param value The value is write to debugfs
 * @param len The value buffer size
 * @return 0 on success, error code on failure
 */
int debugfs_set_attr(char *class_path, const char *attr, char *value, int len)
{
    char sysbuf[PATH_MAX];
    int fd = 0;

    if (class_path == NULL || *class_path == null_byte || attr == NULL ||
        value == NULL || len < 1) {
        return -EINVAL;
    }

    snprintf(sysbuf, sizeof(sysbuf), "%s/%s", class_path, attr);
    sysbuf[sizeof(sysbuf) - 1] = null_byte;

    fd = open(sysbuf, O_WRONLY);
    if (fd < 0) {
        return -ENOENT;
    }

    if (write(fd, value, len) < 0) {
        close(fd);
        return -ENOENT;
    }

    close(fd);
    return 0;
}
