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

#ifndef UART_TASK_H_FILE
#define UART_TASK_H_FILE

#include <stdint.h>
#include <fcntl.h>

#define APP_NAME "UART"

#define PATH_LEN    80
#define BUFFER_SIZE 2000

/**
 * @brief The UART device info.
 */
struct uart_dev {
    int fd;
    char name[PATH_LEN];
};

/**
 * @brief The global test info.
 */
struct uart_test_info {
    /** UART test case id */
    int case_id;
    /** UART target driver */
    struct uart_dev *target;
    /** UART pair driver */
    struct uart_dev *pair;
    /** pointer to test pattern */
    char *pattern;
    /** pointer to compare pattern */
    char *compare;
    /** pattern size */
    int size;
    /** test direction, target to pair or part to target */
    int direction;
    /** pattern test result */
    int result;
};

int uart_prepare(struct uart_test_info *info);
int uart_complete(struct uart_test_info *info);

int uart_line_coding(struct uart_test_info *info);
int uart_data_transfer(struct uart_test_info *info);
int uart_control_line(struct uart_test_info *info);
int uart_break_control(struct uart_test_info *info);

#endif /* UART_TASK_H_FILE */
