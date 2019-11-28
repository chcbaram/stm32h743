/*
 * log.h
 *
 *  Created on: 2019. 3. 13.
 *      Author: HanCheol Cho
 */

#ifndef SRC_COMMON_HW_INCLUDE_LOG_H_
#define SRC_COMMON_HW_INCLUDE_LOG_H_


#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_LOG




#define LOG_CH      HW_LOG_CH

bool logInit(void);

void logOn(uint8_t mode);
void logOff(uint8_t mode);
void logSetChannel(uint8_t ch);
void logPrintf(uint8_t mode, const char *fmt, ...);


#endif

#ifdef __cplusplus
}
#endif



#endif /* SRC_COMMON_HW_INCLUDE_LOG_H_ */
