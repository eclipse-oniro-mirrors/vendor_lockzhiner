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
#include "los_task.h"
#include "ohos_init.h"
#include "lz_hardware.h"

/* 任务的堆栈大小 */
#define TASK_STACK_SIZE         20480
/* 任务的优先级 */
#define TASK_PRIO               24

/* 循环等待时间 */
#define WAIT_MSEC               1000

/* 字符串最大长度 */
#define STRING_MAXSIZE          32

/* OLED显示字符串1的位置、长度和宽度 */
#define OLED_STRING1_X          6
#define OLED_STRING1_Y          0
#define OLED_STRING1_TEXT       "0.96' OLED TEST"
#define OLED_STRING1_SIZE       16
/* OLED显示字符串2的位置、长度和宽度 */
#define OLED_STRING2_X          0
#define OLED_STRING2_Y          3
#define OLED_STRING2_TEXT       "ASCII:"
#define OLED_STRING2_SIZE       16
/* OLED显示字符串3的位置、长度和宽度 */
#define OLED_STRING3_X          64
#define OLED_STRING3_Y          3
#define OLED_STRING3_TEXT       "CODE:"
#define OLED_STRING3_SIZE       16
/* OLED显示字符串4的位置、长度和宽度 */
#define OLED_STRING4_X          40
#define OLED_STRING4_Y          6
#define OLED_STRING4_SIZE       16

void oled_process(void *arg)
{
    unsigned char buffer[STRING_MAXSIZE];
    int i = 0;

    oled_init();
    oled_clear();

    while (1) {
        printf("========= Oled Process =============\n");
        oled_show_string(OLED_STRING1_X, OLED_STRING1_Y, OLED_STRING1_TEXT, OLED_STRING1_SIZE);
        oled_show_string(OLED_STRING2_X, OLED_STRING2_Y, OLED_STRING2_TEXT, OLED_STRING2_SIZE);
        oled_show_string(OLED_STRING3_X, OLED_STRING3_Y, OLED_STRING3_TEXT, OLED_STRING3_SIZE);

        snprintf(buffer, sizeof(buffer), "%d Sec!", i++);
        oled_show_string(OLED_STRING4_X, OLED_STRING4_Y, buffer, OLED_STRING4_SIZE);

        printf("\n\n");
        LOS_Msleep(WAIT_MSEC);
    }
}


/***************************************************************
* 函数名称: oled_example
* 说    明: 开机自启动调用函数
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void oled_example(void)
{
    unsigned int thread_id;
    TSK_INIT_PARAM_S task = {0};
    unsigned int ret = LOS_OK;

    task.pfnTaskEntry = (TSK_ENTRY_FUNC)oled_process;
    task.uwStackSize = TASK_STACK_SIZE;
    task.pcName = "oled process";
    task.usTaskPrio = TASK_PRIO;
    ret = LOS_TaskCreate(&thread_id, &task);
    if (ret != LOS_OK) {
        printf("Falied to create task ret:0x%x\n", ret);
        return;
    }
}


APP_FEATURE_INIT(oled_example);
