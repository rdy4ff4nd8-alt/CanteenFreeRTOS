/*
 * Sensor_Task.c
 *
 *  Created on: Sep 23, 2026
 *      Author: zhaoshuai
 */

#include "Sensor_Task.h"
#include <stdio.h>
#include <string.h>

void vSensorTask(void *pvParameters) {
	TickType_t last = xTaskGetTickCount();
	uint8_t dht22 = 0;
	SensorData_typedef S = {0};
	while(1) {
		vTaskDelayUntil(&last,pdMS_TO_TICKS(1000));

		S.mq2_AO = MQ2_ReadAO();
		S.mq2_DO = MQ2_ReadDO();
		S.flame_AO = Flame_ReadAO();
		S.flame_DO = Flame_ReadDO();

		dht22 ++;
		if(dht22 > 1) {
			DHT22_Data D = DHT22_Read();
			if(D.ok) {
				S.temp_x10 = D.temp_x10;
				S.humi_x10 = D.humi_x10;
			}
			S.dht_ok = D.ok;
			dht22 = 0;
		}
		Data_SetSensor(&S);
		xEventGroupSetBits(xEventUpdate, 1<<0);  //oled刷新sensor的

		if(dht22 == 0) {
			static char sensor[64];
			sprintf(sensor,"$MQ2:%d|FLAME:%d|TEMP:%d|HUMI:%d#\n",
					S.mq2_AO,
					S.flame_DO,
					S.temp_x10,
					S.humi_x10);
			HAL_UART_Transmit_IT(&huart1, (uint8_t *)sensor, strlen(sensor));
		}
	}
}
