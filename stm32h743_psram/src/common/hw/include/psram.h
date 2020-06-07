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

#endif


#ifdef __cplusplus
}
#endif


#endif /* SRC_COMMON_HW_INCLUDE_PSRAM_H_ */
