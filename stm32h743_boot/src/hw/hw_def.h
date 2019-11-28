/*
 * hw_def.h
 *
 *  Created on: 2019. 6. 14.
 *      Author: HanCheol Cho
 */

#ifndef SRC_HW_HW_DEF_H_
#define SRC_HW_HW_DEF_H_


#include "def.h"
#include "bsp.h"




#define _HW_DEF_RTC_BOOT_RESET      RTC_BKP_DR3
#define _HW_DEF_RTC_BOOT_MODE       RTC_BKP_DR4
#define _HW_DEF_RTC_RESET_SRC       RTC_BKP_DR5



#define _USE_HW_MICROS
#define _USE_HW_MILLIS
#define _USE_HW_DELAY
#define _USE_HW_RESET
#define _USE_HW_QSPI


#define _USE_HW_FLASH
#define      HW_USE_CMDIF_FLASH     1


#define _USE_HW_RTC
#define      HW_USE_CMDIF_RTC       1
#define      HW_RTC_CFG_DATA_1      RTC_BKP_DR1
#define      HW_RTC_CFG_DATA_2      RTC_BKP_DR2


#define _USE_HW_SWTIMER
#define      HW_SWTIMER_MAX_CH      8

#define _USE_HW_LED
#define      HW_LED_MAX_CH          1

#define _USE_HW_UART
#define      HW_UART_MAX_CH         2

#define _USE_HW_I2C
#define      HW_I2C_MAX_CH          1
#define      HW_USE_CMDIF_I2C       1

#define _USE_HW_BUTTON
#define      HW_BUTTON_MAX_CH       2
#define      HW_USE_CMDIF_BUTTON    1

#define _USE_HW_CMDIF
#define      HW_CMDIF_LIST_MAX              16
#define      HW_CMDIF_CMD_STR_MAX           16
#define      HW_CMDIF_CMD_BUF_LENGTH        128

#define _USE_HW_CMD
#define      HW_CMD_MAX_DATA_LENGTH         2048


#define FLASH_ADDR_TAG                0x08040000
#define FLASH_ADDR_FW                 0x08040400

#define FLASH_ADDR_START              0x08040000
#define FLASH_ADDR_END                0x08200000


#define FLASH_ADDR_BOOT_START         0x08000000
#define FLASH_ADDR_BOOTEND            (0x08000000 + 128*1024)


#define SDRAM_ADDR_START              0xD0000000    // 16MB
#define SDRAM_ADDR_MEM_BUF            0xD1000000    // 16MB

#define SDRAM_ADDR_FW                 0xD0000000    // 2MB


#define QSPI_ADDR_START               0x90000000
#define QSPI_FW_TAG                   1024
#define QSPI_FW_SIZE                  (2*1024*1024)
#define QSPI_FW_ADDR(x)               ((x)*QSPI_FW_SIZE + QSPI_ADDR_START)




#endif /* SRC_HW_HW_DEF_H_ */
