/*
 * def.h
 *
 *  Created on: 2019. 3. 7.
 *      Author: HanCheol Cho
 */

#ifndef SRC_COMMON_DEF_H_
#define SRC_COMMON_DEF_H_


#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "error_code.h"


#ifndef BOOL
#define BOOL uint8_t
#endif

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif



#define _DEF_CH1                    0
#define _DEF_CH2                    1
#define _DEF_CH3                    2
#define _DEF_CH4                    3
#define _DEF_CH5                    4
#define _DEF_CH6                    5
#define _DEF_CH7                    6
#define _DEF_CH8                    7


#define _DEF_RESET_POWER            0
#define _DEF_RESET_PIN              1
#define _DEF_RESET_WDG              2
#define _DEF_RESET_SOFT             3



#define _DEF_DIAG_LEVEL1            0
#define _DEF_DIAG_LEVEL2            1
#define _DEF_DIAG_LEVEL3            2
#define _DEF_DIAG_LEVEL4            3
#define _DEF_DIAG_LEVEL5            4


#define _DEF_STATE_IDLE             0
#define _DEF_STATE_READY_ENTER      1
#define _DEF_STATE_READY            2
#define _DEF_STATE_FAULT_ENTER      3
#define _DEF_STATE_FAULT            4
#define _DEF_STATE_FAULT_EXIT       5
#define _DEF_STATE_NOT_CONNECT      6


#define _DEF_LOG_ERR                0
#define _DEF_LOG_WARN               1
#define _DEF_LOG_INFO               2
#define _DEF_LOG_DIAG               3
#define _DEF_LOG_NONE               4


#define _DEF_HIGH                   1
#define _DEF_LOW                    0


#define _DEF_LEFT                   0
#define _DEF_RIGHT                  1


#define _DEF_INPUT                  0
#define _DEF_INPUT_PULLUP           1
#define _DEF_INPUT_PULLDOWN         2
#define _DEF_OUTPUT                 3
#define _DEF_OUTPUT_PULLUP          4
#define _DEF_OUTPUT_PULLDOWN        5


#define _DEF_LED1                   0
#define _DEF_LED2                   1
#define _DEF_LED3                   2
#define _DEF_LED4                   3
#define _DEF_LED5                   4


#define _DEF_BUTTON1                0
#define _DEF_BUTTON2                1
#define _DEF_BUTTON3                2
#define _DEF_BUTTON4                3


#define _DEF_TIMER1                 0
#define _DEF_TIMER2                 1
#define _DEF_TIMER3                 2
#define _DEF_TIMER4                 3


#define _DEF_UART1                  0
#define _DEF_UART2                  1
#define _DEF_UART3                  2
#define _DEF_UART4                  3
#define _DEF_UART5                  4
#define _DEF_UART6                  5
#define _DEF_UART7                  6
#define _DEF_UART8                  7
#define _DEF_UART9                  8
#define _DEF_UART10                 9


#define _DEF_SPI1                   0
#define _DEF_SPI2                   1
#define _DEF_SPI3                   2
#define _DEF_SPI4                   3


#define _DEF_I2C1                   0
#define _DEF_I2C2                   1
#define _DEF_I2C3                   2
#define _DEF_I2C4                   3


#define _DEF_DXL1                   0
#define _DEF_DXL2                   1
#define _DEF_DXL3                   2
#define _DEF_DXL4                   3
#define _DEF_DXL5                   4


#define _DEF_TYPE_S08               0
#define _DEF_TYPE_U08               1
#define _DEF_TYPE_S16               2
#define _DEF_TYPE_U16               3
#define _DEF_TYPE_S32               4
#define _DEF_TYPE_U32               5
#define _DEF_TYPE_F32               6


#define _DEF_DXL_BAUD_9600          0
#define _DEF_DXL_BAUD_57600         1
#define _DEF_DXL_BAUD_115200        2
#define _DEF_DXL_BAUD_1000000       3
#define _DEF_DXL_BAUD_2000000       4
#define _DEF_DXL_BAUD_3000000       5
#define _DEF_DXL_BAUD_4000000       6
#define _DEF_DXL_BAUD_4500000       7


#define _DEF_CAN1                   0
#define _DEF_CAN2                   1

#define _DEF_CAN_BAUD_125K          0
#define _DEF_CAN_BAUD_250K          1
#define _DEF_CAN_BAUD_500K          2
#define _DEF_CAN_BAUD_1000K         3

#define _DEF_CAN_STD                0
#define _DEF_CAN_EXT                1


#define _DEF_LIN1                   0
#define _DEF_LIN2                   1

#define _DEF_CH1                    0
#define _DEF_CH2                    1
#define _DEF_CH3                    2
#define _DEF_CH4                    3


typedef uint32_t  err_code_t;




typedef void (*voidFuncPtr)(void);



typedef union
{
  uint8_t  u8Data[4];
  uint16_t u16Data[2];
  uint32_t u32Data;

  int8_t   s8Data[4];
  int16_t  s16Data[2];
  int32_t  s32Data;

  uint8_t  u8D;
  uint16_t u16D;
  uint32_t u32D;

  int8_t   s8D;
  int16_t  s16D;
  int32_t  s32D;
} data_t;


typedef struct
{
  data_t data;
  bool   ret;
} data_ret_t;


typedef struct
{
  uint32_t  ptr_in;
  uint32_t  ptr_out;
  uint32_t  length;
  uint8_t  *p_buf;
} ring_buf_t;


typedef struct
{
  uint32_t addr;
  uint32_t end;
  uint32_t length;
} flash_attr_t;


typedef struct
{
  uint32_t magic_number;

  //-- fw info
  //
  uint8_t  version_str[32];
  uint8_t  board_str  [32];
  uint8_t  name_str   [32];
  uint8_t  date_str   [32];
  uint8_t  time_str   [32];
  uint32_t addr_tag;
  uint32_t addr_fw;
  uint32_t reserved   [32];

  //-- tag info
  //
  uint32_t tag_flash_type;
  uint32_t tag_flash_start;
  uint32_t tag_flash_end;
  uint32_t tag_flash_length;
  uint32_t tag_flash_crc;
  uint32_t tag_length;
  uint8_t  tag_date_str[32];
  uint8_t  tag_time_str[32];
} flash_tag_t;



#define FLASH_MAGIC_NUMBER      0x5555AAAA


#define PI              3.1415926535897932384626433832795
#define HALF_PI         1.5707963267948966192313216916398
#define TWO_PI          6.283185307179586476925286766559
#define DEG_TO_RAD      0.017453292519943295769236907684886
#define RAD_TO_DEG      57.295779513082320876798154814105
#define EULER           2.718281828459045235360287471352

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef map
#define map(value, in_min, in_max, out_min, out_max) ((value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)
#endif

#ifndef byte
#define byte uint8_t
#endif

#ifndef radians
#define radians(deg) ((deg)*DEG_TO_RAD)
#endif
#ifndef degress
#define degrees(rad) ((rad)*RAD_TO_DEG)
#endif
#ifndef sq
#define sq(x) ((x)*(x))
#endif



#endif /* SRC_COMMON_DEF_H_ */
