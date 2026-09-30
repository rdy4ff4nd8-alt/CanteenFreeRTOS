/*
 * Alarm_Task.c
 *
 *  Created on: Sep 24, 2026
 *      Author: zhaoshuai
 */
#include "Alarm_Task.h"

void vAlarmTask(void *pvParameters) {
	while(1) {
		SensorData_typedef s={0};
		MqttData_typedef m={0};
		Data_GetSensor(&s);
		Data_GetMqtt(&m);

		uint8_t blink = (uint8_t)((xTaskGetTickCount() / pdMS_TO_TICKS(1000)) & 1);
		uint8_t active = (xTaskGetTickCount() - m.last_data) < pdMS_TO_TICKS(4000);

		LED_Off(1);
		LED_Off(2);
		LED_Off(3);
		Buzzer_Off();

        uint32_t note = 0;
		static uint8_t mute = 0;
		if(xTaskNotifyWait(0,0xFFFFFFFF,&note,0) == pdTRUE) {
			if(note & 1) mute =! mute;
		}
		if(s.mq2_DO == 0 && s.flame_DO == 0) {
			LED_On(3);
			Buzzer_On();
		} else if(s.mq2_DO == 0 || s.flame_DO == 0) {
			if(blink) {
				LED_On(3);
				Buzzer_On();
			}
		} else mute = 0;
		if(mute) Buzzer_Off();
		if(m.online == 0) LED_On(2);
		else if(m.thresh > 0 && m.count > m.thresh) {
			if(blink) LED_On(2);
		}
		if(m.online == 1 && !(m.thresh > 0 && m.count > m.thresh)) {
			if(active) {
				if(blink) LED_On(1);
			} else if(!(s.mq2_DO == 0 || s.flame_DO == 0)) LED_On(1);
		}
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}
