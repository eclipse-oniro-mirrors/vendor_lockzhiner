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
#include "e53_wisdom_gardens.h"

/***************************************************************
* 函数名称: e53_wg_thread
* 说    明: 林园模块线程
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void e53_wg_thread(void *args)
{
    unsigned int ret, ret1;
    LzGpioValue val = LZGPIO_LEVEL_HIGH, val_last = LZGPIO_LEVEL_HIGH, val1;

    e53_wg_init(1);
    printf("e53_wg_thread\n");
  
    while (1)
    {
        ret = e53_wg_get_fire_state(&val);
        // val1 = (LzGpioValue)((GPIO0->EXT_PORT)&(1 << GPIO0_PB4));

        // printf("ig:%d-%d,%x-%x\n", val1, val, &GPIO0->EXT_PORT, GPIO0->EXT_PORT);
        if (ret != LZ_HARDWARE_SUCCESS)
        {
            printf("get gpio value failed ret:%d\n", ret);
        }
        if (val_last != val)
        {
            if (val == LZGPIO_LEVEL_LOW)
            {
                e53_wg_buzzer_set_status(ON);
                printf("buzzer on\n");
                // e53_wg_buzzer_set_status(OFF);
                e53_wg_led_status_set(ON);
                printf("Fire alarm buzzer and led on\n");
            }
            else
            {
                e53_wg_buzzer_set_status(OFF);
                e53_wg_led_status_set(OFF);
                printf("Release the fire alarm buzzer and led off\n");
            }
            val_last = val;
        }
        LOS_Msleep(1000);
    }
}

/***************************************************************
* 函数名称: e53_wg_example
* 说    明: 人体感应模块例程
* 参    数: 无
* 返 回 值: 无
***************************************************************/
// void e53_wg_example()
// {
//     unsigned int ret = LOS_OK;
//     unsigned int thread_id;
//     TSK_INIT_PARAM_S task = {0};

//     task.pfnTaskEntry = (TSK_ENTRY_FUNC)e53_wg_thread;
//     task.uwStackSize = 10240;
//     task.pcName = "e53_wg_thread";
//     task.usTaskPrio = 24;
//     ret = LOS_TaskCreate(&thread_id, &task);
//     if (ret != LOS_OK)
//     {
//         printf("Falied to create e53_wg_thread ret:0x%x\n", ret);
//         return;
//     }
// }

// APP_FEATURE_INIT(e53_wg_example);
