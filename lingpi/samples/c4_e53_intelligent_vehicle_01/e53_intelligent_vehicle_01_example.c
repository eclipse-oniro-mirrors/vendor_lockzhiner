/*
 * Copyright (c) 2022 FuZhou Lockzhiner Electronic Co., Ltd. All rights reserved.
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
#include "lz_hardware.h"
#include "ohos_init.h"
#include "los_task.h"
#include "e53_intelligent_vehicle_01.h"

/* 任务的堆栈大小 */
#define TASK_STACK_SIZE         20480
/* 任务的优先级 */
#define TASK_PRIO               24

/* 循环等待时间 */
#define WAIT_MSEC               2000

/* PWM配置，周期时间，单位为纳秒 */
#define PWM_CYCLE_NS            1000000
/* PWM配置，高电平时间，单位为纳秒 */
#define PWM_DUTY_NS             500000
/* PWM配置，开启PWM */
#define PWM_ON                  1
/* PWM配置，关闭PWM */
#define PWM_OFF                 0

/* LED配置，打开LED */
#define LED_ON                  1
/* LED配置，关闭LED */
#define LED_OFF                 0

/* 最大报警距离 */
#define DISTANCE_MAX_ALARM      (20.0)

/***************************************************************
* 函数名称: e53_iv01_process
* 说    明: 智慧农业例程
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void e53_iv01_process(void)
{
    unsigned int ret = 0;
    /* 每个周期为200usec，占空比为100usec */
    unsigned int duty_ns = PWM_DUTY_NS;
    unsigned int cycle_ns = PWM_CYCLE_NS;
    float distance_cm = 0.0;

    e53_iv01_init();

    while (1) {
        printf("========== E53 IV Example ==========\n");

        ret = e53_iv01_get_distance(&distance_cm);
        if (ret == 1) {
            printf("distance cm: %f\n", distance_cm);

            if (distance_cm <= DISTANCE_MAX_ALARM) {
                e53_iv01_buzzer_set(PWM_ON, duty_ns, cycle_ns);
                e53_iv01_led_warning_set(LED_ON);
            } else {
                e53_iv01_buzzer_set(PWM_OFF, duty_ns, cycle_ns);
                e53_iv01_led_warning_set(LED_OFF);
            }
        }

        LOS_Msleep(WAIT_MSEC);
    }
}

/***************************************************************
* 函数名称: e53_iv01_example
* 说    明: 智慧农业开机启动任务
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void e53_iv01_example(void)
{
    unsigned int ret = 0;
    unsigned int thread_id;
    TSK_INIT_PARAM_S task = {0};

    task.pfnTaskEntry = (TSK_ENTRY_FUNC)e53_iv01_process;
    task.uwStackSize = TASK_STACK_SIZE;
    task.pcName = "e53_iv01_process";
    task.usTaskPrio = TASK_PRIO;
    ret = LOS_TaskCreate(&thread_id, &task);
    if (ret != LOS_OK) {
        printf("Falied to create e53_ia_thread ret:0x%x\n", ret);
        return;
    }
}


APP_FEATURE_INIT(e53_iv01_example);
