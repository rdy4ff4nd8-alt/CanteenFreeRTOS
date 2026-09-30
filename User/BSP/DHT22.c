/*
 * DHT22.c
 *
 *  Created on: Sep 23, 2026
 *      Author: zhaoshuai
 */
#include "DHT22.h"


/* 等引脚离开 level 这个状态；超过 timeout_us 微秒还没变 → 返回 1（超时） */
static uint8_t DHT22_Wait(GPIO_PinState level, uint16_t timeout_us)
{
    while (HAL_GPIO_ReadPin(DHT22_GPIO_Port, DHT22_Pin) == level)
    {
        if (timeout_us == 0) return 1;   /* 等太久，判定失败 */
        timeout_us--;
        Delay_us(1);
    }
    return 0;                            /* 变了，正常 */
}

DHT22_Data DHT22_Read(void)
{
    DHT22_Data r = {0, 0, 0};          /* ok=0，失败时直接返回它 */
    uint8_t buf[5] = {0};
    uint8_t i, b;

    taskENTER_CRITICAL();              /* ======= 临界区开始，这 5ms 别人别碰 ======= */

    /* ① 起始信号 */
    HAL_GPIO_WritePin(DHT22_GPIO_Port, DHT22_Pin, GPIO_PIN_RESET);
    Delay_us(1200);              /* 拉低 1.2ms */
    HAL_GPIO_WritePin(DHT22_GPIO_Port, DHT22_Pin, GPIO_PIN_SET);  /* 松手 */
    Delay_us(30);

    /* ② 等传感器回应：低 80us → 高 80us */
    if (DHT22_Wait(GPIO_PIN_RESET, 100)) goto FAIL;
    if (DHT22_Wait(GPIO_PIN_SET,   100)) goto FAIL;

    /* ③ 读 40 位 */
    for (i = 0; i < 5; i++)
    {
        for (b = 0; b < 8; b++)
        {
            if (DHT22_Wait(GPIO_PIN_RESET, 100)) goto FAIL;   /* 等 50us 低电平结束 */
            Delay_us(40);                             /* 高电平开始 40us 后采样 */
            buf[i] <<= 1;                                   /* 整体左移，腾出最低位 */
            if (HAL_GPIO_ReadPin(DHT22_GPIO_Port, DHT22_Pin) == GPIO_PIN_SET)
                buf[i] |= 0x01;                             /* 还是高 → 这位是 1 */
            if (DHT22_Wait(GPIO_PIN_SET, 100)) goto FAIL; /* 等这一位结束 */
        }
    }

    taskEXIT_CRITICAL();               /* ======= 临界区结束 ======= */

    /* ⑤ 校验：前 4 字节之和（取低 8 位）== 第 5 字节 */
    if ((uint8_t)(buf[0] + buf[1] + buf[2] + buf[3]) != buf[4])
        return r;                      /* 校验失败，ok=0 */

    /* ⑥ 组装 */
    r.humi_x10 = ((uint16_t)buf[0] << 8) | buf[1];
    uint16_t t = ((uint16_t)buf[2] << 8) | buf[3];
    if (t & 0x8000)  r.temp_x10 = -(int16_t)(t & 0x7FFF);   /* 最高位=1 → 零下 */
    else             r.temp_x10 =  (int16_t)t;
    r.temp_x10 -= 3;                   /* 实测校准 -0.3℃，按你模块实际偏差改 */
    r.ok = 1;
    return r;

FAIL:
    taskEXIT_CRITICAL();               /* 中途失败的出口：先解锁再走 */
    return r;                          /* ok=0 */
}
