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

#include "e53_heart_rate_detection.h"

#define HRD_I2C0                                    0

static I2cBusIo m_hrd_i2c0m2 = {
    .scl =  {.gpio = GPIO0_PA1, .func = MUX_FUNC3, .type = PULL_NONE, .drv = DRIVE_KEEP, .dir = LZGPIO_DIR_KEEP, .val = LZGPIO_LEVEL_KEEP},
    .sda =  {.gpio = GPIO0_PA0, .func = MUX_FUNC3, .type = PULL_NONE, .drv = DRIVE_KEEP, .dir = LZGPIO_DIR_KEEP, .val = LZGPIO_LEVEL_KEEP},
    .id = FUNC_ID_I2C0,
    .mode = FUNC_MODE_M2,
};


void e53_hrd_io_init()
{
    unsigned int ret = LZ_HARDWARE_SUCCESS;

    /*初始化I2C*/
    ret = I2cIoInit(m_hrd_i2c0m2);
    if (ret != LZ_HARDWARE_SUCCESS)
    {
        printf("init I2C I2C0 io fail\n");
        return;
    }
    ret = LzI2cInit(HRD_I2C0, 400000);
    if (ret != LZ_HARDWARE_SUCCESS)
    {
        printf("init I2C I2C0 fail\n");
    }

    LzGpioInit(GPIO0_PA5);
    ret = LzGpioSetDir(GPIO0_PA5, LZGPIO_DIR_IN);
    if (ret != LZ_HARDWARE_SUCCESS)
    {
        printf("GpioSetDir(GPIO0_PA5) failed\n");
        return;
    }
    //ret = GpioRegisterIsrFunc(GPIO0_PA5, GPIO_INT_EDGE_FALLING, func, NULL);
    if (ret != LZ_HARDWARE_SUCCESS)
    {
        printf("GpioRegisterIsrFunc(GPIO0_PA5) failed\n");
        return;
    }
}

/***************************************************************
* 函数名称: Max30102_I2c_Write
* 说    明: max30102写寄存器
* 参    数: Reg:寄存器地址
*          Value:寄存器值
* 返 回 值: 无
***************************************************************/
void Max30102_I2c_Write(unsigned char Reg, unsigned char Value)
{
    unsigned char send_data[2] = {Reg, Value};
    unsigned int sendLen = 2;
    LzI2cWrite(HRD_I2C0, MAX30102_SLAVE_ADDR, send_data, sendLen);
}

/***************************************************************
* 函数名称: Max30102_I2c_Read
* 说    明: max30102读寄存器
* 参    数: Reg:寄存器地址
* 返 回 值: 寄存器值
***************************************************************/
unsigned char Max30102_I2c_Read(unsigned char Reg)
{
    unsigned char value = 0;
    unsigned int status = 0;
    unsigned char  buffer[1] = {Reg};
    unsigned int sendLen = 1;
    unsigned int receive_len;
    unsigned char recv_buffer[1] = {0};

    status = LzI2cWrite(HRD_I2C0, MAX30102_SLAVE_ADDR, buffer, sendLen);
    if (status != 0)
    {
        printf("Error: I2C write status = 0x%x\n", status);
        return status;
    }
    receive_len = 1;
	LzI2cRead(HRD_I2C0, MAX30102_SLAVE_ADDR, &value, receive_len);
    return value;
}

/***************************************************************
* 函数名称: Max30102_I2c_ReadBuffer
* 说    明: max30102读寄存器指定长度值
* 参    数: Reg:寄存器地址
*          pBuffer:数据指针
*          Length:数据长度
* 返 回 值: 状态
***************************************************************/
unsigned char Max30102_I2c_ReadBuffer(unsigned char Reg, unsigned char *pBuffer, unsigned short Length)
{
    unsigned int status = 0;
    unsigned char  buffer[1] = {Reg};

    status = LzI2cWrite(HRD_I2C0, MAX30102_SLAVE_ADDR, buffer, 1);
    if (status != 0)
    {
        printf("Error: I2C write status = 0x%x\n", status);
        return status;
    }

    LzI2cRead(HRD_I2C0, MAX30102_SLAVE_ADDR, pBuffer, Length);
    return 0;  
}

/***************************************************************
* 函数名称: Max30102_Reset
* 说    明: max30102复位
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void Max30102_Reset()
{
    Max30102_I2c_Write(REG_MODE_CONFIG, 0x40);
    Max30102_I2c_Write(REG_MODE_CONFIG, 0x40);
}

/***************************************************************
* 函数名称: Max30102_Init
* 说    明: max30102初始化
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void Max30102_Init()
{
    unsigned char Reg;

    Max30102_Reset();

    LOS_Msleep(100);

    printf("REV_ID:0x%x PART_ID:0x%x\n", Max30102_I2c_Read(REG_REV_ID), Max30102_I2c_Read(REG_PART_ID));

    Max30102_I2c_Write(REG_INTR_ENABLE_1, 0xc0);// INTR setting
    Max30102_I2c_Write(REG_INTR_ENABLE_2, 0x00);
    //Max30102_I2c_Write(REG_TEMP_CONFIG, 0x01);
    Max30102_I2c_Write(REG_FIFO_WR_PTR, 0x00);  //FIFO_WR_PTR[4:0]
    Max30102_I2c_Write(REG_OVF_COUNTER, 0x00);  //OVF_COUNTER[4:0]
    Max30102_I2c_Write(REG_FIFO_RD_PTR, 0x00);  //FIFO_RD_PTR[4:0]
    Max30102_I2c_Write(REG_FIFO_CONFIG, 0x0f);  //sample avg = 1, fifo rollover=false, fifo almost full = 17
    Max30102_I2c_Write(REG_MODE_CONFIG, 0x03);  //0x02 for Red only, 0x03 for SpO2 mode 0x07 multimode LED
    Max30102_I2c_Write(REG_SPO2_CONFIG, 0x27);  // SPO2_ADC range = 4096nA, SPO2 sample rate (100 Hz), LED pulseWidth (400uS)  
    Max30102_I2c_Write(REG_LED1_PA, 0x24);      //Choose value for ~ 7mA for LED1
    Max30102_I2c_Write(REG_LED2_PA, 0x24);      // Choose value for ~ 7mA for LED2
    Max30102_I2c_Write(REG_PILOT_PA, 0x7f);     // Choose value for ~ 25mA for Pilot LED
}

void e53_hrd_init()
{
    e53_hrd_io_init();
    Max30102_Init();
}
