/*
 * Copyright (c) 2021 FuZhou LOCKZHINER Electronic Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#ifndef __E53_WISDOM_GARDENS_H__
#define __E53_WISDOM_GARDENS_H__

#include "lz_hardware.h"

typedef enum
{
    OFF = 0,
    ON
} SWITCH_STATUS_ENUM;

void e53_wg_init(uint32_t args);
void e53_wg_buzzer_set_status(SWITCH_STATUS_ENUM status);
unsigned int e53_wg_get_fire_state(LzGpioValue *v);
void e53_wg_led_status_set(SWITCH_STATUS_ENUM status);
void e53_wg_deinit(void);
int wg_detection(void);

#endif/*__E53_WISDOM_GARDENS_H__*/
