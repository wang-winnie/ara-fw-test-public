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
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include <libfwtest.h>

/* Log tag for print project name */
#define LOG_TAG "ARA"
/* Define Error for ioctl */
#define ERROR (-1)
/* Max bits per word */
#define MAX_BPW 32
/* SPI string buffer size */
#define SPI_STR_BUF 512

struct spi_app_info {
    /* file descriptor */
    int         fd;
    /* case id */
    uint16_t    case_id;
    /* SPI mode */
    uint8_t     mode;
    /* SPI bits per word */
    uint8_t     bpw1;
    /* SPI bits per word */
    uint8_t     bpw2;
    /* SPI bits per word */
    uint8_t     bpw3;
    /* SPI speed */
    uint32_t    speed;
    /* SPI delay */
    uint16_t    delay;
    /* SPI transfer count */
    uint16_t    ntransfers;
    /* SPI toggle cs change */
    uint8_t     toggle_cs;
    /* xfrs data flag */
    uint8_t     data_flag;
};

/* default open device */
static const char *device = "/dev/spidev32766.0";
/* SPI transfer */
static struct spi_ioc_transfer *spi_xfrs = NULL;

/**
 * @brief check step ret value and confirm step status
 *
 * @param case_id The SPI test case number
 * @param ret return value
 * @return None
 */
void check_step_result(int case_id, int ret)
{
    if (ret) {
        print_test_case_log(LOG_TAG, case_id, strerror(-ret));
    }
}

/**
 * @brief check ret value, then print test result
 *
 * @param case_id The SPI test case number
 * @param ret return value
 * @return None
 */
void print_test_result(int case_id, int ret)
{
    if (ret) {
        print_test_case_result(LOG_TAG, case_id, ret, strerror(-ret));
    } else {
        print_test_case_result(LOG_TAG, case_id, ret, NULL);
    }
}

/**
 * @brief Print usage of this SPI test application
 *
 * @param prog The spitest program name
 * @return None
 */
static void print_usage(const char *prog)
{
    printf("\nUsage: %s [-cDsdbnHOCL3lNR] [data,..]\n", prog);
    printf("    -c: Test Rail test case ID\n");
    printf("    -D device to use (default /dev/spidev32766.0)\n");
    printf("    -s max speed (Hz)\n");
    printf("    -d delay (usec)\n");
    printf("    -1 bits per word\n");
    printf("    -H clock phase(CPHA)\n");
    printf("    -O clock polarity(CPOL)\n");
    printf("    -C chip select active high(CS-HIGH)\n");
    printf("    -L least significant bit first(LSB-FIRST)\n");
    printf("    -w SI/SO signals shared(3WIRE)\n");
    printf("    -l loopback(loop)\n");
    printf("    -N one dev/bus no chip select(NO-CS)\n");
    printf("    -R slave pulls low to pause(READY)\n");
    printf("\nExample : case AC1051 \n");
    printf("     \n");
    printf("     %s -c 1051\n", prog);
 }

/**
 * @brief Set SPI default parameters
 *
 * This is function to set GPIO parameters using in this application to default
 * value
 *
 * @param info The SPI info from user
 * @return None
 */
static void default_params(struct spi_app_info *info)
{
    info->fd = -1;
    info->case_id = 0;
    info->mode = 0;
    info->bpw1 = 8;
    info->bpw2 = 0;
    info->bpw3 = 0;
    info->speed = 500000;
    info->delay = 0;
    info->ntransfers = 0;
    info->toggle_cs = 0;
    info->data_flag = 0;
}

/**
 * @brief Command parser
 *
 * Parse the command input from console
 *
 * @param info The SPI info from user
 * @param argc Indicate how many input parameters
 * @param argv Commands input from console
 * @return 0 on success, negative errno on error
 */
static int command_parse(struct spi_app_info *info, int argc, char **argv)
{
    int option;

    while ((option = getopt(argc, argv, "c:D:s:d:1:2:3:HOCLwlNR")) != -1) {
        switch(option) {
            case 'c':
                info->case_id = atoi(optarg);
                break;
            case 'D':
                device = optarg;
                break;
            case 's':
                info->speed = strtol(optarg, NULL, 0);
                break;
            case 'd':
                info->delay = strtol(optarg, NULL, 0);
                break;
            case '1':
                info->bpw1 = strtol(optarg, NULL, 0);
                break;
            case '2':
                info->bpw2 = strtol(optarg, NULL, 0);
                break;
            case '3':
                info->bpw3 = strtol(optarg, NULL, 0);
                break;
            case 'H':
                info->mode |= SPI_CPHA;
                break;
            case 'O':
                info->mode |= SPI_CPOL;
                break;
            case 'C':
                info->mode |= SPI_CS_HIGH;
                break;
            case 'L':
                info->mode |= SPI_LSB_FIRST;
                break;
            case 'w':
                info->mode |= SPI_3WIRE;
                break;
            case 'l':
                info->mode |= SPI_LOOP;
                break;
            case 'N':
                info->mode |= SPI_NO_CS;
                break;
            case 'R':
                info->mode |= SPI_READY;
                break;
            default:
                print_usage(argv[0]);
                return -EINVAL;
        }
    }

    return 0;
}

/**
 * @brief SPI transfer
 *
 * Add the data to transfer buffer
 *
 * @param info The SPI info from user
 * @param data data buffer
 * @param len The length of data
 * @return 0 on success, negative errno on error
 */
static int add_transfer(struct spi_app_info *info, char *data, int len)
{
    int ret = 0;
    uint8_t *txbuf, *rxbuf;
    struct spi_ioc_transfer *xfr;

    txbuf = malloc(len);
    rxbuf = malloc(len);

    if (txbuf == NULL || rxbuf == NULL)
        return -ENOMEM;

    memcpy(txbuf, data, len);
    memset(rxbuf, 0xff, len);

    info->ntransfers += 1;
    spi_xfrs = realloc(spi_xfrs, sizeof(*spi_xfrs) * (info->ntransfers));
    xfr = &spi_xfrs[(info->ntransfers) - 1];

    xfr->tx_buf = (unsigned long)txbuf;
    xfr->rx_buf = (unsigned long)rxbuf;
    xfr->len = len;
    xfr->speed_hz = info->speed;
    xfr->delay_usecs = info->delay;
    xfr->bits_per_word = info->bpw1;
    xfr->cs_change =  info->toggle_cs;
    xfr->pad = 0;

    return ret;
}

/**
 * @brief paser transfer data
 *
 * Parse the data input from console
 *
 * @param info The SPI info from user
 * @param arg The spitest main arguments data
 * @return 0 on success, negative errno on error
 */
static int parse_transfer_data(struct spi_app_info *info, char *arg)
{
    int ret = 0, len = 0;
    char *value;
    char buf[SPI_STR_BUF];

    for (value = strtok(arg, ","); value; value = strtok(NULL, ",")) {
        buf[len++] = strtol(value, NULL, 0);
    }

    if (len > 0) {
        info->data_flag = 1;
        ret = add_transfer(info, buf, len);
    }

    return ret;
}

/**
 * @brief dump SPI xfrs TX and RX biffer data
 *
 * @param info The SPI info from user
 * @return None
 */
static void dump_xfrs(struct spi_app_info *info)
{
    int i, j;
    struct spi_ioc_transfer *xfr;
    char spistr[SPI_STR_BUF];

    for (i = 0; i < info->ntransfers; ++i) {
        xfr = &spi_xfrs[i];
        snprintf(spistr, sizeof(spistr), "TX-buf:");
        for (j = 0; j < xfr->len; ++j) {
            snprintf(spistr, sizeof(spistr), "%s %02X", spistr,
                    ((unsigned char *)(intptr_t)xfr->tx_buf)[j]);
        }
        print_test_case_log(LOG_TAG, info->case_id, spistr);
        snprintf(spistr, sizeof(spistr), "RX-buf:");
        for (j = 0; j < xfr->len; ++j) {
            snprintf(spistr, sizeof(spistr), "%s %02X", spistr,
                    ((unsigned char *)(intptr_t)xfr->rx_buf)[j]);
        }
        print_test_case_log(LOG_TAG, info->case_id, spistr);
    }
}

/**
 * @brief Test Rail test case C1051
 *
 * C1051: SPI Protocol Mode Response contains the supported modes in the SPI
 * master.
 * This test case verifies that the SPI Protocol Mode Response payload contains
 * a two one-byte values representing the modes that the SPI Masters supports.
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_1051_get_mode(struct spi_app_info *info)
{
    int ret = 0, mode = 0;

    /* open device */
    info->fd = open(device, O_RDWR);
    if (info->fd < 0) {
        ret = -ENODEV;
    }

    if (!ret) {
        /* Send different spi mode to get support mode */
        info->mode = 0;
        /* CPHA mode */
        mode = SPI_CPHA;
        ret = ioctl(info->fd, SPI_IOC_WR_MODE, &mode);
        if (ret != ERROR) {
            print_test_case_log(LOG_TAG, info->case_id,
                               "SPI support mode: SPI_CPHA");
            info->mode |= SPI_CPHA;
        }

        /* CPOL */
        mode = SPI_CPOL;
        ret = ioctl(info->fd, SPI_IOC_WR_MODE, &mode);
        if (ret != ERROR) {
            print_test_case_log(LOG_TAG, info->case_id,
                               "SPI support mode: SPI_CPOL");
            info->mode |= SPI_CPOL;
        }

        /* SPI_CS_HIGH */
        mode = SPI_CS_HIGH;
        ret = ioctl(info->fd, SPI_IOC_WR_MODE, &mode);
        if (ret != ERROR) {
            print_test_case_log(LOG_TAG, info->case_id,
                               "SPI support mode: SPI_CS_HIGH");
            info->mode |= SPI_CS_HIGH;
        }

        /* SPI_LSB_FIRST */
        mode = SPI_LSB_FIRST;
        ret = ioctl(info->fd, SPI_IOC_WR_MODE, &mode);
        if (ret != ERROR) {
            print_test_case_log(LOG_TAG, info->case_id,
                               "SPI support mode: SPI_LSB_FIRST");
            info->mode |= SPI_LSB_FIRST;
        }

        /* SPI_3WIRE */
        mode = SPI_3WIRE;
        ret = ioctl(info->fd, SPI_IOC_WR_MODE, &mode);
        if (ret != ERROR) {
            print_test_case_log(LOG_TAG, info->case_id,
                               "SPI support mode: SPI_3WIRE");
            info->mode |= SPI_3WIRE;
        }

        /* SPI_LOOP */
        mode = SPI_LOOP;
        ret = ioctl(info->fd, SPI_IOC_WR_MODE, &mode);
        if (ret != ERROR) {
            print_test_case_log(LOG_TAG, info->case_id,
                               "SPI support mode: SPI_LOOP");
            info->mode |= SPI_LOOP;
        }

        /* SPI_NO_CS */
        mode = SPI_NO_CS;
        ret = ioctl(info->fd, SPI_IOC_WR_MODE, &mode);
        if (ret != ERROR) {
            print_test_case_log(LOG_TAG, info->case_id,
                               "SPI support mode: SPI_NO_CS");
            info->mode |= SPI_NO_CS;
        }

        /* SPI_READY */
        mode = SPI_READY;
        ret = ioctl(info->fd, SPI_IOC_WR_MODE, &mode);
        if (ret != ERROR) {
            print_test_case_log(LOG_TAG, info->case_id,
                               "SPI support mode: SPI_READY");
            info->mode |= SPI_READY;
        }
        close(info->fd);

        /* Clean not support ioctl error */
        if (info->mode > 0) {
            ret = 0;
        } else {
            ret = -errno;
        }
    }

    print_test_result(info->case_id, ret);

    return ret;
}

/**
 * @brief Test Rail test case C1052
 *
 * C1052: SPI Protocol Bits Per word Mask Response contains the Bits Per Word
 * that the SPI Master supports.
 * This test case verifies that the SPI Protocol Bits Per Word Mask Response
 * payload contains a Four-byte value representing the Bits Per Word Mask of
 * the SPI Master.
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_1052_get_bpw(struct spi_app_info *info)
{
    int ret = 0, mode = 0, bpw = 0, speed = 0, i = 0, bpw_flag = 0;
    char spistr[SPI_STR_BUF];

    /* If use for loop to send bpw value, but if ioctl send message had first
     * error, other ioctl send message will fail */
    if (info->bpw1 > 0) {
        if (!info->data_flag) {
            snprintf(spistr, sizeof(spistr), "0x80,0x01,0x02");
            parse_transfer_data(info, spistr);
        }

        /* open device */
        info->fd = open(device, O_RDWR);
        if (info->fd < 0) {
            ret = -ENODEV;
        }

        /* SPI mode */
        mode = info->mode;
        ret = ioctl(info->fd, SPI_IOC_WR_MODE, &mode);
        if (ret == ERROR) {
            ret = -EINVAL;
            goto error_ioctl;
        }

        ret = ioctl(info->fd, SPI_IOC_RD_MODE, &mode);
        if (ret == ERROR) {
            ret = -EINVAL;
            goto error_ioctl;
        }

        /* bits per word */
        bpw = info->bpw1;
        ret = ioctl(info->fd, SPI_IOC_WR_BITS_PER_WORD, &bpw);
        ret = ioctl(info->fd, SPI_IOC_RD_BITS_PER_WORD, &bpw);

        /* speed */
        speed = info->speed;
        ret = ioctl(info->fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
        ret = ioctl(info->fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);

        ret = ioctl(info->fd, SPI_IOC_MESSAGE(info->ntransfers), spi_xfrs);
        if (ret > 1) {
            bpw_flag = 1;
            snprintf(spistr, sizeof(spistr), "SPI BPW support %d", info->bpw1);
            print_test_case_log(LOG_TAG, info->case_id, spistr);
            dump_xfrs(info);
        }
        close(info->fd);
    }

    if (info->bpw2 > 0) {
        info->bpw1 = info->bpw2;
        if (!info->data_flag) {
            snprintf(spistr, sizeof(spistr), "0x80,0x01,0x02");
            parse_transfer_data(info, spistr);
        }

        /* open device */
        info->fd = open(device, O_RDWR);
        if (info->fd < 0) {
            ret = -ENODEV;
        }

        /* SPI mode */
        mode = info->mode;
        ret = ioctl(info->fd, SPI_IOC_WR_MODE, &mode);
        if (ret == ERROR) {
            ret = -EINVAL;
            goto error_ioctl;
        }

        ret = ioctl(info->fd, SPI_IOC_RD_MODE, &mode);
        if (ret == ERROR) {
            ret = -EINVAL;
            goto error_ioctl;
        }

        /* bits per word */
        bpw = info->bpw2;
        ret = ioctl(info->fd, SPI_IOC_WR_BITS_PER_WORD, &bpw);
        ret = ioctl(info->fd, SPI_IOC_RD_BITS_PER_WORD, &bpw);

        /* speed */
        speed = info->speed;
        ret = ioctl(info->fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
        ret = ioctl(info->fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);

        ret = ioctl(info->fd, SPI_IOC_MESSAGE(info->ntransfers), spi_xfrs);
        if (ret > 1) {
            bpw_flag = 1;
            snprintf(spistr, sizeof(spistr), "SPI BPW support %d", info->bpw2);
            print_test_case_log(LOG_TAG, info->case_id, spistr);
            dump_xfrs(info);
        }
        close(info->fd);
    }

    if (info->bpw3 > 0) {
        info->bpw1 = info->bpw3;
        if (!info->data_flag) {
            snprintf(spistr, sizeof(spistr), "0x80,0x01,0x02");
            parse_transfer_data(info, spistr);
        }

        /* open device */
        info->fd = open(device, O_RDWR);
        if (info->fd < 0) {
            ret = -ENODEV;
        }

        /* SPI mode */
        mode = info->mode;
        ret = ioctl(info->fd, SPI_IOC_WR_MODE, &mode);
        if (ret == ERROR) {
            ret = -EINVAL;
            goto error_ioctl;
        }

        ret = ioctl(info->fd, SPI_IOC_RD_MODE, &mode);
        if (ret == ERROR) {
            ret = -EINVAL;
            goto error_ioctl;
        }

        /* bits per word */
        bpw = info->bpw3;
        ret = ioctl(info->fd, SPI_IOC_WR_BITS_PER_WORD, &bpw);
        ret = ioctl(info->fd, SPI_IOC_RD_BITS_PER_WORD, &bpw);

        /* speed */
        speed = info->speed;
        ret = ioctl(info->fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
        ret = ioctl(info->fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);

        ret = ioctl(info->fd, SPI_IOC_MESSAGE(info->ntransfers), spi_xfrs);
        if (ret > 1) {
            bpw_flag = 1;
            snprintf(spistr, sizeof(spistr), "SPI BPW support %d", info->bpw3);
            print_test_case_log(LOG_TAG, info->case_id, spistr);
            dump_xfrs(info);
        }
        close(info->fd);
    }

error_ioctl:
    if (bpw_flag) {
        ret = 0;
    } else {
        ret = -errno;
    }

    print_test_result(info->case_id, ret);

    return ret;
}

/**
 * @brief Test Rail test case C1053
 *
 * C1053: Verify transfer with Clock phase mode is successful.
 * This test case verifies that SPI Transfer Response payload contains the data
 * requested when mode is set to GB_SPI_MODE_CPHASE in the SPI Protocol
 * Transfer Request Operation.
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_1053_CPHA(struct spi_app_info *info)
{
    int ret = 0, mode = 0, bpw = 0, speed = 0, i = 0, bpw_flag = 0;
    char spistr[SPI_STR_BUF];

    info->mode = SPI_CPHA;
    if (!info->data_flag) {
        snprintf(spistr, sizeof(spistr), "0x80,0x01,0x02");
        parse_transfer_data(info, spistr);
    }

    /* open device */
    info->fd = open(device, O_RDWR);
    if (info->fd < 0) {
        ret = -ENODEV;
        goto error_open;
    }

    /* SPI mode */
    mode = info->mode;
    ret = ioctl(info->fd, SPI_IOC_WR_MODE, &mode);
    if (ret == ERROR) {
        ret = -EINVAL;
        goto error_ioctl;
    }

    ret = ioctl(info->fd, SPI_IOC_RD_MODE, &mode);
    if (ret == ERROR) {
        ret = -EINVAL;
        goto error_ioctl;
    }

    /* bits per word */
    bpw = info->bpw1;
    ret = ioctl(info->fd, SPI_IOC_WR_BITS_PER_WORD, &bpw);
    ret = ioctl(info->fd, SPI_IOC_RD_BITS_PER_WORD, &bpw);

    /* speed */
    speed = info->speed;
    ret = ioctl(info->fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    ret = ioctl(info->fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);

    ret = ioctl(info->fd, SPI_IOC_MESSAGE(info->ntransfers), spi_xfrs);
    if (ret == ERROR) {
        ret = -EINVAL;
    } else {
        ret = 0;
        dump_xfrs(info);
    }

error_ioctl:
    close(info->fd);
error_open:
    print_test_result(info->case_id, ret);

    return ret;
}

/**
 * @brief Test Rail test case C1054
 *
 * C1054: Verify transfer with Clock polarity mode is successful.
 * This test case verifies that SPI Transfer Response payload contains the data
 * requested when mode is set to GB_SPI_MODE_CPOL in the SPI Protocol Transfer
 * Request Operation.
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_1054_CPOL(struct spi_app_info *info)
{
    int ret = 0, mode = 0, bpw = 0, speed = 0, i = 0, bpw_flag = 0;
    char spistr[SPI_STR_BUF];

    info->mode = SPI_CPOL;
    if (!info->data_flag) {
        snprintf(spistr, sizeof(spistr), "0x80,0x01,0x02");
        parse_transfer_data(info, spistr);
    }

    /* open device */
    info->fd = open(device, O_RDWR);
    if (info->fd < 0) {
        ret = -ENODEV;
        goto error_open;
    }

    /* SPI mode */
    mode = info->mode;
    ret = ioctl(info->fd, SPI_IOC_WR_MODE, &mode);
    if (ret == ERROR) {
        ret = -EINVAL;
        goto error_ioctl;
    }

    ret = ioctl(info->fd, SPI_IOC_RD_MODE, &mode);
    if (ret == ERROR) {
        ret = -EINVAL;
        goto error_ioctl;
    }

    /* bits per word */
    bpw = info->bpw1;
    ret = ioctl(info->fd, SPI_IOC_WR_BITS_PER_WORD, &bpw);
    ret = ioctl(info->fd, SPI_IOC_RD_BITS_PER_WORD, &bpw);

    /* speed */
    speed = info->speed;
    ret = ioctl(info->fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    ret = ioctl(info->fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);

    ret = ioctl(info->fd, SPI_IOC_MESSAGE(info->ntransfers), spi_xfrs);
    if (ret == ERROR) {
        ret = -EINVAL;
    } else {
        ret = 0;
        dump_xfrs(info);
    }

error_ioctl:
    close(info->fd);
error_open:
    print_test_result(info->case_id, ret);

    return ret;
}

 /**
 * @brief Test Rail test case C1055
 *
 * C1055: Verify transfer with Chip select active high mode is successful.
 * This test case verifies that SPI Transfer Response payload contains the data
 * requested when mode is set to GB_SPI_MODE_CS_HIGH in the SPI Protocol
 * Transfer Request Operation.
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_1055_CS_high(struct spi_app_info *info)
{
    int ret = 0, mode = 0, bpw = 0, speed = 0, i = 0, bpw_flag = 0;
    char spistr[SPI_STR_BUF];

    info->mode = SPI_CS_HIGH;
    if (!info->data_flag) {
        snprintf(spistr, sizeof(spistr), "0x80,0x01,0x02");
        parse_transfer_data(info, spistr);
    }

    /* open device */
    info->fd = open(device, O_RDWR);
    if (info->fd < 0) {
        ret = -ENODEV;
        goto error_open;
    }

    /* SPI mode */
    mode = info->mode;
    ret = ioctl(info->fd, SPI_IOC_WR_MODE, &mode);
    if (ret == ERROR) {
        ret = -EINVAL;
        goto error_ioctl;
    }

    ret = ioctl(info->fd, SPI_IOC_RD_MODE, &mode);
    if (ret == ERROR) {
        ret = -EINVAL;
        goto error_ioctl;
    }

    /* bits per word */
    bpw = info->bpw1;
    ret = ioctl(info->fd, SPI_IOC_WR_BITS_PER_WORD, &bpw);
    ret = ioctl(info->fd, SPI_IOC_RD_BITS_PER_WORD, &bpw);

    /* speed */
    speed = info->speed;
    ret = ioctl(info->fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    ret = ioctl(info->fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);

    ret = ioctl(info->fd, SPI_IOC_MESSAGE(info->ntransfers), spi_xfrs);
    if (ret == ERROR) {
        ret = -EINVAL;
    } else {
        ret = 0;
        dump_xfrs(info);
    }

error_ioctl:
    close(info->fd);
error_open:
    print_test_result(info->case_id, ret);

    return ret;
}

 /**
 * @brief Test Rail test case C1056
 *
 * C1056: Verify transfer with Per-word bits-on-wire mode is successful.
 * This test case verifies that SPI Transfer Response payload contains the data
 * requested when mode is set to GB_SPI_MODE_LSB_FIRST in the SPI Protocol
 * Transfer Request Operation.
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_1056_LSB_first(struct spi_app_info *info)
{
    int ret = 0, mode = 0, bpw = 0, speed = 0, i = 0, bpw_flag = 0;
    char spistr[SPI_STR_BUF];

    info->mode = SPI_LSB_FIRST;
    if (!info->data_flag) {
        snprintf(spistr, sizeof(spistr), "0x80,0x01,0x02");
        parse_transfer_data(info, spistr);
    }

    /* open device */
    info->fd = open(device, O_RDWR);
    if (info->fd < 0) {
        ret = -ENODEV;
        goto error_open;
    }

    /* SPI mode */
    mode = info->mode;
    ret = ioctl(info->fd, SPI_IOC_WR_MODE, &mode);
    if (ret == ERROR) {
        ret = -EINVAL;
        goto error_ioctl;
    }

    ret = ioctl(info->fd, SPI_IOC_RD_MODE, &mode);
    if (ret == ERROR) {
        ret = -EINVAL;
        goto error_ioctl;
    }

    /* bits per word */
    bpw = info->bpw1;
    ret = ioctl(info->fd, SPI_IOC_WR_BITS_PER_WORD, &bpw);
    ret = ioctl(info->fd, SPI_IOC_RD_BITS_PER_WORD, &bpw);

    /* speed */
    speed = info->speed;
    ret = ioctl(info->fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    ret = ioctl(info->fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);

    ret = ioctl(info->fd, SPI_IOC_MESSAGE(info->ntransfers), spi_xfrs);
    if (ret == ERROR) {
        ret = -EINVAL;
    } else {
        ret = 0;
        dump_xfrs(info);
    }

error_ioctl:
    close(info->fd);
error_open:
    print_test_result(info->case_id, ret);

    return ret;
}

 /**
 * @brief Test Rail test case C1057
 *
 * C1057: Verify transfer with SI/SO Signals shared mode is successful.
 * This test case verifies that SPI Transfer Response payload contains the data
 * requested when mode is set to GB_SPI_MODE_3WIRE in the SPI Protocol Transfer
 * Request Operation.
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_1057_3WIRE(struct spi_app_info *info)
{
    int ret = 0, mode = 0, bpw = 0, speed = 0, i = 0, bpw_flag = 0;
    char spistr[SPI_STR_BUF];

    info->mode = SPI_3WIRE;
    if (!info->data_flag) {
        snprintf(spistr, sizeof(spistr), "0x80,0x01,0x02");
        parse_transfer_data(info, spistr);
    }

    /* open device */
    info->fd = open(device, O_RDWR);
    if (info->fd < 0) {
        ret = -ENODEV;
        goto error_open;
    }

    /* SPI mode */
    mode = info->mode;
    ret = ioctl(info->fd, SPI_IOC_WR_MODE, &mode);
    if (ret == ERROR) {
        ret = -EINVAL;
        goto error_ioctl;
    }

    ret = ioctl(info->fd, SPI_IOC_RD_MODE, &mode);
    if (ret == ERROR) {
        ret = -EINVAL;
        goto error_ioctl;
    }

    /* bits per word */
    bpw = info->bpw1;
    ret = ioctl(info->fd, SPI_IOC_WR_BITS_PER_WORD, &bpw);
    ret = ioctl(info->fd, SPI_IOC_RD_BITS_PER_WORD, &bpw);

    /* speed */
    speed = info->speed;
    ret = ioctl(info->fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    ret = ioctl(info->fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);

    ret = ioctl(info->fd, SPI_IOC_MESSAGE(info->ntransfers), spi_xfrs);
    if (ret == ERROR) {
        ret = -EINVAL;
    } else {
        ret = 0;
        dump_xfrs(info);
    }

error_ioctl:
    close(info->fd);
error_open:
    print_test_result(info->case_id, ret);

    return ret;
}

 /**
 * @brief Test Rail test case C1058
 *
 * C1058: Verify transfer with loopback mode is successful.
 * This test case verifies that SPI Transfer Response payload contains the data
 * requested when mode is set to GB_SPI_MODE_LOOP in the SPI Protocol Transfer
 * Request Operation.
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_1058_loop(struct spi_app_info *info)
{
    int ret = 0, mode = 0, bpw = 0, speed = 0, i = 0, bpw_flag = 0;
    char spistr[SPI_STR_BUF];

    info->mode = SPI_LOOP;
    if (!info->data_flag) {
        snprintf(spistr, sizeof(spistr), "0x80,0x01,0x02");
        parse_transfer_data(info, spistr);
    }

    /* open device */
    info->fd = open(device, O_RDWR);
    if (info->fd < 0) {
        ret = -ENODEV;
        goto error_open;
    }

    /* SPI mode */
    mode = info->mode;
    ret = ioctl(info->fd, SPI_IOC_WR_MODE, &mode);
    if (ret == ERROR) {
        ret = -EINVAL;
        goto error_ioctl;
    }

    ret = ioctl(info->fd, SPI_IOC_RD_MODE, &mode);
    if (ret == ERROR) {
        ret = -EINVAL;
        goto error_ioctl;
    }

    /* bits per word */
    bpw = info->bpw1;
    ret = ioctl(info->fd, SPI_IOC_WR_BITS_PER_WORD, &bpw);
    ret = ioctl(info->fd, SPI_IOC_RD_BITS_PER_WORD, &bpw);

    /* speed */
    speed = info->speed;
    ret = ioctl(info->fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    ret = ioctl(info->fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);

    ret = ioctl(info->fd, SPI_IOC_MESSAGE(info->ntransfers), spi_xfrs);
    if (ret == ERROR) {
        ret = -EINVAL;
    } else {
        ret = 0;
        dump_xfrs(info);
    }

error_ioctl:
    close(info->fd);
error_open:
    print_test_result(info->case_id, ret);

    return ret;
}

 /**
 * @brief Test Rail test case C1059
 *
 * C1059: Verify transfer with no chip select mode is successful.
 * This test case verifies that SPI Transfer Response payload contains the data
 * requested when mode is set to GB_SPI_MODE_NO_CS in the SPI Protocol Transfer
 * Request Operation.
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_1059_no_CS(struct spi_app_info *info)
{
    int ret = 0, mode = 0, bpw = 0, speed = 0, i = 0, bpw_flag = 0;
    char spistr[SPI_STR_BUF];

    info->mode = SPI_NO_CS;
    if (!info->data_flag) {
        snprintf(spistr, sizeof(spistr), "0x80,0x01,0x02");
        parse_transfer_data(info, spistr);
    }

    /* open device */
    info->fd = open(device, O_RDWR);
    if (info->fd < 0) {
        ret = -ENODEV;
        goto error_open;
    }

    /* SPI mode */
    mode = info->mode;
    ret = ioctl(info->fd, SPI_IOC_WR_MODE, &mode);
    if (ret == ERROR) {
        ret = -EINVAL;
        goto error_ioctl;
    }

    ret = ioctl(info->fd, SPI_IOC_RD_MODE, &mode);
    if (ret == ERROR) {
        ret = -EINVAL;
        goto error_ioctl;
    }

    /* bits per word */
    bpw = info->bpw1;
    ret = ioctl(info->fd, SPI_IOC_WR_BITS_PER_WORD, &bpw);
    ret = ioctl(info->fd, SPI_IOC_RD_BITS_PER_WORD, &bpw);

    /* speed */
    speed = info->speed;
    ret = ioctl(info->fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    ret = ioctl(info->fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);

    ret = ioctl(info->fd, SPI_IOC_MESSAGE(info->ntransfers), spi_xfrs);
    if (ret == ERROR) {
        ret = -EINVAL;
    } else {
        ret = 0;
        dump_xfrs(info);
    }

error_ioctl:
    close(info->fd);
error_open:
    print_test_result(info->case_id, ret);

    return ret;
}

 /**
 * @brief Test Rail test case C1060
 *
 * C1060: Verify transfer with slave mode is successful.
 * This test case verifies that SPI Transfer Response payload contains the data
 * requested when mode is set to GB_SPI_MODE_READY in the SPI Protocol Transfer
 * Request Operation.
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int ARA_1060_ready(struct spi_app_info *info)
{
    int ret = 0, mode = 0, bpw = 0, speed = 0, i = 0, bpw_flag = 0;
    char spistr[SPI_STR_BUF];

    info->mode = SPI_READY;
    if (!info->data_flag) {
        snprintf(spistr, sizeof(spistr), "0x80,0x01,0x02");
        parse_transfer_data(info, spistr);
    }

    /* open device */
    info->fd = open(device, O_RDWR);
    if (info->fd < 0) {
        ret = -ENODEV;
        goto error_open;
    }

    /* SPI mode */
    mode = info->mode;
    ret = ioctl(info->fd, SPI_IOC_WR_MODE, &mode);
    if (ret == ERROR) {
        ret = -EINVAL;
        goto error_ioctl;
    }

    ret = ioctl(info->fd, SPI_IOC_RD_MODE, &mode);
    if (ret == ERROR) {
        ret = -EINVAL;
        goto error_ioctl;
    }

    /* bits per word */
    bpw = info->bpw1;
    ret = ioctl(info->fd, SPI_IOC_WR_BITS_PER_WORD, &bpw);
    ret = ioctl(info->fd, SPI_IOC_RD_BITS_PER_WORD, &bpw);

    /* speed */
    speed = info->speed;
    ret = ioctl(info->fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    ret = ioctl(info->fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);

    ret = ioctl(info->fd, SPI_IOC_MESSAGE(info->ntransfers), spi_xfrs);
    if (ret == ERROR) {
        ret = -EINVAL;
    } else {
        ret = 0;
        dump_xfrs(info);
    }

error_ioctl:
    close(info->fd);
error_open:
    print_test_result(info->case_id, ret);

    return ret;
}

/**
 * @brief Switch case number and check case number is correct.
 *
 * @param info The GPIO info from user
 * @return 0 on success, error code on failure
 */
static int switch_case_number(struct spi_app_info *info)
{
        switch (info->case_id) {
            case 1051:
                return ARA_1051_get_mode(info);
            case 1052:
                return ARA_1052_get_bpw(info);
            case 1053:
                return ARA_1053_CPHA(info);
            case 1054:
                return ARA_1054_CPOL(info);
            case 1055:
                return ARA_1055_CS_high(info);
            case 1056:
                return ARA_1056_LSB_first(info);
            case 1057:
                return ARA_1057_3WIRE(info);
            case 1058:
                return ARA_1058_loop(info);
            case 1059:
                return ARA_1059_no_CS(info);
            case 1060:
                return ARA_1060_ready(info);
            default:
                print_test_case_log(LOG_TAG, 0,
                                    "Error: The command had error case_id.");
                return -EINVAL;
        }
}

/**
 * @brief The gpiotest main function
 *
 * @param argc The spitest main arguments count
 * @param argv The spitest main arguments data
 * @return 0 on success, error code on failure
 */
int main(int argc, char **argv)
{
    int ret = 0;
    struct spi_app_info info;

    if (argc < 2) {
        print_usage(argv[0]);
        ret = -EINVAL;
    }

    default_params(&info);

    if (!ret) {
        if (command_parse(&info, argc, argv)) {
            ret = -EINVAL;
        }
    }

    parse_transfer_data(&info, argv[optind++]);

    /* Switch test case */
    if (!ret) {
        ret = switch_case_number(&info);
        check_step_result(info.case_id, ret);
    }

    return ret;
}
