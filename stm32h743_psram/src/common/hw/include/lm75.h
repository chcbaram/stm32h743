/*
 * lm75.h
 *
 *  Created on: 2019. 4. 3.
 *      Author: HanCheol Cho
 */

#ifndef SRC_COMMON_HW_INCLUDE_LM75_H_
#define SRC_COMMON_HW_INCLUDE_LM75_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"

#ifdef _USE_HW_LM75

#define LM75_MAX_CH       HW_LM75_MAX_CH


#define LM75_ADDRESS                (0x48)



bool lm75Init(void);
bool lm75Begin(uint8_t channel);
bool lm75ReadValues(uint8_t channel, int32_t *T);


#endif

#ifdef __cplusplus
}
#endif


#endif /* SRC_COMMON_HW_INCLUDE_LM75_H_ */
