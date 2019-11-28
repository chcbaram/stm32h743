/*
 * boot.h
 *
 *  Created on: 2019. 6. 4.
 *      Author: Baram
 */

#ifndef SRC_AP_BOOT_BOOT_H_
#define SRC_AP_BOOT_BOOT_H_

#include "hw.h"





void bootInit(void);
void bootProcessCmd(cmd_t *p_cmd);
void bootJumpToFw(void);
bool bootVerifyFw(void);
bool bootVerifyCrc(void);

#endif /* SRC_AP_BOOT_BOOT_H_ */
