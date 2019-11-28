/*
 * dac.h
 *
 *  Created on: 2019. 11. 2.
 *      Author: Baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_DAC_H_
#define SRC_COMMON_HW_INCLUDE_DAC_H_



#ifdef __cplusplus
 extern "C" {
#endif



#include "hw_def.h"

#ifdef _USE_HW_DAC

#define DAC_MAX_CH       HW_DAC_MAX_CH



void dacInit();
void dacSetup(uint32_t hz);
void dacStart(void);
void dacStop(void);
uint32_t dacAvailable(void);
void     dacPutch(uint8_t data);
void     dacWrite(uint8_t *p_data, uint32_t length);

uint32_t dacGetDebug(void);

#endif


#ifdef __cplusplus
}
#endif


#endif /* SRC_COMMON_HW_INCLUDE_DAC_H_ */
