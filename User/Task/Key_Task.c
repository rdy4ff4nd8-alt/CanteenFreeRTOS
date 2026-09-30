/*
 * Key_Task.c
 *
 *  Created on: Sep 23, 2026
 *      Author: zhaoshuai
 */

#include "Key_Task.h"

void vKeyTask(void *pvParameters) {
	TickType_t last = xTaskGetTickCount();
	while(1) {
		vTaskDelayUntil(&last,pdMS_TO_TICKS(10));

		uint8_t k = Key_Scan();
		if(k == 1) {
			xSemaphoreGive(xSemPage);
		}
		if(k == 2) {
			xTaskNotify(xTaskAlarmHandle,1,eSetBits);
		}
		HAL_IWDG_Refresh(&hiwdg);
	}
}
