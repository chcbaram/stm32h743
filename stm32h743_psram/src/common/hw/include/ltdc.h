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


 enum class_color {
  white     = 0xFFFF,
  gray      = 0x8410,
  darkgray  = 0xAD55,
  black     = 0x0000,
  purple    = 0x8010,
  pink      = 0xFE19,
  red       = 0xF800,
  orange    = 0xFD20,
  brown     = 0xA145,
  beige     = 0xF7BB,
  yellow    = 0xFFE0,
  lightgreen= 0x9772,
  green     = 0x0400,
  darkblue  = 0x0011,
  blue      = 0x001F,
  lightblue = 0xAEDC,
 };

bool ltdcInit(void);
bool ltdcDrawAvailable(void);
void ltdcEnterActiveArea(void);
void ltdcExitActiveArea(void);
uint32_t ltdcWidth(void);
uint32_t ltdcHeight(void);
uint32_t ltdcGetBufferAddr(uint8_t index);
bool ltdcLayerInit(uint16_t LayerIndex, uint32_t Address);
void ltdcSetAlpha(uint16_t LayerIndex, uint32_t value);

#ifdef __cplusplus
}
#endif




#endif /* SRC_COMMON_HW_INCLUDE_LTDC_H_ */
