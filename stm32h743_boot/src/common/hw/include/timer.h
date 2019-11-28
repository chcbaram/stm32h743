/*
 * timer.h
 *
 *  Created on: 2019. 3. 11.
 *      Author: HanCheol Cho
 */

#ifndef SRC_COMMON_HW_INCLUDE_TIMER_H_
#define SRC_COMMON_HW_INCLUDE_TIMER_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"

#ifdef _USE_HW_TIMER

#define TIMER_MAX_CH       HW_TIMER_MAX_CH


bool timerInit(void);
void timerStop(uint8_t channel);
void timerSetPeriod(uint8_t channel, uint32_t period_data);
void timerAttachInterrupt(uint8_t channel, voidFuncPtr handler);
void timerDetachInterrupt(uint8_t channel);
void timerStart(uint8_t channel);
void timerAddPeriod(uint8_t channel, int32_t period_data);

#endif

#ifdef __cplusplus
}
#endif



#endif /* SRC_COMMON_HW_INCLUDE_TIMER_H_ */
