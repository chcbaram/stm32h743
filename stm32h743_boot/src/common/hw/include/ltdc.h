/*
 * ltdc.h
 *
 *  Created on: 2019. 6. 23.
 *      Author: Baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_LTDC_H_
#define SRC_COMMON_HW_INCLUDE_LTDC_H_


#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"


bool ltdcInit(void);
bool ltdcDrawAvailable(void);
void ltdcEnterActiveArea(void);
void ltdcExitActiveArea(void);
uint32_t ltdcWidth(void);
uint32_t ltdcHeight(void);
uint32_t ltdcGetBufferAddr(uint8_t index);
bool ltdcLayerInit(uint16_t LayerIndex, uint32_t Address);

#ifdef __cplusplus
}
#endif




#endif /* SRC_COMMON_HW_INCLUDE_LTDC_H_ */
