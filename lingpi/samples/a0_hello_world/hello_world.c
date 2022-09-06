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

#include "los_task.h"   // OpenHarmony LiteOS的任务管理头文件

/* 等待时间 */
#define HELLOWORLD_WAIT_MSEC        1000
#define OPENHARMONY_WAIT_MSEC       2000

/* 任务的堆栈大小 */
#define TASK_STACK_SIZE             2048
/* 任务的优先级 */
#define HELLWORLD_TASK_PRIO         24
#define OPENHARMONY_TASK_PRIO       25

/***************************************************************
* 函数名称: task_helloworld
* 说    明: 线程函数helloworld
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void task_helloworld()
{
    while (1) {
        printf("Hello World\n");
        /* 睡眠1秒。该函数为OpenHarmony LiteoS内核睡眠函数，单位为：毫秒 */
        LOS_Msleep(HELLOWORLD_WAIT_MSEC);
    }
}

/***************************************************************
* 函数名称: task_openharmony
* 说    明: 线程函数
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void task_openharmony()
{
    while (1) {
        printf("Hello OpenHarmony\n");
        /* 睡眠2秒。该函数为OpenHarmony内核睡眠函数，单位为：毫秒 */
        LOS_Msleep(OPENHARMONY_WAIT_MSEC);
    }
}

/***************************************************************
* 函数名称: helloworld_example
* 说    明: 内核任务创建例程
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void helloworld_example()
{
    /* 任务id */
    unsigned int thread_id1;
    unsigned int thread_id2;
    /* 任务参数 */
    TSK_INIT_PARAM_S task1 = {0};
    TSK_INIT_PARAM_S task2 = {0};
    /* 返回值 */
    unsigned int ret = LOS_OK;

    /* 创建HelloWorld任务 */
    task1.pfnTaskEntry = (TSK_ENTRY_FUNC)task_helloworld;   // 运行函数入口
    task1.uwStackSize = TASK_STACK_SIZE;                    // 堆栈大小
    task1.pcName = "task_helloworld";                       // 函数注册名称
    task1.usTaskPrio = HELLWORLD_TASK_PRIO;                 // 任务的优先级，从0~63
    ret = LOS_TaskCreate(&thread_id1, &task1);              // 创建任务
    if (ret != LOS_OK) {
        printf("Falied to create task_helloworld ret:0x%x\n", ret);
        return;
    }

    task2.pfnTaskEntry = (TSK_ENTRY_FUNC)task_openharmony;  // 运行函数入口
    task2.uwStackSize = TASK_STACK_SIZE;                    // 堆栈大小
    task2.pcName = "task_openharmony";                      // 函数注册名称
    task2.usTaskPrio = OPENHARMONY_TASK_PRIO;               // 任务的优先级，从0~63
    ret = LOS_TaskCreate(&thread_id2, &task2);              // 创建任务
    if (ret != LOS_OK) {
        printf("Falied to create task_openharmony ret:0x%x\n", ret);
        return;
    }
}

