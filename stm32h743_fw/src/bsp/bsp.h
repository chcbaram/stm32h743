/*
 * bsp.h
 *
 *  Created on: 2019. 6. 14.
 *      Author: HanCheol Cho
 */

#ifndef SRC_BSP_BSP_H_
#define SRC_BSP_BSP_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "def.h"
#include "stm32h7xx_hal.h"

#include "cmsis_os.h"


#define logPrintf(...)    printf(__VA_ARGS__)



void bspInit(void);
void bspDeInit(void);

extern void delay(uint32_t ms);
extern uint32_t millis(void);
extern uint32_t micros(void);
extern void Error_Handler(void);

#ifdef __cplusplus
 }
#endif
#endif /* SRC_BSP_BSP_H_ */
