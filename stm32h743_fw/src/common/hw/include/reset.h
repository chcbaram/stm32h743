/*
 * reset.h
 *
 *  Created on: 2019. 6. 21.
 *      Author: HanCheol Cho
 */

#ifndef SRC_COMMON_HW_INCLUDE_RESET_H_
#define SRC_COMMON_HW_INCLUDE_RESET_H_


#ifdef __cplusplus
 extern "C" {
#endif

#include "hw_def.h"


#ifdef _USE_HW_RESET

void resetInit(void);
void resetLog(void);
void resetSetBits(uint8_t data);
void resetRunSoftReset(void);
void resetClearFlag(void);
uint8_t resetGetStatus(void);
uint8_t resetGetBits(void);

#endif

#ifdef __cplusplus
 }
#endif

#endif /* SRC_COMMON_HW_INCLUDE_RESET_H_ */
