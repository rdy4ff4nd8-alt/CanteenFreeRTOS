/*
 * Data_App.h
 *
 *  Created on: Sep 25, 2026
 *      Author: zhaoshuai
 */

#ifndef APP_DATA_APP_H_
#define APP_DATA_APP_H_

#include "main.h"

typedef struct {
	uint16_t mq2_AO;      /* MQ2 模拟量 0~4095 */
	uint16_t flame_AO;    /* 火焰模拟量 0~4095 */
	uint8_t  mq2_DO;       /* 1=正常 0=超标 */
	uint8_t  flame_DO;     /* 1=正常 0=报警 */
	int16_t  temp_x10;     /* 温度×10 */
	uint16_t humi_x10;     /* 湿度×10 */
	uint8_t  dht_ok;       /* DHT22 读成功标志 */
}SensorData_typedef;

typedef struct {
	uint16_t count;       /* 当前人数 */
	uint16_t thresh;      /* 人数阈值 */
	uint8_t  online;      /* 后端在线状态 */
	uint32_t last_data;
	char     time[17];    /* 后端时间戳 "2026-09-26 21:04"（16字符+结尾符） */
	uint16_t predict[5];  /* 预测值，帧里最多 5 个 */
	uint8_t  predict_num; /* 实际收到几个 */
}MqttData_typedef;

void Data_MutexInit(void);
void Data_SetSensor(SensorData_typedef *s);
void Data_GetSensor(SensorData_typedef *s);
void Data_SetMqtt(MqttData_typedef *m);
void Data_GetMqtt(MqttData_typedef *m);

#endif /* APP_DATA_APP_H_ */
