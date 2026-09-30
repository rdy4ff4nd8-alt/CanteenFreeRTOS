/*
 * MyADC.c
 *
 *  Created on: Sep 23, 2026
 *      Author: zhaoshuai
 */
#include "MyADC.h"

extern ADC_HandleTypeDef hadc1;

void MyADC_Init(void) {
    HAL_ADCEx_Calibration_Start(&hadc1);  //校准
}

uint16_t MyADC_Read(uint8_t ch) {
	ADC_ChannelConfTypeDef adc_config={0};
	if(ch == 1) adc_config.Channel = ADC_CHANNEL_1;
	else if(ch == 3) adc_config.Channel = ADC_CHANNEL_3;
	else return 0;
	adc_config.Rank = ADC_REGULAR_RANK_1;
	adc_config.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
	HAL_ADC_ConfigChannel(&hadc1, &adc_config);  //告诉ADC测哪个引脚

	HAL_ADC_Start(&hadc1);//开始测

    if(HAL_ADC_PollForConversion(&hadc1, 10) != HAL_OK) {//等测量完成
    	HAL_ADC_Stop(&hadc1);
    	return 0;//超过10ms就返回值0
    }

    uint16_t v = (uint16_t)HAL_ADC_GetValue(&hadc1);//得到0~4095的值

    HAL_ADC_Stop(&hadc1);

    return v;
}

