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
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <linux/limits.h>
#include <ctype.h>

#include <libfwtest.h>
#include "commsteps.h"

/**
 * @brief Read GPIO debugfs to get Greybus GPIO max count
 *
 * @param case_id The GPIO test case number
 * @param gpio_pin Greybus GPIO base pin
 * @param gpio_max_count Greybus GPIO max count
 * @return 0 on success, error code on failure
 */
int get_greybus_gpio_count(int gpio_pin, char *gpio_max_count, int len)
{
    int ret = 0;
    char gpiostr[PATH_MAX];

    snprintf(gpiostr, sizeof(gpiostr), "%s%d", "/sys/class/gpio/gpiochip",
             gpio_pin);
    ret = debugfs_get_attr(gpiostr, "ngpio", gpio_max_count, len);
    return ret;
}

/**
 * @brief Check the Greybus GPIO controller exists
 *
 * @param gpio_pin GPIO test pin
 * @param gpio_max_count Greybus GPIO max count
 * @return 0 on success, error code on failure
 */
int check_greybus_gpio(int *gpio_pin, int *gpio_max_count)
{
    int ret = 0, i = 0;
    DIR *fdir;
    struct dirent *ptr;
    char buf[PATH_MAX] ,gpiostr[PATH_MAX];

    /* Scan debugfs, find Greybus GPIO sysfs */
    fdir = opendir("/sys/class/gpio");
    if(fdir == NULL) {
        return -ENOENT;
    }

    while ((ptr = readdir(fdir)) != NULL) {
        if (ptr->d_type == DT_LNK) {
            snprintf(gpiostr, sizeof(gpiostr), "%s%s", "/sys/class/gpio/",
                     ptr->d_name);
            if(debugfs_get_attr(gpiostr, "label", buf, sizeof(buf)) >= 0) {
                if(strcmp(buf, "greybus_gpio") == 0) {
                    /* re-assign gpiostr string */
                    snprintf(gpiostr, sizeof(gpiostr), "%s%s",
                             "/sys/class/gpio/", ptr->d_name);
                    ret = debugfs_get_attr(gpiostr, "base", buf, sizeof(buf));
                    *gpio_pin = atoi(buf);
                }
            }
        }
    }

    ret = get_greybus_gpio_count(*gpio_pin, buf, sizeof(buf));
    *gpio_max_count = atoi(buf);

    return ret;
}

/**
 * @brief Activate GPIO single pin
 *
 * @param case_id The GPIO test case number
 * @param gpio_pin GPIO test pin
 * @return 0 on success, error code on failure
 */
int activate_gpio_pin(int case_id, int gpio_pin)
{
    int ret = 0;
    char gpiostr[PATH_MAX];

    snprintf(gpiostr, sizeof(gpiostr), "%d\n", gpio_pin);
    ret = debugfs_set_attr("/sys/class/gpio", "export" , gpiostr,
                           sizeof(gpiostr));
    snprintf(gpiostr, sizeof(gpiostr), "%s%d", "Activate GPIO Pin: gpio",
             gpio_pin);
    if(!ret) {
        print_test_case_log(LOG_TAG, case_id, gpiostr);
    }

    return ret;
}

/**
 * @brief Activate GPIO multiple pin
 *
 * @param case_id The GPIO test case number
 * @param gpio_pin1 SDB APB bridge test GPIO pin1
 * @param gpio_pin2 SDB APB bridge test GPIO pin2
 * @param gpio_pin3 SDB APB bridge test GPIO pin3
 * @return 0 on success, error code on failure
 */
int activate_gpio_multiple_pin(int case_id, int gpio_pin1, int gpio_pin2,
                               int gpio_pin3)
{
    int ret = 0, i = 0;
    char gpiostr[PATH_MAX];

    /* export Greybus GPIO */
    snprintf(gpiostr, sizeof(gpiostr), "%d\n", gpio_pin1);
    ret = debugfs_set_attr("/sys/class/gpio", "export" , gpiostr,
                           sizeof(gpiostr));
    snprintf(gpiostr, sizeof(gpiostr), "%s%i", "Activate GPIO Pin: gpio",
             gpio_pin1);
    if(!ret) {
        print_test_case_log(LOG_TAG, case_id, gpiostr);
    }

    snprintf(gpiostr, sizeof(gpiostr), "%d\n", gpio_pin2);
    ret = debugfs_set_attr("/sys/class/gpio", "export" , gpiostr,
                           sizeof(gpiostr));
    snprintf(gpiostr, sizeof(gpiostr), "%s%i", "Activate GPIO Pin: gpio",
             gpio_pin2);
    if(!ret) {
        print_test_case_log(LOG_TAG, case_id, gpiostr);
    }

    snprintf(gpiostr, sizeof(gpiostr), "%d\n", gpio_pin3);
    ret = debugfs_set_attr("/sys/class/gpio", "export" , gpiostr,
                           sizeof(gpiostr));
    snprintf(gpiostr, sizeof(gpiostr), "%s%i", "Activate GPIO Pin: gpio",
             gpio_pin3);
    if(!ret) {
        print_test_case_log(LOG_TAG, case_id, gpiostr);
    }

    return ret;
}

/**
 * @brief Deactivate GPIO single pin
 *
 * @param case_id The GPIO test case number
 * @param gpio_pin GPIO test pin
 * @return 0 on success, error code on failure
 */
int deactivate_gpio_pin(int case_id, int gpio_pin)
{
    int ret = 0;
    char gpiostr[PATH_MAX];

    snprintf(gpiostr, sizeof(gpiostr), "%d\n", gpio_pin);
    ret = debugfs_set_attr("/sys/class/gpio", "unexport" , gpiostr,
                           sizeof(gpiostr));
    snprintf(gpiostr, sizeof(gpiostr), "%s%i", "Deactivate GPIO Pin: gpio",
             gpio_pin);
    if(!ret) {
        print_test_case_log(LOG_TAG, case_id, gpiostr);
    }

    return ret;
}

/**
 * @brief Deactivate GPIO multiple pins
 *
 * @param case_id The GPIO test case number
 * @param gpio_pin1 SDB APB bridge test GPIO pin1
 * @param gpio_pin2 SDB APB bridge test GPIO pin2
 * @param gpio_pin3 SDB APB bridge test GPIO pin3
 * @return 0 on success, error code on failure
 */
int deactivate_gpio_multiple_pin(int case_id, int gpio_pin1, int gpio_pin2,
                                 int gpio_pin3)
{
    int ret = 0, i = 0;
    char gpiostr[PATH_MAX];

    /* unexport Greybus GPIO */
    snprintf(gpiostr, sizeof(gpiostr), "%d\n", gpio_pin1);
    ret = debugfs_set_attr("/sys/class/gpio", "unexport" , gpiostr,
                           sizeof(gpiostr));
    snprintf(gpiostr, sizeof(gpiostr), "%s%i", "Deactivate GPIO Pin: gpio",
             gpio_pin1);
    if(!ret) {
        print_test_case_log(LOG_TAG, case_id, gpiostr);
    }

    snprintf(gpiostr, sizeof(gpiostr), "%d\n", gpio_pin2);
    ret = debugfs_set_attr("/sys/class/gpio", "unexport" , gpiostr,
                           sizeof(gpiostr));
    snprintf(gpiostr, sizeof(gpiostr), "%s%i", "Deactivate GPIO Pin: gpio",
             gpio_pin2);
    if(!ret) {
        print_test_case_log(LOG_TAG, case_id, gpiostr);
    }

    snprintf(gpiostr, sizeof(gpiostr), "%d\n", gpio_pin3);
    ret = debugfs_set_attr("/sys/class/gpio", "unexport" , gpiostr,
                           sizeof(gpiostr));
    snprintf(gpiostr, sizeof(gpiostr), "%s%i", "Deactivate GPIO Pin: gpio",
             gpio_pin3);
    if(!ret) {
        print_test_case_log(LOG_TAG, case_id, gpiostr);
    }

    return ret;
}

/**
 * @brief Set GPIO pin direction
 *
 * @param case_id The GPIO test case number
 * @param gpio_pin GPIO test pin
 * @param gpio_direction GPIO direction (in or out)
 * @param len gpio_direction buffer size
 * @return 0 on success, error code on failure
 */
int set_gpio_direction(int case_id, int gpio_pin, char *gpio_direction, int len)
{
    int ret = 0;
    char gpiostr[PATH_MAX];

    snprintf(gpiostr, sizeof(gpiostr), "%s%i", "/sys/class/gpio/gpio", gpio_pin)
             ;
    ret = debugfs_set_attr(gpiostr, "direction", gpio_direction, len);
    snprintf(gpiostr, sizeof(gpiostr), "Set GPIO%d direction = %s",  gpio_pin,
             gpio_direction);
    print_test_case_log(LOG_TAG, case_id, gpiostr);
    return ret;
}

/**
 * @brief Get GPIO pin direction
 *
 * @param case_id The GPIO test case number
 * @param gpio_pin GPIO test pin
 * @param gpio_direction GPIO direction (in or out)
 * @param len gpio_direction buffer size
 * @return 0 on success, error code on failure
 */
int get_gpio_direction(int case_id, int gpio_pin, char *gpio_direction, int len)
{
    int ret = 0;
    char gpiostr[PATH_MAX];

    snprintf(gpiostr, sizeof(gpiostr), "%s%i", "/sys/class/gpio/gpio",
             gpio_pin);
    ret = debugfs_get_attr(gpiostr, "direction", gpio_direction, len);
    snprintf(gpiostr, sizeof(gpiostr), "GPIO%d direction = %s", gpio_pin,
             gpio_direction);
    print_test_case_log(LOG_TAG, case_id, gpiostr);
    return ret;
}

/**
 * @brief Set GPIO pin value
 *
 * @param case_id The GPIO test case number
 * @param gpio_pin GPIO test pin
 * @param gpio_value GPIO value (0 or 1)
 * @param len gpio_value buffer size
 * @return 0 on success, error code on failure
 */
int set_gpio_value(int case_id, int gpio_pin, char *gpio_value, int len)
{
    int ret = 0;
    char gpiostr[PATH_MAX];

    snprintf(gpiostr, sizeof(gpiostr), "%s%i", "/sys/class/gpio/gpio",
             gpio_pin);
    ret = debugfs_set_attr(gpiostr, "value", gpio_value, len);
    snprintf(gpiostr, sizeof(gpiostr), "Set GPIO%d value = %d", gpio_pin,
             atoi(gpio_value));
    print_test_case_log(LOG_TAG, case_id, gpiostr);

    return ret;
}

/**
 * @brief Get GPIO pin value
 *
 * @param case_id The GPIO test case number
 * @param gpio_pin GPIO test pin
 * @param gpio_value GPIO value (0 or 1)
 * @param len gpio_value buffer size
 * @return 0 on success, negative errno on error
 */
int get_gpio_value(int case_id, int gpio_pin, char *gpio_value, int len)
{
    int ret = 0;
    char gpiostr[PATH_MAX];

    snprintf(gpiostr, sizeof(gpiostr), "%s%i", "/sys/class/gpio/gpio",
             gpio_pin);
    ret = debugfs_get_attr(gpiostr, "value", gpio_value, len);
    snprintf(gpiostr, sizeof(gpiostr), "GPIO%d value = %d", gpio_pin,
             atoi(gpio_value));
    print_test_case_log(LOG_TAG, case_id, gpiostr);

    return ret;
}

/**
 * @brief Set GPIO pin edge status
 *
 * @param case_id The GPIO test case number
 * @param gpio_pin GPIO test pin
 * @param gpio_edge GPIO edge (none, rising, falling, both)
 * @param len gpio_edge buffer size
 * @return 0 on success, error code on failure
 */
int set_gpio_edge(int case_id, int gpio_pin, char *gpio_edge, int len)
{
    int ret = 0;
    char gpiostr[PATH_MAX];

    snprintf(gpiostr, sizeof(gpiostr), "%s%i", "/sys/class/gpio/gpio",
             gpio_pin);
    ret = debugfs_set_attr(gpiostr, "edge", gpio_edge, len);
    snprintf(gpiostr, sizeof(gpiostr), "Set GPIO%d edge = %s", gpio_pin,
             gpio_edge);
    print_test_case_log(LOG_TAG, case_id, gpiostr);

    return ret;
}

/**
 * @brief Get GPIO pin edge status
 *
 * @param case_id The GPIO test case number
 * @param gpio_pin GPIO test pin
 * @param gpio_edge GPIO edge (none, rising, falling, both)
 * @param len gpio_edge buffer size
 * @return 0 on success, error code on failure
 */
int get_gpio_edge(int case_id, int gpio_pin, char *gpio_edge, int len)
{
    int ret = 0;
    char gpiostr[PATH_MAX];

    snprintf(gpiostr, sizeof(gpiostr), "%s%i", "/sys/class/gpio/gpio", gpio_pin)
             ;
    ret = debugfs_get_attr(gpiostr, "edge", gpio_edge, len);
    snprintf(gpiostr, sizeof(gpiostr), "GPIO%d edge = %s", gpio_pin, gpio_edge);
    print_test_case_log(LOG_TAG, case_id, gpiostr);

    return ret;
}

/**
 * @brief check step ret value and confirm step status
 *
 * @param case_id The GPIO test case number
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
 * @param case_id The GPIO test case number
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
