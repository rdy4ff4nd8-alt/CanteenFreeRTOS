/*
 * Sensor.c
 *
 *  Created on: Sep 23, 2026
 *      Author: zhaoshuai
 */

#include "Sensor.h"
#include "MyADC.h"

void Sensor_Init(void) {
	MyADC_Init();
}
uint8_t MQ2_ReadDO(void) {
	return HAL_GPIO_ReadPin(MQ2DO_GPIO_Port, MQ2DO_Pin);
}
uint16_t MQ2_ReadAO(void) {
	return MyADC_Read(1);
}
uint8_t Flame_ReadDO(void) {
	return HAL_GPIO_ReadPin(FIREDO_GPIO_Port, FIREDO_Pin);
}
uint16_t Flame_ReadAO(void) {
	return MyADC_Read(3);
}
