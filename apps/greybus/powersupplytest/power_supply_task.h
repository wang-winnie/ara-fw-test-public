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

#ifndef POWER_SUPPOLY_TASK_H_FILE
#define POWER_SUPPOLY_TASK_H_FILE

#include <stdint.h>

#define APP_NAME "POWER_SUPPLY"
#define POWERSUPPLY_PATH "/sys/class/power_supply"

#define OPSUCCESS 0
#define OPERROR -1
#define ENOINPUT 255

#define LENGTH 32
#define MAXLENGTH 128

#define NAME_ARA(y) NAME_ARA_##y

#define NAME_ARA_1003    "status"
#define NAME_ARA_1004    "health"
#define NAME_ARA_1005    "present"
#define NAME_ARA_1006    "online"
#define NAME_ARA_1007    "authentic"
#define NAME_ARA_1008    "technology"
#define NAME_ARA_1009    "cycle_count"
#define NAME_ARA_1010    "voltage_max"
#define NAME_ARA_1230    "voltage_min"
#define NAME_ARA_1231    "voltage_max_design"
#define NAME_ARA_1232    "voltage_min_design"
#define NAME_ARA_1233    "voltage_now"
#define NAME_ARA_1234    "voltage_avg"
#define NAME_ARA_1235    "voltage_ocv"
#define NAME_ARA_1236    "current_max"
#define NAME_ARA_1342    "current_now"
#define NAME_ARA_1343    "current_avg"
#define NAME_ARA_1344    "power_now"
#define NAME_ARA_1345    "power_avg"
#define NAME_ARA_1346    "charge_full_design"
#define NAME_ARA_1347    "charge_empty_design"
#define NAME_ARA_1348    "charge_full"
#define NAME_ARA_1349    "charge_empty"
#define NAME_ARA_1350    "charge_now"
#define NAME_ARA_1351    "charge_avg"
#define NAME_ARA_1352    "charge_counter"
#define NAME_ARA_1353    "constant_charge_current"
#define NAME_ARA_1354    "constant_charge_current_max"
#define NAME_ARA_1437    "constant_charge_voltage"
#define NAME_ARA_1438    "constant_charge_voltage_max"
#define NAME_ARA_1439    "charge_control_limit"
#define NAME_ARA_1440    "charge_control_limit_max"
#define NAME_ARA_1441    "energy_full_design"
#define NAME_ARA_1442    "energy_empty_design"
#define NAME_ARA_1443    "energy_full"
#define NAME_ARA_1444    "energy_empty"
#define NAME_ARA_1445    "energy_now"
#define NAME_ARA_1446    "energy_avg"
#define NAME_ARA_1447    "capacity"
#define NAME_ARA_1448    "capacity_alert_min"
#define NAME_ARA_1449    "capacity_alert_max"
#define NAME_ARA_1450    "capacity_level"
#define NAME_ARA_1451    "temp"
#define NAME_ARA_1452    "temp_alert_max"
#define NAME_ARA_1453    "temp_alert_min"
#define NAME_ARA_1454    "temp_ambient"
#define NAME_ARA_1455    "temp_ambient_alert_min"
#define NAME_ARA_1456    "temp_ambient_alert_max"
#define NAME_ARA_1457    "time_to_empty_now"
#define NAME_ARA_1458    "time_to_empty_avg"
#define NAME_ARA_1459    "time_to_full_now"
#define NAME_ARA_1460    "time_to_full_avg"
#define NAME_ARA_1461    "type"
#define NAME_ARA_1462    "scope"
#define NAME_ARA_1463    "manufacturer"
#define NAME_ARA_1464    "model_name"
#define NAME_ARA_1465    "serial_number"

enum run_case {
    NONE,
    READ_EXISTS,
    READ_INRANGE,
    READ_EXACTLY,
    WRITE_VERIFY,
};

struct gb_powersupply_info {
    char prop_path[LENGTH];
    int caseid;
    int runcase;
    char key[LENGTH];
    char prop_value[LENGTH];
    /* input value from user */
    int min;
    int max;
    char buff[LENGTH];
};

int open_powersupply_dev(struct gb_powersupply_info *info);
int ara_compare_string(struct gb_powersupply_info *info);
int ara_compare_range(struct gb_powersupply_info *info);
int ara_check_value(struct gb_powersupply_info *info);
int ara_write_value(struct gb_powersupply_info *info);

#endif /* POWER_SUPPOLY_TASK_H_FILE */
