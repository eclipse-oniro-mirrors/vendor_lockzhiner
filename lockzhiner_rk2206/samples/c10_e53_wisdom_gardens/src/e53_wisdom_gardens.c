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

#include "e53_wisdom_gardens.h"
#define  EEPROM_ADDR          0              //保存地址

#define  EEPROM_HEAD_DATA     0x4c5a000a     //Eeprom数据头
#define  DEFUALT_SN_DATA      1              //用于判断默认参数是否保存至eeprom， 当此数值改变时则保存
#define  DEFUALT_TIME_DATA1   3           

/***************************************************************
* 名    称: wg_eeprom_data_s
* 说    明：Eeprom数据结构体
***************************************************************/
typedef struct
{
    uint32_t            head;               //数据头   0x4c5a0003
    uint16_t            sn;                 //用于判断是否需要保存默认参数 当次数与已保存的不一致时保存默认参数
    uint8_t             time;               //时间
    uint8_t             reserve[35];        //保留
    uint16_t            crc;                //crc校验
} __attribute__((packed)) wg_eeprom_data_s;

static wg_eeprom_data_s m_wg_eeprom_data =
{
    .head          = EEPROM_HEAD_DATA,
    .sn            = DEFUALT_SN_DATA,
    .time          = DEFUALT_TIME_DATA1,
};

/***************************************************************
* 函数名称: wg_get_eeprom_data
* 说    明: 读取eeprom数据
* 参    数: wg_eeprom_data_s *data
* 返 回 值: 0 成功，1 失败
***************************************************************/
static uint32_t wg_get_eeprom_data(wg_eeprom_data_s *data)
{
    wg_eeprom_data_s   temp = {0};
    
    if (eeprom_read(EEPROM_ADDR, (unsigned char*)&temp, sizeof(wg_eeprom_data_s)) == sizeof(wg_eeprom_data_s))
    {
        /*crc16校验*/
        if (calc_crc16((uint8_t*)&temp, sizeof(wg_eeprom_data_s) - 2) == temp.crc)
        {
            *data = temp;
            return 0;
        }
        else
        {
            printf("%s crc err Calc:0x%04x, read:0x%04x\n", __FUNCTION__, calc_crc16((uint8_t*)&temp, sizeof(wg_eeprom_data_s) - 2), temp.crc);
        }
    }
    else
    {
        printf("%s read eeprom data error read\n", __FUNCTION__);
    }
    
    return 1;
}

/***************************************************************
* 函数名称: wg_set_eeprom_data
* 说    明: 保存eeprom数据
* 参    数: wg_eeprom_data_s *data
* 返 回 值: 0 成功，1 失败
***************************************************************/
static uint32_t wg_set_eeprom_data(wg_eeprom_data_s *data)
{
    uint32_t wlen = 0;
    uint32_t rlen = 0;
    uint16_t crc  = 0;
    wg_eeprom_data_s temp = {0};
    /*计算crc*/
    data->crc = calc_crc16((uint8_t*)data, sizeof(wg_eeprom_data_s) - 2);
    wlen = eeprom_write(EEPROM_ADDR, (unsigned char*)data, sizeof(wg_eeprom_data_s));
    if (wlen == sizeof(wg_eeprom_data_s))
    {
        /*读出已写的数据进行校验是否写成功*/
        rlen = eeprom_read(EEPROM_ADDR, (unsigned char*)&temp, sizeof(wg_eeprom_data_s));
        if (rlen == sizeof(wg_eeprom_data_s))
        {
            crc = calc_crc16((uint8_t*)&temp, sizeof(wg_eeprom_data_s) - 2);
            if (crc == temp.crc)
            {
                return 0;
            }
            else
            {
                printf("%s crc err write:0x%04x, Calc:0x%04x, read:0x%04x\n", __FUNCTION__ , data->crc, crc, temp.crc);
            }
        }
        else
        {
            printf(" %s read eeprom data error read len:%d, len:%d\n", __FUNCTION__ , rlen, sizeof(wg_eeprom_data_s));
        }
    }
    else
    {
        printf("%s write eeprom data error write len:%d, len:%d\n", __FUNCTION__ , wlen, sizeof(wg_eeprom_data_s));
    }
    
    return 1;
}

/***************************************************************
* 函数名称: wg_save_default_param
* 说    明: 保存默认的ig数据
* 参    数: void
* 返 回 值: 0 成功，1 失败
***************************************************************/
static uint32_t wg_save_default_param(void)
{
    wg_eeprom_data_s temp = {0};
    /*读取数据*/
    if (wg_get_eeprom_data(&temp) == 0)
    {
        /*已保存的数据与默认参数一致就不重复保存*/
        if (memcmp(&temp, &m_wg_eeprom_data, sizeof(wg_eeprom_data_s)) == 0)
        {
            return 0;
        }
        /*一致不保存默认参数*/
        if (temp.head == m_wg_eeprom_data.head && temp.sn == m_wg_eeprom_data.sn)
        {
            return 0;
        }
        /*更新sn*/
        m_wg_eeprom_data.sn = temp.sn;
    }
    else
    {
        m_wg_eeprom_data.sn = DEFUALT_SN_DATA;
    }
    
    m_wg_eeprom_data.head  = EEPROM_HEAD_DATA;
    m_wg_eeprom_data.time  = DEFUALT_TIME_DATA1;
    m_wg_eeprom_data.crc   = calc_crc16((uint8_t*)&m_wg_eeprom_data, sizeof(wg_eeprom_data_s) - 2);
    
    /*保存默认参数*/
    if (wg_set_eeprom_data(&m_wg_eeprom_data) != 0)
    {
        printf("%s err\n", __FUNCTION__);
        return 1;
    }
    
    return 0;
}

/***************************************************************
* 函数名称: wg_param_init
* 说    明: 初始化ig数据
* 参    数: uint32_t args
* 返 回 值: 0 成功，1 失败
***************************************************************/
static uint32_t wg_param_init(uint32_t args)
{
    /*判断是否要保存默认参数*/
    if(args)
    {
        // printf("%s:%d\n", __FUNCTION__, __LINE__);
        wg_save_default_param();
    }
    /*先从eeprom读数据*/
    if (wg_get_eeprom_data(&m_wg_eeprom_data) != 0)
    {
        /*读失败时防止保存的参数是异常值*/
        if (m_wg_eeprom_data.time > 60)
        {
            m_wg_eeprom_data.time = DEFUALT_TIME_DATA1;
        }
        return 1;
    }
    printf("\n\neeprom data:%08x %04x %02x %04x\n\n", m_wg_eeprom_data.head, m_wg_eeprom_data.sn, m_wg_eeprom_data.time, m_wg_eeprom_data.crc);
           
    /*读成功时防止保存的参数是异常值*/
    if (m_wg_eeprom_data.time > 60)
    {
        m_wg_eeprom_data.time = DEFUALT_TIME_DATA1;
    }
    
    return  0;
}

//检测模块是否存在
int wg_detection(void)
{
    wg_eeprom_data_s temp = {0};

    if (wg_get_eeprom_data(&temp) == 0)
    {
        if(temp.head == m_wg_eeprom_data.head) return 0;
    }

    return -1;
}






#define IG_I2C0                             0 
#define IG_PWM7                             7

static I2cBusIo m_wg_i2c0m2 = {
    .scl =  {.gpio = GPIO0_PA1, .func = MUX_FUNC3, .type = PULL_NONE, .drv = DRIVE_KEEP, .dir = LZGPIO_DIR_KEEP, .val = LZGPIO_LEVEL_KEEP},
    .sda =  {.gpio = GPIO0_PA0, .func = MUX_FUNC3, .type = PULL_NONE, .drv = DRIVE_KEEP, .dir = LZGPIO_DIR_KEEP, .val = LZGPIO_LEVEL_KEEP},
    .id = FUNC_ID_I2C0,
    .mode = FUNC_MODE_M2,
};

static PwmBusIo m_buzzer  = {
    .pwm = {.gpio = GPIO1_PD0, .func = MUX_FUNC1, .type = PULL_DOWN, .drv = MUX_FUNC7, .dir = LZGPIO_DIR_KEEP, .val = LZGPIO_LEVEL_KEEP},
    .id = FUNC_ID_PWM7,
    .mode = FUNC_MODE_NONE,
};

//io配置
static Pinctrl m_led  = {.gpio = GPIO0_PB0, .func = MUX_GPIO, .type = PULL_DOWN, .drv = DRIVE_KEEP, .dir = LZGPIO_DIR_OUT, .val = LZGPIO_LEVEL_HIGH};
static Pinctrl m_fire = {.gpio = GPIO0_PB4, .func = MUX_GPIO, .type = PULL_DOWN, .drv = DRIVE_KEEP, .dir = LZGPIO_DIR_IN, .val = LZGPIO_LEVEL_LOW};
//GPIO0_PB4
/***************************************************************
* 函数名称: e53_wg_init
* 说    明: E53_BI初始化
* 参    数: uint32_t args
* 返 回 值: 无
***************************************************************/
void e53_wg_init(uint32_t args)
{
    uint32_t ret = LZ_HARDWARE_SUCCESS;

    eeprom_init();
    wg_param_init(args);

    ret = PwmIoInit(m_buzzer);
    if (ret != LZ_HARDWARE_SUCCESS)
    {
        printf("PwmIoInit failed ret:%d\n", ret);
        // return;
    }

    ret = LzPwmInit(IG_PWM7);
    if (ret != LZ_HARDWARE_SUCCESS)
    {
        printf("LzPwmInit 7 failed ret:%d\n", ret);
        // return;
    }

    if (PinctrlInit(m_led) != 0)
    {
        LzGpioDeinit(m_led.gpio);
        if (PinctrlInit(m_led) != 0)
        {
            printf("PinctrlInit failed id:%d\n", m_led.gpio);
            // return;
        }
    }

    if (PinctrlInit(m_fire) != 0)
    {
        LzGpioDeinit(m_fire.gpio);
        if (PinctrlInit(m_fire) != 0)
        {
            printf("PinctrlInit failed id:%d\n", m_fire.gpio);
            // return;
        }
    }
}

void e53_wg_deinit(void)
{
    e53_wg_led_status_set(ON);
    e53_wg_buzzer_set_status(OFF);
    LzGpioDeinit(m_led.gpio);
    LzGpioDeinit(m_fire.gpio);
    LzI2cDeinit(IG_I2C0);
    LzGpioDeinit(m_wg_i2c0m2.scl.gpio);
    LzGpioDeinit(m_wg_i2c0m2.sda.gpio);
}

unsigned int e53_wg_get_fire_state(LzGpioValue *v)
{
    return LzGpioGetVal(m_fire.gpio, v);
}

/***************************************************************
* 函数名称: e53_wg_buzzer_set_status
* 说    明: 蜂鸣器控制
* 参    数: OFF,关蜂鸣器
*          ON,开蜂鸣器
* 返 回 值: 无
***************************************************************/
void e53_wg_buzzer_set_status(SWITCH_STATUS_ENUM status)
{
    if(status == ON)
    {                                
        LzPwmStart(IG_PWM7, 1000000/2, 1000000);
    }
    if(status == OFF)
    {
        LzPwmStop(IG_PWM7);
    }
}

/***************************************************************
* 函数名称: e53_wg_led_status_set
* 说    明: led控制
* 参    数: OFF,关led
*          ON,开led
* 返 回 值: 无
***************************************************************/
void e53_wg_led_status_set(SWITCH_STATUS_ENUM status)
{
    if (status == ON)
    {
        LzGpioSetVal(m_led.gpio, LZGPIO_LEVEL_LOW);//设置GPIO输出低电平打开led
    }
    if (status == OFF)
    {
        LzGpioSetVal(m_led.gpio, LZGPIO_LEVEL_HIGH);//设置GPIO输出高电平关闭led
    }
}


