/*
 * Buzzer.c
 *
 *  Created on: Sep 22, 2026
 *      Author: zhaoshuai
 */
#include "Buzzer.h"

void Buzzer_On(void) {
	HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
}

void Buzzer_Off(void) {
	HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
}

void Buzzer_Toggle(void) {
	HAL_GPIO_TogglePin(BUZZER_GPIO_Port, BUZZER_Pin);
}
