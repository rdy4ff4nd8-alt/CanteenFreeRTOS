/*
 * Display_Task.c
 *
 *  Created on: Sep 25, 2026
 *      Author: zhaoshuai
 */
#include "Display_Task.h"
#include <string.h>

void vDisplayTask(void *pvParameters) {
	uint8_t page = 1;
	static uint8_t	drawn = 0;
	//last是让变值才重画
	uint16_t last_cnt = 0xFFFF, last_lim = 0xFFFF;
	char     last_time[17] = "";
	uint16_t last_pred[5] = {0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF};

	int16_t  last_temp = -32768;
	uint16_t last_humi = 0xFFFF;
	uint16_t last_mq2 = 0xFFFF, last_flame = 0xFFFF;
	uint8_t  last_dht = 0xFF;

	while(1) {
		xEventGroupWaitBits(xEventUpdate, (1<<0) | (1<<1), pdTRUE, pdFALSE, pdMS_TO_TICKS(50));
		if(xSemaphoreTake(xSemPage,0) == pdTRUE) {
			page =! page;
			OLED_Clear();
			drawn = 0;
			last_cnt = 0xFFFF;  last_lim = 0xFFFF;
			last_time[0] = '\0';
			last_pred[0] = 0xFFFF; last_pred[1] = 0xFFFF;
			last_pred[2] = 0xFFFF; last_pred[3] = 0xFFFF;
			last_temp = -32768; last_humi = 0xFFFF;
			last_mq2 = 0xFFFF;  last_flame = 0xFFFF;
			last_dht = 0xFF;
		}

		if(page == 1) {
			if(drawn != 1) {
				OLED_Clear();
				OLED_ShowString(1, 1, "Temp:");
				OLED_ShowString(2, 1, "Humi:");
				OLED_ShowString(3, 1, "Smoke:");
				OLED_ShowString(4, 1, "Flame:");
				drawn = 1;
			}

			SensorData_typedef s;
			Data_GetSensor(&s);
			if(s.dht_ok == 1) {
				if(s.temp_x10 != last_temp) {
					if(s.temp_x10 < 0) {
						OLED_ShowChar(1, 6, '-');
						s.temp_x10 = -s.temp_x10;
					}
					else OLED_ShowChar(1, 6, ' ');

					OLED_ShowNum(1, 7, s.temp_x10 / 10 , 2);
					OLED_ShowChar(1, 9, '.');
					OLED_ShowNum(1, 10, s.temp_x10 % 10 , 1);
					OLED_ShowChar(1, 11, 'C');

					last_temp = s.temp_x10;
				}

				if(s.humi_x10 != last_humi) {
					OLED_ShowChar(2, 6, ' ');
					OLED_ShowNum(2, 7, s.humi_x10 / 10 , 2);
					OLED_ShowChar(2, 9, '.');
					OLED_ShowNum(2, 10, s.humi_x10 % 10 , 1);
					OLED_ShowChar(2, 11, '%');

					last_humi = s.humi_x10;
				}

			}
			else if(last_dht != 0){
				last_dht = 0;
				OLED_ShowString(1, 1, "Temp:---.-C");
				OLED_ShowString(2, 1, "Humi:---.-%");
			}
			if(s.dht_ok != last_dht) {
				last_dht = s.dht_ok;
				if(s.dht_ok == 1) {               //重插DHT22后重画
					last_temp = -32768;
					last_humi = 0xFFFF;
				}
			}

			uint8_t fast = (uint8_t)((xTaskGetTickCount() / pdMS_TO_TICKS(200)) & 1);
			if(s.mq2_AO != last_mq2) {
				OLED_ShowNum(3, 7, s.mq2_AO, 4);
				last_mq2 = s.mq2_AO;
			}
			if(s.mq2_DO == 0) {
				if(fast)
					OLED_ShowString(3,13,"WARN");
				else
					OLED_ShowString(3,13,"    ");
			}
			else OLED_ShowString(3,13,"    ");

			if(s.flame_AO != last_flame) {
				OLED_ShowNum(4,7,s.flame_AO,4);
				last_flame = s.flame_AO;
			}
			if(s.flame_DO == 0) {
				if(fast)
					OLED_ShowString(4,13,"WARN");
				else
					OLED_ShowString(4,13,"    ");
			}
			else OLED_ShowString(4,13,"    ");
		}

		if(page == 0) {
			if(drawn != 2) {
				OLED_Clear();
				OLED_ShowString(1,1,"Count:");
				OLED_ShowString(2,1,"Thresh:");
				OLED_ShowString(3,1,"T:");
				OLED_ShowString(4,1,"Pred:");
				drawn = 2;
			}
			MqttData_typedef m;
			Data_GetMqtt(&m);

			if(m.count != last_cnt) {
				OLED_ShowNum(1, 7, m.count, 4);
				last_cnt = m.count;
			}
			if(m.thresh != last_lim) {
				OLED_ShowNum(2, 8, m.thresh, 4);
				last_lim = m.thresh;
			}
			if(strncmp(m.time, last_time, 17) != 0) {
				if(m.time[0] != '\0')
					OLED_ShowString(3, 3, m.time + 2);
				else
					OLED_ShowString(3, 3,"-------- --:--");
				strncpy(last_time, m.time, 17);
			}
			for(uint8_t i = 0;i < 4; i++) {
				uint16_t v;
				if(i < m.predict_num) v = m.predict[i];
				else v = 0xFFFF;
				if(v != last_pred[i]) {
					if(i < m.predict_num) OLED_ShowNum(4, 6 + i*3, v, 2);
					else OLED_ShowString(4, 6 + i*3, "  ");
					last_pred[i] = v;
				}
			}
		}
	}
}
