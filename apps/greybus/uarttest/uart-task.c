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
#include <pthread.h>
#include <termios.h>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <libfwtest.h>

#include "uart-task.h"

#define UART_PATH   "/dev/ttyGB"

#define MAX_INDEX   10

#define PARITY_NONE 0
#define PARITY_ODD  1
#define PARITY_EVEN 2

#define STOP_BITS_1 1
#define STOP_BITS_2 2

#define FLOW_OFF    0
#define FLOW_ON     1

#define VMIN_128    128
#define VTIME_10    10

#define PATTERN_SIZE 1800
#define RX_TIMEOUT  2
#define NUM_TEST    10

#define LOG_LEN     64

/**
 * @brief The UART configuration.
 */
struct uart_config {
    uint32_t baud_rate;
    uint8_t data_bits;
    uint8_t stop_bits;
    uint8_t parity;
    uint8_t flow_ctrl;
};

/**
 * @brief The UART test configuration list.
 */
struct uart_config config_list[] = {
    /* generic settings */
    { 115200, 8, STOP_BITS_1, PARITY_NONE, FLOW_OFF},
    {  57600, 8, STOP_BITS_1, PARITY_NONE, FLOW_OFF},
    {  38400, 8, STOP_BITS_1, PARITY_NONE, FLOW_OFF},
    {  19200, 8, STOP_BITS_1, PARITY_NONE, FLOW_OFF},
    {   9600, 8, STOP_BITS_1, PARITY_NONE, FLOW_OFF},
    {   4800, 8, STOP_BITS_1, PARITY_NONE, FLOW_OFF},
    /* data bits test */
    { 115200, 7, STOP_BITS_1, PARITY_NONE, FLOW_OFF},
    {  57600, 7, STOP_BITS_1, PARITY_NONE, FLOW_OFF},
    {  38400, 7, STOP_BITS_1, PARITY_NONE, FLOW_OFF},
    {  19200, 7, STOP_BITS_1, PARITY_NONE, FLOW_OFF},
    {   9600, 7, STOP_BITS_1, PARITY_NONE, FLOW_OFF},
    {   4800, 7, STOP_BITS_1, PARITY_NONE, FLOW_OFF},
    /* stop bits test */
    { 115200, 8, STOP_BITS_2, PARITY_NONE, FLOW_OFF},
    {  57600, 8, STOP_BITS_2, PARITY_NONE, FLOW_OFF},
    {  38400, 8, STOP_BITS_2, PARITY_NONE, FLOW_OFF},
    {  19200, 8, STOP_BITS_2, PARITY_NONE, FLOW_OFF},
    {   9600, 8, STOP_BITS_2, PARITY_NONE, FLOW_OFF},
    {   4800, 8, STOP_BITS_2, PARITY_NONE, FLOW_OFF},
    /* parity odd test */
    { 115200, 8, STOP_BITS_1, PARITY_ODD, FLOW_OFF},
    {  57600, 8, STOP_BITS_1, PARITY_ODD, FLOW_OFF},
    {  38400, 8, STOP_BITS_1, PARITY_ODD, FLOW_OFF},
    {  19200, 8, STOP_BITS_1, PARITY_ODD, FLOW_OFF},
    {   9600, 8, STOP_BITS_1, PARITY_ODD, FLOW_OFF},
    {   4800, 8, STOP_BITS_1, PARITY_ODD, FLOW_OFF},
    /* parity even test */
    { 115200, 8, STOP_BITS_1, PARITY_EVEN, FLOW_OFF},
    {  57600, 8, STOP_BITS_1, PARITY_EVEN, FLOW_OFF},
    {  38400, 8, STOP_BITS_1, PARITY_EVEN, FLOW_OFF},
    {  19200, 8, STOP_BITS_1, PARITY_EVEN, FLOW_OFF},
    {   9600, 8, STOP_BITS_1, PARITY_EVEN, FLOW_OFF},
    {   4800, 8, STOP_BITS_1, PARITY_EVEN, FLOW_OFF},
};

/**
 * @brief The UART normal configuration.
 */
struct uart_config normal_cfg = {
    115200, 8, STOP_BITS_1, PARITY_NONE, FLOW_OFF
};

/**
 * @brief The UART flow control configuration.
 */
struct uart_config flow_cfg = {
    115200, 8, STOP_BITS_1, PARITY_NONE, FLOW_ON
};

/**
 * @brief The UART flow control configuration.
 */
static char short_pattern[] = "0123456789AaBbCcDdEe";

/**
 * @brief Convert raw baudrate value to definition.
 *
 * @param baudrate The raw baudrate value.
 * @return 0 for success, -errno for failures.
 */
static int convert_baudrate(int baudrate)
{
    switch (baudrate) {
    case 115200:
        return B115200;
    case 57600:
        return B57600;
    case 38400:
        return B38400;
    case 19200:
        return B19200;
    case 9600:
        return B9600;
    case 4800:
        return B4800;
    }
    return B115200;
}

/**
 * @brief Get RTS state.
 *
 * @param fd The file description.
 * @param status The RTS value, 0 for de-active, 1 for active.
 * @return 0 for success, -errno for failures.
 */
static int get_rts(int fd, int *status)
{
    int ret, sercmd, serstat;

    ret = ioctl(fd, TIOCMGET, &serstat);
    if (ret < 0) {
        return -EIO;
    }

    if (serstat & TIOCM_RTS) {
        *status = 1;
    } else {
        *status = 0;
    }

    return 0;
}

/**
 * @brief Set RTS state.
 *
 * @param fd The file description.
 * @param active The RTS value, 0 for de-active, 1 for active.
 * @return 0 for success, -errno for failures.
 */
static int set_rts(int fd, int active)
{
    int ret, sercmd, status;

    sercmd = TIOCM_RTS;
    if (active) {
        ret = ioctl(fd, TIOCMBIS, &sercmd); /* set rts pin */
    } else {
        ret = ioctl(fd, TIOCMBIC, &sercmd); /* reset rts pin */
    }
    if (ret < 0) {
        return -EIO;
    }

    ret = get_rts(fd, &status);
    if (ret < 0) {
        return -EIO;
    }
    if (status != active) {
        return -EIO;
    }

    return 0;
}

/**
 * @brief Configure uart port settings
 *
 * @param fd The file description.
 * @param cfg The configuration values.
 * @return 0 for success, -errno for failures.
 */
static int port_config(int fd, struct uart_config *cfg)
{
    int ret;
    struct termios tio;

    if (fd < 0) {
        return -ENODEV;
    }

    memset(&tio, 0, sizeof(tio));

    /* termios settings */
    tio.c_cflag = convert_baudrate(cfg->baud_rate);

    /* Character size mask */
    switch (cfg->data_bits) {
    case 8:
        tio.c_cflag |= CS8;
        break;
    case 7:
        tio.c_cflag |= CS7;
        break;
    default:
        return -EINVAL;
    }

    /* stop bits */
    switch (cfg->stop_bits) {
    case STOP_BITS_1:
        tio.c_cflag &= ~CSTOPB; /* set one stop bits */
        break;
    case STOP_BITS_2:
        tio.c_cflag |= CSTOPB;  /* set two stop bits */
        break;
    default:
        return -EINVAL;
    }

    /* parity, odd or even */
    switch (cfg->parity) {
    case PARITY_NONE:
        tio.c_cflag &= ~PARENB; /* disable parity checking */
        break;
    case PARITY_ODD:
        tio.c_cflag &= ~PARENB; /* disable parity checking */
        tio.c_cflag |= PARODD;  /* odd parity */
        break;
    case PARITY_EVEN:
        tio.c_cflag |= PARENB;  /* enable parity checking */
        tio.c_cflag &= ~PARODD; /* even parity */
        break;
    default:
        return -EINVAL;
    }

    /* CLOCAL ignore modem control lines */
    tio.c_cflag |= CLOCAL;
    /* CREAD enable receiver */
    tio.c_cflag |= CREAD;

    /* flow control */
    if (cfg->flow_ctrl) {
        tio.c_cflag |= CRTSCTS;  /* control mode flags */
    }

    tio.c_iflag = 0;    /* input mode flags */
    tio.c_oflag = 0;    /* output mode flags */
    tio.c_lflag = 0;    /* local mode flags */

    /* minimum number of characters for noncanonical read */
    tio.c_cc[VMIN] = VMIN_128;

    /* timeout in deciseconds for noncanonical read */
    tio.c_cc[VTIME] = VTIME_10; /* 1 sec */

    /* activate the settings for the port */
    ret = tcflush(fd, TCIFLUSH);
    if (ret < 0) {
        return -EIO;
    }
    ret = tcsetattr(fd, TCSANOW, &tio);
    if (ret < 0) {
        return -EIO;
    }

    return 0;
}

/**
 * @brief Write data to uart port
 *
 * @param fd The file description.
 * @param data The pointer to output data.
 * @param size The pointer to output size and real output size.
 * @return 0 for success, -errno for failures.
 */
static int port_write(int fd, char *data, int *size)
{
    int ret, send;
    char *buf;

    if (fd < 0) {
        return -ENODEV;
    }

    buf = data;
    send = *size;
    *size = 0;
    do {
        ret = write(fd, buf, send);
        if (send == ret) { /* complete */
            *size += ret;
            break;
        } else if (ret < 0) { /* got error */
            return -EIO;
        } else {
            buf += ret;
            send -= ret;
            *size += ret;
        }
    } while (1);

    return 0;
}

/**
 * @brief Read data from uart port
 *
 * @param fd The file description.
 * @param data The pointer to input buffer.
 * @param size The pointer to input buffer size and real input size.
 * @param timeout Timeout for read.
 * @return 0 for success, -errno for failures.
 */
static int port_read(int fd, char *data, int *size, int timeout)
{
    int ret, get, rv;
    char *buf;
    fd_set set;
    struct timeval to;

    if (fd < 0) {
        return -ENODEV;
    }

    FD_ZERO(&set);
    FD_SET(fd, &set);

    to.tv_sec = timeout;
    to.tv_usec = 0;

    buf = data;
    get = *size;
    *size = 0;
    do {
        rv = select(fd + 1, &set, NULL, NULL, &to);
        if (rv == -1) { /* error */
            return -EIO;
        } else if (!rv) { /* timeout */
            return 0;
        } else { /* data received */
            ret = read(fd, buf, get);
            if (ret < 0) { /* read error */
                return -EIO;
            } else { /* read data */
                buf += ret;
                get -= ret;
                *size += ret;
            }

            if (!get) {
                return 0;
            }
        }
    } while (1);

    return 0;
}

/**
 * @brief The thread for read and write in parallel
 *
 * @param object The thread input value.
 * @return 0 for success, -errno for failures.
 */
static void *read_thread(void* object)
{
    int ret, size;
    struct uart_test_info *info = object;
    int fd;

    size = BUFFER_SIZE;

    if (!info->direction) {
        fd = info->pair->fd;
    } else {
        fd = info->target->fd;
    }

    ret = port_read(fd, info->compare, &size, 2);
    if (ret < 0) {
        info->result = -EIO;
    } else if (size != info->size) {
        info->result = -EIO;
    } else if (memcmp(info->pattern, info->compare, info->size)) {
        info->result = -EIO;
    } else {
        info->result = 0;
    }

    return 0;
}

/**
 * @brief Prepare test code for accessing driver
 *
 * @param info The test global infomation.
 * @param direction The data transfer direction.
 * @return 0 for success, -errno for failures.
 */
static int mass_transfer(struct uart_test_info *info, int direction)
{
    int ret, i, sum, check, size;
    int *buf;
    int fd;
    pthread_t tid;

    if (!direction) { /* output test */
        info->direction = 0;
        fd = info->target->fd;
    } else { /* input test */
        info->direction = 1;
        fd = info->pair->fd;
    }

    /* generate pattern */
    buf = (int*)info->pattern;
    for (i = 0, sum = 0; i < (int)(PATTERN_SIZE / sizeof(int)); i++) {
        *buf = random();
        sum += *buf++;
    }

    /* generate another pattern to valifying pattern is valid */
    buf = (int*)info->pattern;
    for (i = 0, check = 0; i < (int)(PATTERN_SIZE / sizeof(int)); i++) {
        *buf = random();
        check += *buf++;
    }
    if (sum == check) {
        return -1;
    }

    ret = pthread_create(&tid, NULL, &read_thread, (void*)info);
    if (ret < 0) {
        return -EPERM;
    }

    size = PATTERN_SIZE;
    ret = port_write(fd, info->pattern, &size);
    if (ret < 0) {
        return -EIO;
    }

    pthread_join(tid, NULL); /* wait for pthread stop */

    if (info->result < 0) {
        return -EIO;
    }

    return 0;
}

/**
 * @brief Prepare test code for accessing driver
 *
 * @param info The test global infomation.
 * @return 0 for success, -errno for failures.
 */
int uart_prepare(struct uart_test_info *info)
{
    int i = 0, j;
    char log_buf[LOG_LEN];

    for (i = 0; i < MAX_INDEX; i++) {
        snprintf(info->target->name, PATH_LEN, "%s%d", UART_PATH, i);
        info->target->fd = open(info->target->name, O_RDWR | O_NOCTTY);
        if (info->target->fd >= 0) {
            i++;
            snprintf(log_buf, LOG_LEN, "Found %s", info->target->name);
            print_test_case_log(APP_NAME, info->case_id, log_buf);
            break;
        }
    }
    if (i == MAX_INDEX) {
        snprintf(log_buf, LOG_LEN, "Can't find any port:%s", info->pair->name);
        print_test_case_log(APP_NAME, info->case_id, log_buf);
        return -ENODEV;
    }

    for (; i < MAX_INDEX; i++) {
        snprintf(info->pair->name, PATH_LEN, "%s%d", UART_PATH, i);
        info->pair->fd = open(info->pair->name, O_RDWR | O_NOCTTY);
        if (info->pair->fd >= 0) {
            snprintf(log_buf, LOG_LEN, "Found %s", info->pair->name);
            print_test_case_log(APP_NAME, info->case_id, log_buf);
            break;
        }
    }
    if (i == MAX_INDEX) {
        snprintf(log_buf, LOG_LEN, "Only one port be found:%s",
                 info->pair->name);
        print_test_case_log(APP_NAME, info->case_id, log_buf);
        goto err_close_target_dev;
    }

    return 0;

err_close_target_dev:
    close(info->target->fd);

    return -ENODEV;
}

/**
 * @brief Leave the driver usage
 *
 * @param info The test global infomation.
 * @return 0 for success, -errno for failures.
 */
int uart_complete(struct uart_test_info *info)
{
    close(info->target->fd);
    close(info->pair->fd);

    return 0;
}

/**
 * @brief Line coding test
 *
 * This test set variety of the combinatin of the UART configuration, such
 * baudrate, databits, parity and stopbit to UART module. Send data to the
 * other module and verify the data can be correctly received.
 *
 * @param info The test global infomation.
 * @return 0 for success, -errno for failures.
 */
int uart_line_coding(struct uart_test_info *info)
{
    int ret, i, size;
    char *tx_data, *rx_data;

    tx_data = info->pattern;
    rx_data = info->compare;

    for (i = 0; i < (int)(sizeof(config_list)/sizeof(config_list[0])); i++) {
        port_config(info->target->fd, &config_list[i]);
        port_config(info->pair->fd, &config_list[i]);

        sprintf(tx_data, "%d, %d, %d, %d, %d, %d",
                config_list[i].baud_rate,
                config_list[i].data_bits,
                config_list[i].stop_bits,
                config_list[i].parity,
                config_list[i].flow_ctrl,
                rand());

        size = strlen(tx_data) + 1;
        ret = port_write(info->target->fd, tx_data, &size);
        if (ret < 0) {
            return -EIO;
        }

        size = BUFFER_SIZE;
        port_read(info->pair->fd, rx_data, &size, RX_TIMEOUT);
        if (size) {
            if (strcmp(tx_data, rx_data)) {
                return -EIO;
            }
        } else {
            return -EIO;
        }
    }

    return 0;
}

/**
 * @brief Data transfer test
 *
 * This test send data from one module to the other module and verify whether
 * the data can be correctly send and received. With small data and large data
 * transfer, slow speed and fast transfer.
 *
 * @param info The test global infomation.
 * @return 0 for success, -errno for failures.
 */
int uart_data_transfer(struct uart_test_info *info)
{
    int ret, i;

    port_config(info->target->fd, &normal_cfg);
    port_config(info->pair->fd, &normal_cfg);

    tcflush(info->target->fd, TCIOFLUSH);
    tcflush(info->pair->fd, TCIOFLUSH);

    info->direction = 0;
    info->size = PATTERN_SIZE;

    for (i = 0; i < NUM_TEST; i++) {
        ret = mass_transfer(info, i & 0x01);
        if (info->result || ret) {
            return -EIO;
        }
    }

    return 0;
}

/**
 * @brief Control line test
 *
 * This test is concentrating on RTS flow control. One UART module send the to
 * the other module, and the transfer should pause when the other moulde
 * generate the RTS signal. After the RTS signal disabled, the tranfer is able
 * to continue the data transfer.
 *
 * @param info The test global infomation.
 * @return 0 for success, -errno for failures.
 */
int uart_control_line(struct uart_test_info *info)
{
    int ret, size, status;
    char *tx_data, *rx_data;
    int tx_fd, rx_fd;

    tx_data = short_pattern;
    rx_data = info->compare;

    tx_fd = info->target->fd;
    rx_fd = info->pair->fd;

    port_config(tx_fd, &flow_cfg);
    port_config(rx_fd, &flow_cfg);

    tcflush(tx_fd, TCIOFLUSH);
    tcflush(rx_fd, TCIOFLUSH);

    ret = set_rts(rx_fd, 1);

    /* normal case test */
    size = strlen(tx_data) + 1;
    ret = port_write(tx_fd, tx_data, &size);
    if (ret < 0) {
        return -EIO;
    }

    size = BUFFER_SIZE;
    port_read(rx_fd, rx_data, &size, 2);
    if (size) {
        if (strcmp(tx_data, rx_data)) {
            return -EIO;
        }
    } else {
        return -EIO;
    }
    tcflush(tx_fd, TCIOFLUSH);
    tcflush(rx_fd, TCIOFLUSH);

    /* de-active RTS*/
    ret = set_rts(rx_fd, 0);
    if (ret < 0) {
        return -EIO;
    }
    sleep(1);

    size = strlen(tx_data) + 1;
    ret = port_write(tx_fd, tx_data, &size);
    if (ret < 0) {
        /* active RTS for recovery */
        ret = set_rts(rx_fd, 1);
        return 0;
    }

    size = BUFFER_SIZE;
    port_read(rx_fd, rx_data, &size, 2);
    if (size >= (int)strlen(tx_data)) {
        return -1;
    }

    tcflush(tx_fd, TCIOFLUSH);
    tcflush(rx_fd, TCIOFLUSH);

    return 0;
}

/**
 * @brief Break set test
 *
 * This test is verifying the set break funciton and data status with the
 * break status from peer module. One module set the break and the other
 * module should receive the status.
 *
 * @param info The test global infomation.
 * @return 0 for success, -errno for failures.
 */
int uart_break_control(struct uart_test_info *info)
{
    int ret, size, status;
    char *tx_data, *rx_data;
    int tx_fd, rx_fd;

    tx_data = short_pattern;
    rx_data = info->compare;

    tx_fd = info->target->fd;
    rx_fd = info->pair->fd;

    port_config(tx_fd, &normal_cfg);
    port_config(rx_fd, &normal_cfg);

    tcflush(tx_fd, TCIOFLUSH);
    tcflush(rx_fd, TCIOFLUSH);

    ret = tcsendbreak(tx_fd, 0);
    if (ret < 0) {
        return -EIO;
    }

    rx_data[0] = 'a'; /* set a default value */

    size = BUFFER_SIZE;
    port_read(rx_fd, rx_data, &size, 2);

    /* for a break, the received data is a 0x00 character */
    if (size != 1) {
        return -EIO;
    }
    if (rx_data[0] != 0x00) {
        return -EIO;
    }

    return 0;
}
