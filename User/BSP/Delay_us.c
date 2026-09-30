/*
 * Delay.c
 *
 *  Created on: Sep 23, 2026
 *      Author: zhaoshuai
 */

#include "Delay_us.h"

void Delay_us_Init(void) {
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;   /* ① 通电 */
	DWT->CYCCNT = 0;                                   /* ② 清零 */
	DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk;              /* ③ 开始走 */
}

void Delay_us(uint32_t us) {
    uint32_t start  = DWT->CYCCNT;                     /* ① 记下当前读数 */
    uint32_t target = us * (SystemCoreClock / 1000000U); /* ② 每微秒 72 格 */

    while ((DWT->CYCCNT - start) < target)
    {
        /* ③ 空转，什么都不干，就等 */
    }
}
