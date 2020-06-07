/*
 * battery.h
 *
 *  Created on: 2019. 11. 14.
 *      Author: Baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_BATTERY_H_
#define SRC_COMMON_HW_INCLUDE_BATTERY_H_


#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_BATTERY




bool batteryInit(void);
void batteryUpdate(void);

int32_t batteryGetLevel(void);
int32_t batteryGetVoltage(void);
#endif


#ifdef __cplusplus
}
#endif


#endif /* SRC_COMMON_HW_INCLUDE_BATTERY_H_ */
