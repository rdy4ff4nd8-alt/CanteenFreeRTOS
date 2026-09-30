/*
 * LED.c
 *
 *  Created on: Sep 22, 2026
 *      Author: zhaoshuai
 */
#include "LED.h"

static LED_typedef LED[LED_NUM] = {
		{LED1_GPIO_Port,LED1_Pin},
		{LED2_GPIO_Port,LED2_Pin},
		{LED3_GPIO_Port,LED3_Pin}
};

void LED_On(uint8_t idx) {
	if (idx > LED_NUM) return;
	HAL_GPIO_WritePin(LED[idx-1].Port, LED[idx-1].Pin, GPIO_PIN_SET);
}

void LED_Off(uint8_t idx) {
	if (idx > LED_NUM) return;
	HAL_GPIO_WritePin(LED[idx-1].Port, LED[idx-1].Pin, GPIO_PIN_RESET);
}

void LED_Toggle(uint8_t idx) {
	if (idx > LED_NUM) return;
	HAL_GPIO_TogglePin(LED[idx-1].Port, LED[idx-1].Pin);
}
