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
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/hidraw.h>

#include <libudev.h>
#include "hid-task.h"

/**
 * @brief Find hidraw interface by VendorID and ProductID
 *
 * @param info The HID info from user
 * @return 0 on success, error code on failure
 */
static int find_hidraw_by_id(struct hid_test_info *info)
{
    struct udev *udev;
    struct udev_enumerate *enumerate;
    struct udev_list_entry *devices, *dev_list_entry;
    struct udev_device *dev;
    const char *path;
    const char *devpath;
    int fd;
    int ret = -ENODEV;
    char buf[BUFLENGTH];
    struct hidraw_devinfo devinfo;

    /* Create the udev object */
    if (!(udev = udev_new())) {
        printf("Can't create udev\n");
        return -ENOMEM;
    }

    /* Create a list of the devices in the 'hidraw' subsystem. */
    enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "hidraw");
    udev_enumerate_scan_devices(enumerate);
    devices = udev_enumerate_get_list_entry(enumerate);
    udev_list_entry_foreach(dev_list_entry, devices) {
        path = udev_list_entry_get_name(dev_list_entry);
        dev = udev_device_new_from_syspath(udev, path);
        if (dev != NULL) {
            devpath = udev_device_get_devnode(dev);
            fd = open(devpath, O_RDWR|O_NONBLOCK);
            if (fd < 0) {
                perror("Unable to open device");
                ret = -ENODEV;
                udev_device_unref(dev);
                break;
            }

            /* Get HID Raw Info */
            ret = ioctl(fd, HIDIOCGRAWINFO, &devinfo);
            if (ret < 0) {
                perror("HIDIOCGRAWINFO");
            } else {
                printf("HID Raw Info:\n");
                printf("\tvendor: 0x%04hx\n", devinfo.vendor);
                printf("\tproduct: 0x%04hx\n", devinfo.product);
                if ((devinfo.vendor != info->vid) ||
                    (devinfo.product != info->pid)) {
                    printf("Not expected HID Device Descriptor\n");
                    ret = -ENODATA;
                } else {
                    printf("HID Device Descriptor matched!\n");
                    strcpy(info->devpath, devpath);
                    break;
                }
            }

            close(fd);
            udev_device_unref(dev);
        }
    }

    /* Free the enumerator object */
    udev_enumerate_unref(enumerate);
    udev_unref(udev);

    return ret;
}

/**
 * @brief Testrail test case C5853
 *
 * C5853: Retrieve HID Descriptor from HID device. This test case gets vendor
 * and product from HIDRAW interface. It verifies version, get_descriptor, and
 * power_on operations of Greybus HID Protocol
 *
 * @param info The HID info from user
 * @return 0 on success, error code on failure
 */
int ARA_5853_get_hid_descriptor(struct hid_test_info *info)
{
    return find_hidraw_by_id(info);
}

/**
 * @brief Testrail test case C5854
 *
 * C5854: Retrieve HID Report Descriptor from HID device. This test case gets
 * HID Report Descriptor size and Report Descriptor from HIDRAW interface. It
 * verifies get_report_descriptor operation of Greybus HID Protocol
 *
 * @param info The HID info from user
 * @return 0 on success, error code on failure
 */
int ARA_5854_get_hid_report_descriptor(struct hid_test_info *info)
{
    int fd;
    int ret;
    int desc_size = 0;
    int i;
    struct hidraw_report_descriptor rpt_desc;

    if (!(ret = find_hidraw_by_id(info))) {
        fd = open(info->devpath, O_RDWR|O_NONBLOCK);
        if (fd < 0) {
            perror("Unable to open device");
            return -ENODEV;
        }

        /* Get HID Report Descriptor size */
        ret = ioctl(fd, HIDIOCGRDESCSIZE, &desc_size);
        if (ret < 0) {
            perror("HIDIOCGRDESCSIZE");
        } else {
            printf("HID Report Descriptor size: %d\n", desc_size);

            if (desc_size == info->desc_len) {
                /* Get HID Report Descriptor */
                rpt_desc.size = desc_size;
                ret = ioctl(fd, HIDIOCGRDESC, &rpt_desc);
                if (ret < 0) {
                    perror("HIDIOCGRDESC");
                } else {
                    printf("HID Report Descriptor:\n");
                    for (i = 0; i < (int)rpt_desc.size; i++) {
                        if (rpt_desc.value[i] == info->desc[i]) {
                            printf("%hhx ", rpt_desc.value[i]);
                        } else {
                            ret = -ENODATA;
                            break;
                        }
                    }
                    printf("\n");
                    if (ret != -ENODATA) {
                        printf("HID Report Descriptor matched!\n");
                    }
                }
            } else {
                ret = -ENODATA;
            }
        }

        close(fd);
    }

    return ret;
}

/**
 * @brief Testrail test case C5855
 *
 * C5855: Set/Get Feature to/from HID device. This test case sets feature to
 * and gets it back from HIDRAW interface. It verifies get_report and set_report
 * for Feature report of Greybus HID Protocol
 *
 * @param info The HID info from user
 * @return 0 on success, error code on failure
 */
int ARA_5855_test_feature(struct hid_test_info *info)
{
    int fd;
    int ret;
    int i;
    char buf[BUFLENGTH];

    if (!(ret = find_hidraw_by_id(info))) {
        fd = open(info->devpath, O_RDWR|O_NONBLOCK);
        if (fd < 0) {
            perror("Unable to open device");
            return -ENODEV;
        }

        memset(buf, 0x0, sizeof(buf));

        /* Set Feature */
        buf[0] = 0x9; /* Report Number */
        buf[1] = 0xff;
        buf[2] = 0xff;
        buf[3] = 0xff;
        ret = ioctl(fd, HIDIOCSFEATURE(4), buf);
        if (ret < 0)
            perror("HIDIOCSFEATURE");
        else
            printf("ioctl HIDIOCSFEATURE returned: %d\n", ret);

        /* Get Feature */
        buf[0] = 0x9; /* Report Number */
        ret = ioctl(fd, HIDIOCGFEATURE(BUFLENGTH), buf);
        if (ret < 0) {
            perror("HIDIOCGFEATURE");
        } else {
            printf("ioctl HIDIOCGFEATURE returned: %d\n", ret);
            printf("Report data (not containing the report number):\n\t");
            for (i = 0; i < ret; i++)
                printf("%hhx ", buf[i]);
            puts("\n");
        }

        close(fd);
    }

    return ret;
}

/**
 * @brief Testrail test case C5856
 *
 * C5856: Send/Get Report to/from HID device. This test case sends an output
 * report to, and gets an input report from HIDRAW interface. It verifies
 * get_report and set_report for input/output report of Greybus HID Protocol
 *
 * @param info The HID info from user
 * @return 0 on success, error code on failure
 */
int ARA_5856_test_report(struct hid_test_info *info)
{
    int fd;
    int ret;
    int i;
    char buf[BUFLENGTH];

    if (!(ret = find_hidraw_by_id(info))) {
        fd = open(info->devpath, O_RDWR|O_NONBLOCK);
        if (fd < 0) {
            perror("Unable to open device");
            return -ENODEV;
        }

        memset(buf, 0x0, sizeof(buf));

        /* Send a Report to the device */
        buf[0] = 0x1; /* Report Number */
        buf[1] = 0x77;
        ret = write(fd, buf, 2);
        if (ret < 0) {
            printf("Error: %d\n", errno);
            perror("write");
        } else {
            printf("write() wrote %d bytes\n", ret);
        }

        /* Get a Report from the device */
        ret = read(fd, buf, 16);
        if (ret < 0) {
            perror("read");
        } else {
            printf("read() read %d bytes:\n\t", ret);
            for (i = 0; i < ret; i++)
                printf("%hhx ", buf[i]);
            puts("\n");
        }

        close(fd);
    }

    return ret;
}
