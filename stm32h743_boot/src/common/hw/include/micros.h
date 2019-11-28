/*
 * micros.h
 *
 *  Created on: 2019. 3. 7.
 *      Author: HanCheol Cho
 */

#ifndef SRC_COMMON_HW_MICROS_H_
#define SRC_COMMON_HW_MICROS_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"

#ifdef _USE_HW_MICROS


bool microsInit(void);
uint32_t micros(void);


#endif


#ifdef __cplusplus
}
#endif


#endif /* SRC_COMMON_HW_MICROS_H_ */
