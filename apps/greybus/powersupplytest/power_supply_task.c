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
#include <fcntl.h>

#include <dirent.h>
#include "power_supply_task.h"

/**
 * @brief read the property value.
 *
 * @param info The gb_powersupply_info info.
 *
 * @return 0 on success, error code on failure.
 */
int read_prop(struct gb_powersupply_info *info)
{
    int ret ;
    char debugstr[MAXLENGTH];

    ret = debugfs_get_attr(info->prop_path, info->key, info->prop_value,
                                             sizeof(info->prop_value));

    if (ret) {
       snprintf(debugstr, sizeof(debugstr), "read property '%s' failed",
                                            info->key);
    } else {
       snprintf(debugstr, sizeof(debugstr), "read property '%s' is %s succeed",
                                            info->key, info->prop_value);
    }

    print_test_case_log(APP_NAME, info->caseid, debugstr);

    return ret;
}

/**
 * @brief Check the Greybus power supply exists
 *
 * @return 0 on success, -error code on failure
 */
int open_powersupply_dev(struct gb_powersupply_info *info)
{
    DIR *fdir = NULL;

    /* Scan debugfs, find Greybus sysfs */
    fdir = opendir(info->prop_path);
    if (fdir == NULL) {
        return -ENOENT;
    }

    closedir(fdir);
    return OPSUCCESS;
}

/**
 * @brief compare the user input and actual property value in string.
 *
 * @param info The gb_powersupply_info.
 *
 * @return return 0 on success, -error code on failure.
 */
int ara_compare_string(struct gb_powersupply_info *info)
{
    int ret = 0;
    /* check input value. */
    if (0 == strlen(info->buff)) {
        return -ENOINPUT;
    }

    ret = read_prop(info);
    if (ret) {
        return ret;
    }

    if (strcmp(info->buff, info->prop_value)) {
        return -EPERM;
    } else {
        return OPSUCCESS;
    }
}

/**
 * @brief compare the actual property value in user input range.
 *
 * @param info The gb_powersupply_info.
 *
 * @return return 0 on success, -error code on failure.
 */
int ara_compare_range(struct gb_powersupply_info *info)
{
    int ret = 0;
    int value = 0;
    /* check input value. */
    if ((-EINVAL == info->min) ||
        (-EINVAL == info->max) ||
        (0 < strlen(info->buff))) {
        return -ENOINPUT;
    }

    ret = read_prop(info);
    if(ret) {
        return ret;
    }

    value = atoi(info->prop_value);

    if ((info->min -1 <  value) &&
        (value < info->max +1)) {
        return OPSUCCESS;
    } else {
        return -EPERM;
    }
}

/**
 * @brief check the property is exist.
 *
 * @param info The gb_powersupply_info.
 *
 * @return return 0 on success, -error code on failure.
 */
int ara_check_value(struct gb_powersupply_info *info)
{
    return read_prop(info);
}

/**
 * @brief write the property.
 *
 * @param info The gb_powersupply_info.
 *
 * @return return 0 on success, -error code on failure.
 */
int ara_write_value(struct gb_powersupply_info *info)
{
    int ret;
    char propstr[LENGTH];
    char debugstr[MAXLENGTH];

     /* check input value. */
    if (0 >  strlen(info->buff)) {
        return -ENOINPUT;
    }

    ret = read_prop(info);
    if (ret)
        goto err_out;
    else
        snprintf(propstr, sizeof(propstr), "%s", info->prop_value);

    ret = debugfs_set_attr(info->prop_path, info->key, info->buff,
                                             sizeof(info->buff));
    if (ret) {
        goto err_out;
    }

    ret = ara_compare_string(info);
    if (ret) {
        goto err_out;
    }

    ret = debugfs_set_attr(info->prop_path, info->key, propstr,
                                             sizeof(propstr));

err_out:
    if (ret) {
       snprintf(debugstr, sizeof(debugstr), "write property '%s' failed ",
                                            info->key);
    } else {
       snprintf(debugstr, sizeof(debugstr), "write property '%s' succeed ",
                                            info->key);
    }

    print_test_case_log(APP_NAME, info->caseid, debugstr);

    return ret;
}
