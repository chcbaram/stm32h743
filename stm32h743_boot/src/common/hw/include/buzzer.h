/*
 * buzzer.h
 *
 *  Created on: 2019. 3. 11.
 *      Author: HanCheol Cho
 */

#ifndef SRC_COMMON_HW_INCLUDE_BUZZER_H_
#define SRC_COMMON_HW_INCLUDE_BUZZER_H_


#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_BUZZER


#define BUZZER_TIMER    HW_BUZZER_TIMER

bool buzzerInit(void);

void buzzerOn(uint32_t freq, uint32_t time);
void buzzerOff(void);

void buzzerAddPeriod(int32_t period);

void buzzerStop(void);
void buzzerStart(void);

#endif


#ifdef __cplusplus
}
#endif



#endif /* SRC_COMMON_HW_INCLUDE_BUZZER_H_ */
