/*
 * psram.h
 *
 *  Created on: 2020. 5. 30.
 *      Author: Baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_PSRAM_H_
#define SRC_COMMON_HW_INCLUDE_PSRAM_H_



#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_PSRAM


bool psramInit(void);

uint32_t psramGetAddr(void);
uint32_t psramGetLength(void);
bool psramRead(uint32_t addr, uint8_t *p_data, uint32_t length);
bool psramWrite(uint32_t addr, uint8_t *p_data, uint32_t length);

#endif


#ifdef __cplusplus
}
#endif


#endif /* SRC_COMMON_HW_INCLUDE_PSRAM_H_ */
