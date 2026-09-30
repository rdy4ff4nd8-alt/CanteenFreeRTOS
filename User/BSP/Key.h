/*
 * Key.h
 *
 *  Created on: Sep 22, 2026
 *      Author: zhaoshuai
 */

#ifndef BSP_KEY_H_
#define BSP_KEY_H_

#include "main.h"

typedef struct {
	GPIO_TypeDef *Port;
	uint16_t      Pin;
	GPIO_PinState cur;
	GPIO_PinState pre;
	uint8_t       cnt;
}Key_typedef;

#define KEY_NUM 2

uint8_t Key_Scan(void);

#endif /* BSP_KEY_H_ */
