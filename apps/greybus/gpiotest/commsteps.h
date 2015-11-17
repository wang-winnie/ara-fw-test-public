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

#ifndef __COMMSTEPS_H__
#define __COMMSTEPS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Log tag for print function name */
#define LOG_TAG "ARA"
/* If getopt is -1 will exit */
#define ERROR (-1)

int get_greybus_gpio_count(int gpio_pin, char *gpio_max_count, int len);
int check_greybus_gpio(int *gpio_pin, int *gpio_max_count);
int activate_gpio_pin(int case_id, int gpio_pin);
int activate_gpio_multiple_pin(int case_id, int gpio_pin1, int gpio_pin2,
                               int gpio_pin3);
int deactivate_gpio_pin(int case_id, int gpio_pin);
int deactivate_gpio_multiple_pin(int case_id, int gpio_pin1, int gpio_pin2,
                                 int gpio_pin3);
int set_gpio_direction(int case_id, int gpio_pin, char *gpio_direction,
                       int len);
int get_gpio_direction(int case_id, int gpio_pin, char *gpio_direction,
                       int len);
int set_gpio_value(int case_id, int gpio_pin, char *gpio_value, int len);
int get_gpio_value(int case_id, int gpio_pin, char *gpio_value, int len);
int set_gpio_edge(int case_id, int gpio_pin, char *gpio_edge, int len);
int get_gpio_edge(int case_id, int gpio_pin, char *gpio_edge, int len);
void check_step_result(int case_id, int ret);
void print_test_result(int case_id, int ret);

#ifdef __cplusplus
}
#endif

#endif
