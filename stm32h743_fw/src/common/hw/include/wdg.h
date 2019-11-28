/*
 * wdg.h
 *
 *  Created on: 2019. 3. 19.
 *      Author: HanCheol Cho
 */

#ifndef SRC_COMMON_HW_INCLUDE_WDG_H_
#define SRC_COMMON_HW_INCLUDE_WDG_H_


#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_WDG


void wdgInit(void);
bool wdgStart(uint32_t reload_time_ms);
bool wdgRefresh(void);
bool wdgGetReset(void);

#endif

#ifdef __cplusplus
}
#endif


#endif /* SRC_COMMON_HW_INCLUDE_WDG_H_ */
