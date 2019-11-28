/*
 * delay.h
 *
 *  Created on: 2019. 3. 7.
 *      Author: HanCheol Cho
 */

#ifndef SRC_COMMON_HW_DELAY_H_
#define SRC_COMMON_HW_DELAY_H_


#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_DELAY


bool delayInit();

void delay(uint32_t ms);
void delayNs(uint32_t ns);
void delayUs(uint32_t us);
void delayMs(uint32_t ms);

#endif


#ifdef __cplusplus
}
#endif


#endif /* SRC_COMMON_HW_DELAY_H_ */
