/*
 * i2cmux.h
 *
 *  Created on: 2019. 4. 3.
 *      Author: HanCheol Cho
 */

#ifndef SRC_COMMON_HW_INCLUDE_I2CMUX_H_
#define SRC_COMMON_HW_INCLUDE_I2CMUX_H_


#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_I2CMUX




bool i2cmuxInit(void);
bool i2cmuxBegin(uint8_t i2c_ch, uint32_t freq_khz);
bool i2cmuxEnableCh(uint8_t i2c_ch, uint8_t ch);
bool i2cmuxDisableCh(uint8_t i2c_ch, uint8_t ch);
bool i2cmuxGetCh(uint8_t i2c_ch, uint8_t *p_ch);
uint32_t i2cmuxGetErr(void);

#endif

#ifdef __cplusplus
}
#endif


#endif /* SRC_COMMON_HW_INCLUDE_I2CMUX_H_ */
