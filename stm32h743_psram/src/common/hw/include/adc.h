/*
 * adc.h
 *
 *  Created on: 2019. 5. 17.
 *      Author: HanCheol Cho
 */

#ifndef SRC_COMMON_HW_INCLUDE_ADC_H_
#define SRC_COMMON_HW_INCLUDE_ADC_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "hw_def.h"

#ifdef _USE_HW_ADC


#define ADC_MAX_CH    HW_ADC_MAX_CH

bool     adcInit(void);
uint32_t adcRead(uint8_t ch);
uint32_t adcRead8(uint8_t ch);
uint32_t adcRead10(uint8_t ch);
uint32_t adcRead12(uint8_t ch);
uint32_t adcRead16(uint8_t ch);
uint32_t adcReadVoltage(uint8_t ch);
uint32_t adcReadCurrent(uint8_t ch);
uint32_t adcConvVoltage(uint8_t ch, uint32_t adc_value);
uint32_t adcConvCurrent(uint8_t ch, uint32_t adc_value);

uint8_t  adcGetRes(uint8_t ch);


#endif

#ifdef __cplusplus
}
#endif

#endif /* SRC_COMMON_HW_INCLUDE_ADC_H_ */
