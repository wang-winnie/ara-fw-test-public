powersupplytest uasge

Usage: powersupplytest [-p property_path] [-c case_id] [-d data] [-m min] [-M max] 
        -p: power supply property path.
        -c: TestRail test id.
        -d: data need to input for compare in test case. 
        -m: the range for minimum number.
        -M: the range for maximun number. 
        -h: the detail for case usage. 

support cases: 1003, 1004, 1005, 1006, 1007, 1008, 1009, 1010,
               1230, 1231, 1232, 1233, 1234, 1235, 1236, 1342,
               1343, 1344, 1345, 1346, 1347, 1348, 1349, 1350,
               1351, 1352, 1353, 1354, 1437, 1438, 1439, 1440,
               1441, 1442, 1443, 1444, 1445, 1446, 1447, 1448,
               1449, 1450, 1451, 1452, 1453, 1454, 1455, 1456,
               1457, 1458, 1459, 1460, 1461, 1462, 1463, 1464,
               1465, 2234, 2235, 2261, 2262, 2264, 2265, 2266,
               2267, 2268, 2269, 2270, 2271

case 1003: powersupplytest -c 1003 [-d data]
           possible value is 'Charging', 'Discharging', 'Not charging', 'Full'

case 1004: powersupplytest -c 1004 [-d data]
           possible value is 'Good', 'Overheat', 'Dead',  'Over voltage',
                             'Unspecified failure', 'Cold', 'Watchdog timer expire',
                             'Safety timer expire'

case 1005, powersupplytest -c 1005 [-d data]
           data, 1 is present, 0 is not present 

case 1006, powersupplytest -c 1006 [-d data]
           data, 1 is online, 0 is not online 

case 1007, powersupplytest -c 1007 [-d data]
           data, 1 is authentic, 0 is not authentic 

case 1008: powersupplytest -c 1008 [-d data]
           possible technology is 'NiMH', 'Li-ion', 'Li-poly','LiFe', 
                                  'NiCd', 'LiMn'

case 1009, powersupplytest -c 1009 [-m min] [-M max]
           complete charge cycle counter should between min and max

case 1010, powersupplytest -c 1010 [-m min] [-M max]
           maximum voltage should between min and max

case 1230, powersupplytest -c 1230 [-m min] [-M max]
           minimum voltage should between min and max

case 1231, powersupplytest -c 1231 [-d data]
           data is maximum vlotage

case 1232, powersupplytest -c 1232 [-d data]
           data is minimum vlotage

case 1233, powersupplytest -c 1233 [-m min] [-M max]
           current voltage should between min and max

case 1234, powersupplytest -c 1234 [-m min] [-M max]
           average voltage should between min and max

case 1235, powersupplytest -c 1235 [-m min] [-M max]
           open circuit voltage should between min and max

case 1236, powersupplytest -c 1236
           check max current exists

case 1342, powersupplytest -c 1342
           check current exists

case 1343, powersupplytest -c 1343
           check ave current exists

case 1344, powersupplytest -c 1344 [-m min] [-M max]
           instantaneous power consumption should between min and max

case 1345, powersupplytest -c 1345 [-m min] [-M max]
           average power consumption should between min and max

case 1346, powersupplytest -c 1346 [-d data]
           data is threshold for full charge

case 1347, powersupplytest -c 1347 [-d data]
           data is threshold for empty charge

case 1348, powersupplytest -c 1348 [-m min] [-M max]
           maximum charge should between min and max

case 1349, powersupplytest -c 1349 [-m min] [-M max]
           minimum charge should between min and max

case 1350, powersupplytest -c 1350 [-m min] [-M max]
           instantaneous charge value should between min and max

case 1351, powersupplytest -c 1351 [-m min] [-M max]
           average charge value should between min and max

case 1352, powersupplytest -c 1352 [-m min] [-M max]
           charge counter should between min and max

case 1353, powersupplytest -c 1353 [-d data]
           data is charge current

case 1354, powersupplytest -c 1354 [-d data]
           data is maximum charge current supported

case 1437, powersupplytest -c 1437 [-d data]
           data is Charge Voltage

case 1438, powersupplytest -c 1438 [-d data]
           data is maximum charge voltage supported

case 1439, powersupplytest -c 1439 [-d data]
           data is current charge control limit

case 1440, powersupplytest -c 1440 [-d data]
           data is maximum charge control limit

case 1441, powersupplytest -c 1441 [-d data]
           data is threshold for full energy

case 1442, powersupplytest -c 1442 [-d data]
           data is threshold for empty energy

case 1443, powersupplytest -c 1443 [-m min] [-M max]
           maximum energy should between min and max

case 1444, powersupplytest -c 1444 [-m min] [-M max]
           minimum energy should between min and max

case 1445, powersupplytest -c 1445 [-m min] [-M max]
           instantaneous energy value should between min and max

case 1446, powersupplytest -c 1446 [-m min] [-M max]
           average energy value should between min and max

case 1447, powersupplytest -c 1447 [-m min] [-M max]
           capacity percent should between min and max

case 1448, powersupplytest -c 1448 [-d data]
           data is minimum capacity alert value

case 1449, powersupplytest -c 1449 [-d data]
           data is maximum capacity alert value

case 1450: powersupplytest -c 1450 [-d data]
           possible value is 'Critical', 'Low', 'Normal', 'High', 'Full'

case 1451, powersupplytest -c 1451 [-m min] [-M max]
           temperature should between min and max

case 1452, powersupplytest -c 1452 [-d data]
           data is shutdown temperature

case 1453, powersupplytest -c 1453 [-d data]
           data is minimum temperature alert

case 1454, powersupplytest -c 1454 [-m min] [-M max]
           ambient temperature should between min and max

case 1455, powersupplytest -c 1455 [-d data]
           data is minimum ambient temperature alert

case 1456, powersupplytest -c 1456 [-d data]
           data is maximum ambient temperature alert

case 1457, powersupplytest -c 1457 [-m min] [-M max]
           instant seconds left to be considered empty should between min and max

case 1458, powersupplytest -c 1458 [-m min] [-M max]
           average seconds left to be considered empty should between min and max

case 1459, powersupplytest -c 1459 [-m min] [-M max]
           instant seconds left to be considered full should between min and max

case 1460, powersupplytest -c 1460 [-m min] [-M max]
           average seconds left to be considered full should between min and max

case 1461: powersupplytest -c 1461 [-d data]
           possible value is 'Battery', 'UPS', 'Mains', 'USB', 'USB_DCP',
                             'USB_CDP', 'USB_ACA'

case 1462: powersupplytest -c 1462 [-d data]
           possible value is 'System', 'Device'

case 1463, powersupplytest -c 1463 [-d data]
           data is manufacturer name

case 1464, powersupplytest -c 1464 [-d data]
           data is model name

case 1465, powersupplytest -c 1465 [-d data]
           data is serial Number

case 2234, powersupplytest -c 2234 [-d data]
           data is set value

case 2235, powersupplytest -c 2235 [-d data]
           data is set value

case 2261, powersupplytest -c 2261 [-d data]
           data is set value

case 2262, powersupplytest -c 2262 [-d data]
           data is set value

case 2264, powersupplytest -c 2264 [-d data]
           data is set value

case 2265, powersupplytest -c 2265 [-d data]
           data is set value

case 2266, powersupplytest -c 2266 [-d data]
           data is set value

case 2267, powersupplytest -c 2267 [-d data]
           data is set value

case 2268, powersupplytest -c 2268 [-d data]
           data is set value

case 2269, powersupplytest -c 2269 [-d data]
           data is set value

case 2270, powersupplytest -c 2270 [-d data]
           data is set value

case 2271, powersupplytest -c 2271 [-d data]
           data is set value

