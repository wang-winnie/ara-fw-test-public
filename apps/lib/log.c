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

#include "stdio.h"
#include "./include/libfwtest.h"

/**
 * @brief print test case result.
 *
 * @param TAG The test module name.
 * @param case_id The testlink id for test case.
 * @param result The test result.
 * @param data The error reason or if needed.
 */
void print_test_case_result(char *TAG, int case_id, int result, char *data)
{
    if (!TAG)
        TAG = "NONE";

    if (!result || !data)
        return print_test_case_result_only(case_id, result);
    else
    {
        printf("\n[I][%s-%d][fail][%s]\n", TAG, case_id, data);
        print_test_case_result_only(case_id, result);
     }
}

/**
 * @brief print test case result without data.
 *
 * @param case_id The testlink id for test case.
 * @param result The test result.
 */
void print_test_case_result_only(int case_id, int result)
{
    printf("\n[A][ARA-%d][%s]\n", case_id, result? "fail": "pass");
}

/**
 * @brief print debug message.
 *
 * @param TAG The test module name.
 * @param case_id The testlink id for test case.
 * @param data The debug message.
 */
void print_test_case_log(char *TAG, int case_id, char *data)
{
    if (!TAG)
        TAG = "NONE";

    if (!data)
        data = "NONE";

    printf("\n[D][%s-%d][%s]\n", TAG, case_id, data);
}
