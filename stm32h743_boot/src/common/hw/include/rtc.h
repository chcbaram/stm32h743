/*
 * rtc.h
 *
 *  Created on: 2019. 6. 21.
 *      Author: HanCheol Cho
 */

#ifndef SRC_COMMON_HW_INCLUDE_RTC_H_
#define SRC_COMMON_HW_INCLUDE_RTC_H_


#ifdef __cplusplus
 extern "C" {
#endif

#include "hw_def.h"


#ifdef _USE_HW_RTC


#define RTC_CFG_DATA_1    HW_RTC_CFG_DATA_1
#define RTC_CFG_DATA_2    HW_RTC_CFG_DATA_2


bool rtcInit(void);
bool rtcIsInit(void);

time_t rtcGetTime();
void rtcSetTime(time_t time_data);
void rtcWriteBackupData(uint32_t index, uint32_t data);
uint32_t rtcReadBackupData(uint32_t index);
uint32_t rtcGetSecondsFromPower(void);
uint32_t rtcGetSecondsFromReset(void);

#endif

#ifdef __cplusplus
 }
#endif


#endif /* SRC_COMMON_HW_INCLUDE_RTC_H_ */
