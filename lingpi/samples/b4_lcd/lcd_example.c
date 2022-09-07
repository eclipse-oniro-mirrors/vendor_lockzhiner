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
#include "picture.h"
#include "lcd.h"

/* 任务的堆栈大小 */
#define TASK_STACK_SIZE     20480
/* 任务的优先级 */
#define TASK_PRIO           24

/* 循环等待时间 */
#define WAIT_MSEC           1000

/* 图片数据 */
extern const unsigned char gImage_lingzhi[IMAGE_MAXSIZE_LINGZHI];

/* LCD的位置 */
#define LCD_FILL_X              0
#define LCD_FILL_Y              0
/* 图片的位置，长度和宽度 */
#define LCD_PICTURE_X           15
#define LCD_PICTURE_Y           0
#define LCD_PICTURE_LENGTH      210
#define LCD_PICTURE_WIDTH       62
/* 字符串1的位置，长度和宽度 */
#define LCD_STRING1_X           0
#define LCD_STRING1_Y           130
#define LCD_STRING1_SIZE        16
#define LCD_STRING1_TEXT        "Welcome to XiaoLingPai!"
#define LCD_STRING1_MODE        0
/* 字符串2的位置，长度和宽度 */
#define LCD_STRING2_X           0
#define LCD_STRING2_Y           130
#define LCD_STRING2_SIZE        16
#define LCD_STRING2_TEXT        "URL: http://www.fzlzda.com"
#define LCD_STRING2_MODE        0
/* 字符串3的位置，长度和宽度 */
#define LCD_STRING3_X           0
#define LCD_STRING3_Y           160
#define LCD_STRING3_SIZE        16
#define LCD_STRING3_TEXT        "LCD_W:"
#define LCD_STRING3_MODE        0
/* 数字1int的位置，长度和宽度 */
#define LCD_INT1_X              128
#define LCD_INT1_Y              160
#define LCD_INT1_SIZE           16
#define LCD_INT1_VALUE          3
/* 字符串4的位置，长度和宽度 */
#define LCD_STRING4_X           80
#define LCD_STRING4_Y           160
#define LCD_STRING4_SIZE        16
#define LCD_STRING4_TEXT        "LCD_H:"
#define LCD_STRING4_MODE        0
/* 数字2int的位置，长度和宽度 */
#define LCD_INT2_X              128
#define LCD_INT2_Y              160
#define LCD_INT2_SIZE           16
#define LCD_INT2_VALUE          3
/* 字符串5的位置，长度和宽度 */
#define LCD_STRING5_X           80
#define LCD_STRING5_Y           160
#define LCD_STRING5_SIZE        16
#define LCD_STRING5_TEXT        "LCD_H:"
#define LCD_STRING5_MODE        0
/* 字符串6的位置，长度和宽度 */
#define LCD_STRING6_X           0
#define LCD_STRING6_Y           190
#define LCD_STRING6_SIZE        16
#define LCD_STRING6_TEXT        "Increaseing Num:"
#define LCD_STRING6_MODE        0
/* 数字float的位置，长度和宽度 */
#define LCD_FLOAT1_X            128
#define LCD_FLOAT1_Y            190
#define LCD_FLOAT1_SIZE         16
#define LCD_FLOAT1_NUM          4
#define LCD_FLOAT1_INCREASE     (0.11)
/* 中文数字的位置，长度和宽度 */
#define LCD_CHINESE_X           0
#define LCD_CHINESE_Y           220
#define LCD_CHINESE_MODE        0
/***************************************************************
* 函数名称: lcd_process
* 说    明: lcd例程
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void lcd_process(void *arg)
{
    uint32_t ret = 0;
    float t = 0;
    uint8_t chinese_string[] = "小凌派";
    uint8_t cur_sizey = SIZEY_12;

    ret = lcd_init();
    if (ret != 0) {
        printf("lcd_init failed(%d)\n", ret);
        return;
    }

    lcd_fill(LCD_FILL_X, LCD_FILL_Y, LCD_W, LCD_H, LCD_WHITE);

    while (1) {
        printf("************Lcd Example***********\n");
        lcd_show_picture(LCD_PICTURE_X, LCD_PICTURE_Y, LCD_PICTURE_LENGTH, LCD_PICTURE_WIDTH, &gImage_lingzhi[0]);
        lcd_show_string(LCD_STRING1_X, LCD_STRING1_Y, LCD_STRING1_TEXT, LCD_RED, LCD_WHITE, LCD_STRING1_SIZE, LCD_STRING1_MODE);
        lcd_show_string(LCD_STRING2_X, LCD_STRING2_Y, LCD_STRING2_TEXT, LCD_RED, LCD_WHITE, LCD_STRING2_SIZE, LCD_STRING2_MODE);
        lcd_show_string(LCD_STRING3_X, LCD_STRING3_Y, LCD_STRING3_TEXT, LCD_BLUE, LCD_WHITE, LCD_STRING3_SIZE, LCD_STRING3_MODE);
        lcd_show_int_num(LCD_INT1_X, LCD_INT1_Y, LCD_W, LCD_INT1_VALUE, LCD_BLUE, LCD_WHITE, LCD_INT1_SIZE);
        lcd_show_string(LCD_STRING4_X, LCD_STRING4_Y, LCD_STRING4_TEXT, LCD_BLUE, LCD_WHITE, LCD_STRING4_SIZE, LCD_STRING4_MODE);
        lcd_show_int_num(LCD_INT2_X, LCD_INT2_Y, LCD_H, LCD_INT2_VALUE, LCD_BLUE, LCD_WHITE, LCD_INT2_SIZE);
        lcd_show_string(LCD_STRING5_X, LCD_STRING5_Y, LCD_STRING5_TEXT, LCD_BLUE, LCD_WHITE, LCD_STRING5_SIZE, LCD_STRING5_MODE);
        lcd_show_string(LCD_STRING6_X, LCD_STRING6_Y, LCD_STRING6_TEXT, LCD_BLACK, LCD_WHITE, LCD_STRING6_SIZE, LCD_STRING6_MODE);
        lcd_show_float_num1(LCD_FLOAT1_X, LCD_FLOAT1_Y, t, LCD_FLOAT1_NUM, LCD_BLACK, LCD_WHITE, LCD_FLOAT1_SIZE);
        t += LCD_FLOAT1_INCREASE;

        lcd_fill(LCD_CHINESE_X, LCD_CHINESE_Y, LCD_W, LCD_H, LCD_WHITE);
        lcd_show_chinese(LCD_CHINESE_X, LCD_CHINESE_Y, chinese_string, LCD_RED, LCD_WHITE, cur_sizey, LCD_CHINESE_MODE);
        if (cur_sizey == LCD_FONT_SIZE12) {
            cur_sizey = LCD_FONT_SIZE16;
        } else if (cur_sizey == LCD_FONT_SIZE16) {
            cur_sizey = LCD_FONT_SIZE24;
        } else if (cur_sizey == LCD_FONT_SIZE24) {
            cur_sizey = LCD_FONT_SIZE32;
        } else {
            cur_sizey = LCD_FONT_SIZE12;
        }

        printf("\n");
        LOS_Msleep(WAIT_MSEC);
    }
}


/***************************************************************
* 函数名称: lcd_example
* 说    明: 开机自启动调用函数
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void lcd_example()
{
    unsigned int thread_id;
    TSK_INIT_PARAM_S task = {0};
    unsigned int ret = LOS_OK;

    task.pfnTaskEntry = (TSK_ENTRY_FUNC)lcd_process;
    task.uwStackSize = TASK_STACK_SIZE;
    task.pcName = "lcd process";
    task.usTaskPrio = TASK_PRIO;
    ret = LOS_TaskCreate(&thread_id, &task);
    if (ret != LOS_OK) {
        printf("Falied to create task ret:0x%x\n", ret);
        return;
    }
}


APP_FEATURE_INIT(lcd_example);
