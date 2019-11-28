/*
 * boot.h
 *
 *  Created on: 2018. 8. 26.
 *      Author: Baram
 */

#ifndef SRC_AP_BOOT_BOOT_H_
#define SRC_AP_BOOT_BOOT_H_


#include "hw.h"
#include "cmd/cmd.h"
#include "core/uart.h"



bool bootInit(uint8_t channel, char *port_name, uint32_t baud);

uint8_t bootCmdReadBootVersion(uint8_t *p_version);
uint8_t bootCmdReadBootName(uint8_t *p_str);
uint8_t bootCmdReadFirmVersion(uint8_t *p_version);
uint8_t bootCmdReadFirmName(uint8_t *p_str);
uint8_t bootCmdLedOn(uint8_t enable);

uint8_t bootCmdFlashErase(uint32_t addr, uint32_t length);
uint8_t bootCmdFlashWrite(uint32_t addr, uint8_t *p_data, uint32_t length);
uint8_t bootCmdJumpToFw(void);


#endif /* SRC_AP_BOOT_BOOT_H_ */
