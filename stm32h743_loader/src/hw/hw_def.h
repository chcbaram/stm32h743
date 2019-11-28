/*
 * hw_def.h
 *
 *  Created on: 2018. 8. 25.
 *      Author: Baram
 */

#ifndef SRC_HW_HW_DEF_H_
#define SRC_HW_HW_DEF_H_


#include "def.h"
#include "bsp.h"


#define log_printf(fmt, ...)        printf(fmt, __VA_ARGS__)


#define FLASH_TAG_ADDR_START        0x08008000
#define FLASH_VER_ADDR_START        0x08008800

#define FLASH_FW_SIZE               (222*1024)
#define FLASH_FW_ADDR_START         0x08008400
#define FLASH_FW_ADDR_END           (FLASH_FW_ADDR_START + FLASH_FW_SIZE)

#define FLASH_TX_BLOCK_LENGTH       (512)



#endif /* SRC_HW_HW_DEF_H_ */
