/*
 * MyADC.h
 *
 *  Created on: Sep 23, 2026
 *      Author: zhaoshuai
 */

#ifndef BSP_MYADC_H_
#define BSP_MYADC_H_

#include "main.h"

void MyADC_Init(void);
uint16_t MyADC_Read(uint8_t ch);

#endif /* BSP_MYADC_H_ */
