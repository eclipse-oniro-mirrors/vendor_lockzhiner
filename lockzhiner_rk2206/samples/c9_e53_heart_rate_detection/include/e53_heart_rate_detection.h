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
#ifndef __E53_HEART_RATE_DETECTION_H__
#define __E53_HEART_RATE_DETECTION_H__

#include "lz_hardware.h"
#include "algorithm.h"

#define MAX30102_SLAVE_ADDR             0x57
#define MAX30102_WRITE_ADDR             0xAE
#define MAX30102_READ_ADDR              0xAF

#define REG_INTR_STATUS_1               0x00
#define REG_INTR_STATUS_2               0x01
#define REG_INTR_ENABLE_1               0x02
#define REG_INTR_ENABLE_2               0x03
#define REG_FIFO_WR_PTR                 0x04
#define REG_OVF_COUNTER                 0x05
#define REG_FIFO_RD_PTR                 0x06
#define REG_FIFO_DATA                   0x07
#define REG_FIFO_CONFIG                 0x08
#define REG_MODE_CONFIG                 0x09
#define REG_SPO2_CONFIG                 0x0A
#define REG_LED1_PA                     0x0C
#define REG_LED2_PA                     0x0D
#define REG_PILOT_PA                    0x10
#define REG_MULTI_LED_CTRL1             0x11
#define REG_MULTI_LED_CTRL2             0x12
#define REG_TEMP_INTR                   0x1F
#define REG_TEMP_FRAC                   0x20
#define REG_TEMP_CONFIG                 0x21
#define REG_PROX_INT_THRESH             0x30
#define REG_REV_ID                      0xFE
#define REG_PART_ID                     0xFF

void e53_hrd_init();

#endif/*__E53_HEART_RATE_DETECTION_H__*/

