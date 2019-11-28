/*
 * hw.h
 *
 *  Created on: 2019. 6. 14.
 *      Author: HanCheol Cho
 */

#ifndef SRC_HW_HW_H_
#define SRC_HW_HW_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "hw_def.h"


#include "micros.h"
#include "millis.h"
#include "delay.h"
#include "led.h"
#include "uart.h"
#include "cmd.h"
#include "cmdif.h"
#include "i2c.h"
#include "reset.h"
#include "rtc.h"
#include "qspi.h"
#include "swtimer.h"
#include "button.h"
#include "pwm.h"
#include "flash.h"
#include "usb.h"
#include "vcp.h"


void hwInit(void);


uint8_t hwGetResetCount(void);
void hwJumpToFw(uint32_t addr);


#ifdef __cplusplus
 }
#endif

#endif /* SRC_HW_HW_H_ */
