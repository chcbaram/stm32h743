/*
 * error.h
 *
 *  Created on: 2019. 3. 18.
 *      Author: HanCheol Cho
 */

#ifndef SRC_COMMON_HW_INCLUDE_ERROR_H_
#define SRC_COMMON_HW_INCLUDE_ERROR_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"

#ifdef _USE_HW_ERROR


#define ERROR_MAX_CH     HW_ERROR_MAX_CH

err_code_t errorAdd(err_code_t err_code);
void errorClear(void);
err_code_t errorGet(void);

#endif

#ifdef __cplusplus
}
#endif

#endif /* SRC_COMMON_HW_INCLUDE_ERROR_H_ */
