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

#include "ohos_init.h"
#include "los_task.h"
#include "e53_heart_rate_detection.h"

#define MAX_BUFFER_SIZE                 500
#define EVENT_MASK                      0x00000001U


void e53_hrd_thread()
{
    uint8_t buffer[6];
    uint32_t i;
    uint32_t buffer_ir[MAX_BUFFER_SIZE];//IR LED sensor data
    uint32_t buffer_red[MAX_BUFFER_SIZE];//Red LED sensor data
    uint32_t tmp_ir, tmp_red;
    int32_t spo2; //SPO2 value
    int8_t spo2_valid;   //indicator to show if the SP02 calculation is valid
    int32_t heart_rate;   //heart rate value
    int8_t  hr_valid;    //indicator to show if the heart rate calculation is valid
    uint32_t invalid_data = 0, invalid_data_num = 0, valid_data_num = 0;
    uint8_t intr_status1, intr_status2;
    uint32_t gpio_val = 1, ret = LZ_HARDWARE_SUCCESS;
    uint64_t sum_ir, sum_red;

    e53_hrd_init();

    //GpioEnableIsr(GPIO0_PA5);
    //Max30102_I2c_Read(REG_INTR_STATUS_1);
    intr_status1 = Max30102_I2c_Read(REG_INTR_STATUS_1);
    intr_status2 = Max30102_I2c_Read(REG_INTR_STATUS_2);
    printf("0x%x 0x%x\n", intr_status1, intr_status2);
    memset(buffer_ir, 0, MAX_BUFFER_SIZE);
    memset(buffer_red, 0, MAX_BUFFER_SIZE);

    while(1)
    {
        //printf("wait...");
        sum_ir = 0;
        sum_red = 0;
        for(i = 100; i < MAX_BUFFER_SIZE; i++)
        {
            buffer_red[i - 100] = buffer_red[i];
            buffer_ir[i - 100] = buffer_ir[i];
            sum_ir += buffer_ir[i - 100];
            sum_red += buffer_red[i - 100];
        }

        for (i = 400; i < MAX_BUFFER_SIZE; i++)
        {
            while(1)
            {
                ret = LzGpioGetVal(GPIO0_PA5, &gpio_val);
                //printf("ret:%u val:%u", ret, gpio_val);
                if (gpio_val == 0)
                    break;
            }

            intr_status1 = Max30102_I2c_Read(REG_INTR_STATUS_1);
            intr_status2 = Max30102_I2c_Read(REG_INTR_STATUS_2);

            Max30102_I2c_ReadBuffer(REG_FIFO_DATA, buffer, 6);
            /*Combine values to get the actual number*/
            buffer_red[i] = (((long)buffer[0] & 0x03)<<16) | (long)buffer[1]<<8 | (long)buffer[2];
            buffer_ir[i] = (((long)buffer[3] & 0x03)<<16) | (long)buffer[4]<<8 | (long)buffer[5];

            //buffer_ir[i] = tmp_ir;
            //buffer_red[i] = tmp_red;
            //printf("  %u\n", buffer_ir[i]);

            sum_ir += buffer_ir[i];
            sum_red += buffer_red[i];
            //LOG_INFO("%3u ir 0x%x 0x%2x 0x%2x 0x%2x red 0x%x 0x%2x 0x%2x 0x%2x", valid_data_num, tmp_ir, buffer[0], buffer[1], buffer[2], tmp_red, buffer[3], buffer[4], buffer[5]);
        }

        maxim_heart_rate_and_oxygen_saturation(buffer_ir, MAX_BUFFER_SIZE, buffer_red, &spo2, &spo2_valid, &heart_rate, &hr_valid);
        //if (spo2_valid && hr_valid && heart_rate < 120)
        {
            printf("%s spo2:%d %s heart rate:%d\n", spo2_valid?"valid":"invalid", spo2, hr_valid?"valid":"invalid", heart_rate);
        }
    }
}

void e53_hrd_example()
{
    unsigned int ret = LOS_OK;
    unsigned int thread_id;
    TSK_INIT_PARAM_S task = {0};

    task.pfnTaskEntry = (TSK_ENTRY_FUNC)e53_hrd_thread;
    task.uwStackSize = 10240;
    task.pcName = "e53_hrd_thread";
    task.usTaskPrio = 5;
    ret = LOS_TaskCreate(&thread_id, &task);
    if (ret != LOS_OK)
    {
        printf("Falied to create e53_hrd_thread ret:0x%x\n", ret);
        return;
    }
}

APP_FEATURE_INIT(e53_hrd_example);

