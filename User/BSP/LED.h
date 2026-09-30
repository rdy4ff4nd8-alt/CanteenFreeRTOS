/*
 * LED.h
 *
 *  Created on: Sep 22, 2026
 *      Author: zhaoshuai
 */

#ifndef BSP_LED_H_
#define BSP_LED_H_

#include "main.h"

typedef struct {
	GPIO_TypeDef *Port;
	uint16_t      Pin;
}LED_typedef;

#define LED_NUM     3

void LED_On(uint8_t idx);
void LED_Off(uint8_t idx);
void LED_Toggle(uint8_t idx);

#endif /* BSP_LED_H_ */
