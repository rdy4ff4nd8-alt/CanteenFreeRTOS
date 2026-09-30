/*
 * Data_App.c
 *
 *  Created on: Sep 25, 2026
 *      Author: zhaoshuai
 */

#include "Data_App.h"

static SemaphoreHandle_t xMutexSensor;
static SemaphoreHandle_t xMutexMqtt;
static SensorData_typedef S;
static MqttData_typedef M;

void Data_MutexInit(void) {
	xMutexSensor = xSemaphoreCreateMutex();
	xMutexMqtt =xSemaphoreCreateMutex();
}

void Data_SetSensor(SensorData_typedef *s) {
	if(xSemaphoreTake(xMutexSensor,portMAX_DELAY) == pdTRUE) {
		S = *s;
		xSemaphoreGive(xMutexSensor);
	}
}

void Data_GetSensor(SensorData_typedef *s) {
	if(xSemaphoreTake(xMutexSensor,portMAX_DELAY) == pdTRUE) {
		*s = S;
		xSemaphoreGive(xMutexSensor);
	}
}

void Data_SetMqtt(MqttData_typedef *m) {
	if(xSemaphoreTake(xMutexMqtt,portMAX_DELAY) == pdTRUE) {
		M = *m;
		xSemaphoreGive(xMutexMqtt);
	}
}
void Data_GetMqtt(MqttData_typedef *m) {
	if(xSemaphoreTake(xMutexMqtt,portMAX_DELAY) == pdTRUE) {
		*m = M;
		xSemaphoreGive(xMutexMqtt);
	}
}
