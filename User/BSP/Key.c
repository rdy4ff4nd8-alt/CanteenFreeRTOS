/*
 * Key.c
 *
 *  Created on: Sep 22, 2026
 *      Author: zhaoshuai
 */

#include "Key.h"

static Key_typedef Key[KEY_NUM]={
		{KEY1_GPIO_Port,KEY1_Pin,GPIO_PIN_SET,GPIO_PIN_SET,0},
		{KEY2_GPIO_Port,KEY2_Pin,GPIO_PIN_SET,GPIO_PIN_SET,0}
};

uint8_t Key_Scan(void) {
	uint8_t r = 0;
	for(uint8_t i=0;i<KEY_NUM;i++) {
		Key[i].cur = HAL_GPIO_ReadPin(Key[i].Port, Key[i].Pin);
		if(Key[i].cur == Key[i].pre)
			Key[i].cnt = 0;
		else {
			Key[i].cnt++;
			if(Key[i].cnt > 1) {
				if(Key[i].cur == GPIO_PIN_RESET)
					r = i+1;
				Key[i].pre = Key[i].cur;
				Key[i].cnt = 0;
			}
		}
	}
	return r;
}
