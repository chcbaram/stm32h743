/*
 * ws2812.h
 *
 *  Created on: 2019. 5. 3.
 *      Author: HanCheol Cho
 */

#ifndef SRC_HW_DRIVER_WS2812_H_
#define SRC_HW_DRIVER_WS2812_H_


#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"


#ifdef _USE_HW_WS2812

#define WS2812_MAX_CH     HW_WS2812_MAX_CH


bool ws2812Init(void);
void ws2812SetColor(uint8_t ch, uint8_t red, uint8_t green, uint8_t blue);

#endif

#ifdef __cplusplus
}
#endif


#endif /* SRC_HW_DRIVER_WS2812_H_ */
