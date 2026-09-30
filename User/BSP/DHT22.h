/*
 * DHT22.h
 *
 *  Created on: Sep 23, 2026
 *      Author: zhaoshuai
 */

#ifndef BSP_DHT22_H_
#define BSP_DHT22_H_

#include "main.h"
#include "Delay_us.h"

typedef struct {
    uint8_t  ok;          /* 1 = 这次读成功了 */
    uint16_t humi_x10;    /* 湿度×10，如 605 = 60.5% */
    int16_t  temp_x10;    /* 温度×10，如 256 = 25.6℃，可为负 */
} DHT22_Data;

DHT22_Data DHT22_Read(void);

#endif /* BSP_DHT22_H_ */
