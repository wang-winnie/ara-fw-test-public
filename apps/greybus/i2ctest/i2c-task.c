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
#include <fcntl.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "i2c-task.h"

/**
 * @brief open I2C devices.
 *
 * @param i2cbus HW I2C bus.
 *
 * @return file The new file descriptor, or -1 if an error occurred.
 */
int open_i2c_dev(int i2cbus)
{
    int file;
    char devname[MAXLENGTH];
    size_t size = sizeof(devname);

    snprintf(devname, size, "/dev/i2c-%d", i2cbus);
    file = open(devname, O_RDWR);

    if (file < 0 && (errno == ENOENT || errno == ENOTDIR))
    {
        sprintf(devname, "/dev/i2c/%d", i2cbus);
        file = open(devname, O_RDWR);
    }

    return file;
}

/**
 * @brief set I2C devuce as slave.
 *
 * @param file The file descriptor return from open().
 *
 * @param address The I2C device address.
 *
 * @return 0 for success, -error if fail.
 */
int force_set_slave_addr(int file, int address)
{
    if (ioctl(file, I2C_SLAVE_FORCE, address) < 0) {
        return -errno;
    }

    return 0;
}

/**
 * @brief get I2C function support.
 *
 * @param info The I2C info from user.
 *
 * @return return the result compare with data and get HW function.
 */
int ARA_556_i2cgetfunsupport(struct gb_i2c_info *info)
{
    unsigned long funcs;
    int file;
    int ret = 0;
    char function[MAXLENGTH];
    size_t size = sizeof(function);

    /* check input value. */
    if ((-EINVAL == info->busid) || 0 == strlen(info->functionality) ) {
        return -ENOINPUT;
    }

    file = open_i2c_dev(info->busid);

    if (file < 0) {
        return -1;
    }

    if (ioctl(file, I2C_FUNCS, &funcs) < 0) {
        close(file);
        return -1;
    }

    snprintf(function, size, "%x", funcs);

    ret = strcmp(info->functionality ,function);

    close(file);
    return ret;
}

/**
 * @brief read I2C data.
 *
 * @param info The I2C info from user.
 *
 * @return return the result compare with data and read from HW.
 */
int ARA_556_i2creaddata(struct gb_i2c_info *info)
{
    int file;
    int ret ;
    int size = 1;
    uint8_t buf[2];

    /* check input value. */
    if ((-EINVAL == info->busid) ||
        (-EINVAL == info->devaddress) ||
        (-EINVAL == info->addr) ||
        (-EINVAL == info->buf))
    {
        return -ENOINPUT;
    }

    file = open_i2c_dev(info->busid);
    if (file < 0) {
        return -1;
    }

    if (force_set_slave_addr(file, info->devaddress) < 0) {
        close(file);
        return ret;
    }

    buf[0] = (uint8_t)info->addr;
    write(file, buf,size);

    read(file, buf, size);

    ret = (buf[0] == (uint8_t)info->buf) ? 0 : -1 ;

    close(file);
    return ret;
}
