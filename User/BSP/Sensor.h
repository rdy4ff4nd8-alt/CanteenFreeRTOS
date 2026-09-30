/*
 * Sensor.h
 *
 *  Created on: Sep 23, 2026
 *      Author: zhaoshuai
 */

#ifndef BSP_SENSOR_H_
#define BSP_SENSOR_H_

#include "main.h"

void Sensor_Init(void);
uint8_t MQ2_ReadDO(void);
uint16_t MQ2_ReadAO(void);
uint8_t Flame_ReadDO(void);
uint16_t Flame_ReadAO(void);

#endif /* BSP_SENSOR_H_ */
