/*
 * UART_Task.c
 *
 *  Created on: Sep 24, 2026
 *      Author: zhaoshuai
 */

#include "UART_Task.h"
#include <string.h>
#include <stdlib.h>

static uint8_t dma_buf[128];
static char rx_buf[128];  //缓冲区
static volatile uint8_t rx_flag = 0;  //为1就是有新帧要解析了

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart,uint16_t Size) {
	if(huart->Instance != USART1) return;
	memcpy(rx_buf, dma_buf, Size);
	rx_buf[Size] = '\0';
	rx_flag = 1;
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, dma_buf, 128);
}
static void Uart_Parse(char *frame) {
	MqttData_typedef m;
	Data_GetMqtt(&m);
	char *p = strstr(frame,"COUNT:");
	if(p != NULL) m.count = (uint16_t)atoi(p+6);
	p = strstr(frame,"THRESH:");
	if(p != NULL) m.thresh = (uint16_t)atoi(p+7);
	p = strstr(frame,"TIME:");
	if(p != NULL)  {
		strncpy(m.time,p+5,16);
		m.time [16] = '\0';
	}
	p = strstr(frame,"PREDICT:");
	if(p != NULL) {
		p+=8;
		uint8_t num = 0;
		while(num<5 && *p >= '0' && *p <= '9') {
			m.predict[num++] = (uint16_t)atoi(p);
			p = strchr(p,',');
			if(p == NULL) break;
			p++;
		}
		m.predict_num = num;
	}
	m.online = 1;
	if (strstr(frame, "COUNT:") != NULL) m.last_data = xTaskGetTickCount();
	Data_SetMqtt(&m);
}
void vUartTask(void *pvParameters) {
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, dma_buf, 128);
	while(1) {
		vTaskDelay(pdMS_TO_TICKS(50));
		if(rx_flag == 1) {
			rx_flag = 0;
			Uart_Parse(rx_buf);
			xEventGroupSetBits(xEventUpdate, 1<<1);  //oled刷新，主要是count和thresh
			xTimerReset(xTimerOffline,0);
		}
	}
}
