/*
 * gpio.h
 *
 *  Created on: 2019. 3. 8.
 *      Author: HanCheol Cho
 */

#ifndef SRC_COMMON_HW_GPIO_H_
#define SRC_COMMON_HW_GPIO_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"

#ifdef _USE_HW_GPIO


#define GPIO_MAX_CH     HW_GPIO_MAX_CH

bool    gpioInit(void);
void    gpioPinMode(uint8_t channel, uint8_t mode);
void    gpioPinWrite(uint8_t channel, uint8_t value);
void    gpioPinToggle(uint8_t channel);
uint8_t gpioPinRead(uint8_t channel);


#endif

#ifdef __cplusplus
}
#endif



#endif /* SRC_COMMON_HW_GPIO_H_ */
