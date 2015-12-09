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
#include <stdlib.h>
#include <getopt.h>

#include <libfwtest.h>
#include "power_supply_task.h"

void print_message(char *type)
{
    printf("           %s should between min and max\n\n", type);
}

/**
 * print usage.
 */
void print_usage(void)
{
    printf("\nUsage: powersupplytest [-p property_path] [-c case_id] [-d data] "
                                    "[-m min] [-M max] \n");
    printf("        -p: power supply property path.\n");
    printf("        -c: TestRail test id.\n");
    printf("        -d: data need to input for compare in test case. \n");
    printf("        -m: the range for minimum number.\n");
    printf("        -M: the range for maximun number. \n");
    printf("        -h: the detail for case usage. \n");
    printf("\nsupport cases: 1003, 1004, 1005, 1006, 1007, 1008, 1009, 1010,\n"
           "               1230, 1231, 1232, 1233, 1234, 1235, 1236, 1342,\n"
           "               1343, 1344, 1345, 1346, 1347, 1348, 1349, 1350,\n"
           "               1351, 1352, 1353, 1354, 1437, 1438, 1439, 1440,\n"
           "               1441, 1442, 1443, 1444, 1445, 1446, 1447, 1448,\n"
           "               1449, 1450, 1451, 1452, 1453, 1454, 1455, 1456,\n"
           "               1457, 1458, 1459, 1460, 1461, 1462, 1463, 1464,\n"
           "               1465, 2234, 2235, 2261, 2262, 2264, 2265, 2266,\n"
           "               2267, 2268, 2269, 2270, 2271\n\n");
}

/**
 * print usage.
 */
void print_case_usage(int caseid)
{
    print_usage();

    switch (caseid) {
    /* read case. */
    case 1003:
        printf("case 1003: powersupplytest -c 1003 [-d data]\n"
               "           possible value is 'Charging', 'Discharging', "
               "'Not charging', 'Full'\n\n");
        break;
    case 1004:
        printf("case 1004: powersupplytest -c 1004 [-d data]\n"
               "           possible value is 'Good', 'Overheat', 'Dead', "
               " 'Over voltage',\n"
               "                             'Unspecified failure', 'Cold',"
               " 'Watchdog timer expire',\n"
               "                             'Safety timer expire'\n\n");
        break;
    case 1005:
        printf("case 1005, powersupplytest -c 1005 [-d data]\n"
               "           data, 1 is present, 0 is not present \n\n");
        break;
    case 1006:
        printf("case 1006, powersupplytest -c 1006 [-d data]\n"
               "           data, 1 is online, 0 is not online \n\n");
        break;
    case 1007:
        printf("case 1007, powersupplytest -c 1007 [-d data]\n"
               "           data, 1 is authentic, 0 is not authentic \n\n");
        break;
    case 1008:
        printf("case 1008: powersupplytest -c 1008 [-d data]\n"
               "           possible technology is 'NiMH', 'Li-ion', "
               "'Li-poly','LiFe', \n"
               "                                  'NiCd', 'LiMn'\n\n");
        break;
    case 1009:
        printf("case 1009, powersupplytest -c 1009 [-m min] [-M max]\n");
        print_message("complete charge cycle counter");
        break;
    case 1010:
        printf("case 1010, powersupplytest -c 1010 [-m min] [-M max]\n");
        print_message("maximum voltage");
        break;
    case 1230:
        printf("case 1230, powersupplytest -c 1230 [-m min] [-M max]\n");
        print_message("minimum voltage");
        break;
    case 1231:
        printf("case 1231, powersupplytest -c 1231 [-d data]\n"
               "           data is maximum vlotage\n\n");
        break;
    case 1232:
        printf("case 1232, powersupplytest -c 1232 [-d data]\n"
               "           data is minimum vlotage\n\n");
        break;
    case 1233:
        printf("case 1233, powersupplytest -c 1233 [-m min] [-M max]\n");
        print_message("current voltage");
        break;
    case 1234:
        printf("case 1234, powersupplytest -c 1234 [-m min] [-M max]\n");
        print_message("average voltage");
        break;
    case 1235:
        printf("case 1235, powersupplytest -c 1235 [-m min] [-M max]\n");
        print_message("open circuit voltage");
        break;
    case 1236:
        printf("case 1236, powersupplytest -c 1236\n"
               "           check max current exists\n\n");
        break;
    case 1342:
        printf("case 1342, powersupplytest -c 1342\n"
               "           check current exists\n\n");
        break;
    case 1343:
        printf("case 1343, powersupplytest -c 1343\n"
               "           check ave current exists\n\n");
        break;
    case 1344:
        printf("case 1344, powersupplytest -c 1344 [-m min] [-M max]\n");
        print_message("instantaneous power consumption");
        break;
    case 1345:
        printf("case 1345, powersupplytest -c 1345 [-m min] [-M max]\n");
        print_message("average power consumption");
        break;
    case 1346:
        printf("case 1346, powersupplytest -c 1346 [-d data]\n"
               "           data is threshold for full charge\n\n");
        break;
    case 1347:
        printf("case 1347, powersupplytest -c 1347 [-d data]\n"
               "           data is threshold for empty charge\n\n");
        break;
    case 1348:
        printf("case 1348, powersupplytest -c 1348 [-m min] [-M max]\n");
        print_message("maximum charge");
        break;
    case 1349:
        printf("case 1349, powersupplytest -c 1349 [-m min] [-M max]\n");
        print_message("minimum charge");
        break;
    case 1350:
        printf("case 1350, powersupplytest -c 1350 [-m min] [-M max]\n");
        print_message("instantaneous charge value");
        break;
    case 1351:
        printf("case 1351, powersupplytest -c 1351 [-m min] [-M max]\n");
        print_message("average charge value");
        break;
    case 1352:
        printf("case 1352, powersupplytest -c 1352 [-m min] [-M max]\n");
        print_message("charge counter");
        break;
    case 1353:
        printf("case 1353, powersupplytest -c 1353 [-d data]\n"
               "           data is charge current\n\n");
        break;
    case 1354:
        printf("case 1354, powersupplytest -c 1354 [-d data]\n"
               "           data is maximum charge current supported\n\n");
        break;
    case 1437:
        printf("case 1437, powersupplytest -c 1437 [-d data]\n"
               "           data is Charge Voltage\n\n");
        break;
    case 1438:
        printf("case 1438, powersupplytest -c 1438 [-d data]\n"
               "           data is maximum charge voltage supported\n\n");
        break;
    case 1439:
        printf("case 1439, powersupplytest -c 1439 [-d data]\n"
               "           data is current charge control limit\n\n");
        break;
    case 1440:
        printf("case 1440, powersupplytest -c 1440 [-d data]\n"
               "           data is maximum charge control limit\n\n");
        break;
    case 1441:
        printf("case 1441, powersupplytest -c 1441 [-d data]\n"
               "           data is threshold for full energy\n\n");
        break;
    case 1442:
        printf("case 1442, powersupplytest -c 1442 [-d data]\n"
               "           data is threshold for empty energy\n\n");
        break;
    case 1443:
        printf("case 1443, powersupplytest -c 1443 [-m min] [-M max]\n");
        print_message("maximum energy");
        break;
    case 1444:
        printf("case 1444, powersupplytest -c 1444 [-m min] [-M max]\n");
        print_message("minimum energy");
        break;
    case 1445:
        printf("case 1445, powersupplytest -c 1445 [-m min] [-M max]\n");
        print_message("instantaneous energy value");
        break;
    case 1446:
        printf("case 1446, powersupplytest -c 1446 [-m min] [-M max]\n");
        print_message("average energy value");
        break;
    case 1447:
        printf("case 1447, powersupplytest -c 1447 [-m min] [-M max]\n");
        print_message("capacity percent");
        break;
    case 1448:
        printf("case 1448, powersupplytest -c 1448 [-d data]\n"
               "           data is minimum capacity alert value\n\n");
        break;
    case 1449:
        printf("case 1449, powersupplytest -c 1449 [-d data]\n"
               "           data is maximum capacity alert value\n\n");
        break;
    case 1450:
        printf("case 1450: powersupplytest -c 1450 [-d data]\n"
               "           possible value is 'Critical', 'Low', 'Normal', "
               "'High', 'Full'\n\n");
        break;
    case 1451:
        printf("case 1451, powersupplytest -c 1451 [-m min] [-M max]\n");
        print_message("temperature");
        break;
    case 1452:
        printf("case 1452, powersupplytest -c 1452 [-d data]\n"
               "           data is shutdown temperature\n\n");
        break;
    case 1453:
        printf("case 1453, powersupplytest -c 1453 [-d data]\n"
               "           data is minimum temperature alert\n\n");
        break;
    case 1454:
        printf("case 1454, powersupplytest -c 1454 [-m min] [-M max]\n");
        print_message("ambient temperature");
        break;
    case 1455:
        printf("case 1455, powersupplytest -c 1455 [-d data]\n"
               "           data is minimum ambient temperature alert\n\n");
        break;
    case 1456:
        printf("case 1456, powersupplytest -c 1456 [-d data]\n"
               "           data is maximum ambient temperature alert\n\n");
        break;
    case 1457:
        printf("case 1457, powersupplytest -c 1457 [-m min] [-M max]\n");
        print_message("instant seconds left to be considered empty");
        break;
    case 1458:
        printf("case 1458, powersupplytest -c 1458 [-m min] [-M max]\n");
        print_message("average seconds left to be considered empty");
        break;
    case 1459:
        printf("case 1459, powersupplytest -c 1459 [-m min] [-M max]\n");
        print_message("instant seconds left to be considered full");
        break;
    case 1460:
        printf("case 1460, powersupplytest -c 1460 [-m min] [-M max]\n");
        print_message("average seconds left to be considered full");
        break;
    case 1461:
        printf("case 1461: powersupplytest -c 1461 [-d data]\n"
               "           possible value is 'Battery', 'UPS', 'Mains', "
               "'USB', 'USB_DCP',\n"
               "                             'USB_CDP', 'USB_ACA'\n\n");
        break;
    case 1462:
        printf("case 1462: powersupplytest -c 1462 [-d data]\n"
               "           possible value is 'System', 'Device'\n\n");
        break;
    case 1463:
        printf("case 1463, powersupplytest -c 1463 [-d data]\n"
               "           data is manufacturer name\n\n");
        break;
    case 1464:
        printf("case 1464, powersupplytest -c 1464 [-d data]\n"
               "           data is model name\n\n");
        break;
    case 1465:
        printf("case 1465, powersupplytest -c 1465 [-d data]\n"
               "           data is serial Number\n\n");
        break;
    /* write case. */
    case 2234:
    case 2235:
    case 2261:
    case 2262:
    case 2264:
    case 2265:
    case 2266:
    case 2267:
    case 2268:
    case 2269:
    case 2270:
    case 2271:
        printf("case xxxx, powersupplytest -c xxxx [-d data]\n"
               "           data is set value\n\n");
        break;
    default:
        break;
    }
}

/**
 * main function.
 */
int main(int argc, char *argv[])
{
    int ret = 0;
    struct gb_powersupply_info info;
    int options = 0;
    enum run_case runcase = NONE;
    char *path;

    /* init param. */
    memset(info.buff, 0x00, sizeof(info.buff));
    info.caseid = 0;
    info.min = -EINVAL;
    info.max = -EINVAL;

    /* parse options. */
    while ((options = getopt(argc, argv, "c:p:w:d:m:M:h:")) != OPERROR) {
        switch (options) {
        case 'p':
            path = optarg;
            break;
        case 'c':
            info.caseid = atoi(optarg);
            break;
        case 'd':
            snprintf(info.buff, sizeof(info.buff), "%s", optarg);
            break;
        case 'm':
            info.min = atoi(optarg);
            break;
        case 'M':
            info.max = atoi(optarg);
            break;
        case 'h':
            print_case_usage(atoi(optarg));
            return 0;
        case '?':
        default:
            print_usage();
            return 0;
        }
    }

    if (path) {
        snprintf(info.prop_path, sizeof(info.prop_path), "%s/%s",
                                                       POWERSUPPLY_PATH, path);
    } else {
        print_test_case_log(APP_NAME, info.caseid, "The Power supply path is "
                                                "not set, please check again");
        print_test_case_result_only(info.caseid, OPERROR);
        print_usage();
        return 0;
    }

    /* check the Greybus node. */
    if (open_powersupply_dev(&info)) {
        print_test_case_log(APP_NAME, info.caseid, "The Power supply is not "
                                                   "exist, please check again");
        print_test_case_result_only(info.caseid, OPERROR);
        print_usage();
        return 0;
    }

    /* case switch. */
    switch (info.caseid) {
    /* read case. */
    case 1003:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1003));
        runcase = READ_EXACTLY;
        break;
    case 1004:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1004));
        runcase = READ_EXACTLY;
        break;
    case 1005:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1005));
        runcase = READ_EXACTLY;
        break;
    case 1006:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1006));
        runcase = READ_EXACTLY;
        break;
    case 1007:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1007));
        runcase = READ_EXACTLY;
        break;
    case 1008:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1008));
        runcase = READ_EXACTLY;
        break;
    case 1009:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1009));
        runcase = READ_EXACTLY;
        break;
    case 1010:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1010));
        runcase = READ_INRANGE;
        break;
    case 1230:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1230));
        runcase = READ_INRANGE;
        break;
    case 1231:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1231));
        runcase = READ_EXACTLY;
        break;
    case 1232:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1232));
        runcase = READ_EXACTLY;
        break;
    case 1233:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1233));
        runcase = READ_INRANGE;
        break;
    case 1234:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1234));
        runcase = READ_INRANGE;
        break;
    case 1235:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1235));
        runcase = READ_INRANGE;
        break;
    case 1236:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1236));
        runcase = READ_EXISTS;
        break;
    case 1342:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1342));
        runcase = READ_EXISTS;
        break;
    case 1343:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1343));
        runcase = READ_EXISTS;
        break;
    case 1344:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1344));
        runcase = READ_INRANGE;
        break;
    case 1345:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1345));
        runcase = READ_INRANGE;
        break;
    case 1346:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1346));
        runcase = READ_EXACTLY;
        break;
    case 1347:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1347));
        runcase = READ_EXACTLY;
        break;
    case 1348:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1348));
        runcase = READ_INRANGE;
        break;
    case 1349:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1349));
        runcase = READ_INRANGE;
        break;
    case 1350:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1350));
        runcase = READ_INRANGE;
        break;
    case 1351:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1351));
        runcase = READ_INRANGE;
        break;
    case 1352:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1352));
        runcase = READ_INRANGE;
        break;
    case 1353:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1353));
        runcase = READ_EXACTLY;
        break;
    case 1354:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1354));
        runcase = READ_EXACTLY;
        break;
    case 1437:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1437));
        runcase = READ_EXACTLY;
        break;
    case 1438:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1438));
        runcase = READ_EXACTLY;
        break;
    case 1439:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1439));
        runcase = READ_EXACTLY;
        break;
    case 1440:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1440));
        runcase = READ_EXACTLY;
        break;
    case 1441:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1441));
        runcase = READ_EXACTLY;
        break;
    case 1442:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1442));
        runcase = READ_EXACTLY;
        break;
    case 1443:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1443));
        runcase = READ_INRANGE;
        break;
    case 1444:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1444));
        runcase = READ_INRANGE;
        break;
    case 1445:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1445));
        runcase = READ_INRANGE;
        break;
    case 1446:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1446));
        runcase = READ_INRANGE;
        break;
    case 1447:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1447));
        runcase = READ_INRANGE;
        break;
    case 1448:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1448));
        runcase = READ_EXACTLY;
        break;
    case 1449:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1449));
        runcase = READ_EXACTLY;
        break;
    case 1450:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1450));
        runcase = READ_EXACTLY;
        break;
    case 1451:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1451));
        runcase = READ_INRANGE;
        break;
    case 1452:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1452));
        runcase = READ_EXACTLY;
        break;
    case 1453:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1453));
        runcase = READ_EXACTLY;
        break;
    case 1454:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1454));
        runcase = READ_INRANGE;
        break;
    case 1455:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1455));
        runcase = READ_EXACTLY;
        break;
    case 1456:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1456));
        runcase = READ_EXACTLY;
        break;
    case 1457:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1457));
        runcase = READ_INRANGE;
        break;
    case 1458:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1458));
        runcase = READ_INRANGE;
        break;
    case 1459:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1459));
        runcase = READ_INRANGE;
        break;
    case 1460:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1460));
        runcase = READ_INRANGE;
        break;
    case 1461:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1461));
        runcase = READ_EXACTLY;
        break;
    case 1462:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1462));
        runcase = READ_EXACTLY;
        break;
    case 1463:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1463));
        runcase = READ_EXACTLY;
        break;
    case 1464:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1464));
        runcase = READ_EXACTLY;
        break;
    case 1465:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1465));
        runcase = READ_EXACTLY;
        break;
    /* write case. */
    case 2234:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1231));
        runcase = WRITE_VERIFY;
        break;
    case 2235:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1232));
        runcase = WRITE_VERIFY;
        break;
    case 2261:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1346));
        runcase = WRITE_VERIFY;
        break;
    case 2262:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1347));
        runcase = WRITE_VERIFY;
        break;
    case 2264:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1353));
        runcase = WRITE_VERIFY;
        break;
    case 2265:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1354));
        runcase = WRITE_VERIFY;
        break;
    case 2266:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1437));
        runcase = WRITE_VERIFY;
        break;
    case 2267:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1438));
        runcase = WRITE_VERIFY;
        break;
    case 2268:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1441));
        runcase = WRITE_VERIFY;
        break;
    case 2269:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1442));
        runcase = WRITE_VERIFY;
        break;
    case 2270:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1452));
        runcase = WRITE_VERIFY;
        break;
    case 2271:
        snprintf(info.key, sizeof(info.key), "%s", NAME_ARA(1453));
        runcase = WRITE_VERIFY;
        break;
    default:
        ret = -ENOINPUT;
        break;
    }

    /* runcase switch. */
    switch (runcase) {
    case READ_EXISTS:
        ret = ara_check_value(&info);
        break;
    case READ_INRANGE:
        ret = ara_compare_range(&info);
        break;
    case READ_EXACTLY:
        ret = ara_compare_string(&info);
        break;
    case WRITE_VERIFY:
        ret = ara_write_value(&info);
        break;
    case NONE:
    default:
        ret = -ENOINPUT;
        break;
    }

    if (-ENOINPUT == ret) {
        print_usage();
    } else if(OPSUCCESS == ret ) {
        print_test_case_result_only(info.caseid, ret);
    } else {
       print_test_case_result(APP_NAME, info.caseid, ret, strerror(-ret));
   }

    return 0;
}
