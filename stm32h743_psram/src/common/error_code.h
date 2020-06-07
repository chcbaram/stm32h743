/*
 * error_code.h
 *
 *  Created on: 2019. 3. 7.
 *      Author: HanCheol Cho
 */

#ifndef SRC_COMMON_ERROR_CODE_H_
#define SRC_COMMON_ERROR_CODE_H_



#define OK                            0
#define ERR_MEMORY                    1
#define ERR_FULL                      2
#define ERR_EMPTY                     3
#define ERR_NULL                      4
#define ERR_INVAILD_INDEX             5

#define ERR_I2C_READ                  10
#define ERR_I2C_WRITE                 11


#define ERR_LCD                       50
#define ERR_LCD_TIMEOUT               51
#define ERR_LCD_INVAILD_LAYER         52

#define ERR_TS                        53
#define ERR_TS_TIMEOUT                54
#define ERR_TS_DEV_NOT_FOUND          55

#define ERR_AUDIO                     56
#define ERR_AUDIO_TIMEOUT             57


#define ERR_INVALID_CMD               100
#define ERR_INVALID_LENGTH            239
#define ERR_FLASH_INVALID_CHECK_SUM   240
#define ERR_FLASH_INVALID_TAG         241
#define ERR_INVALID_FW                242
#define ERR_FLASH_ADDR_ALIGN          243
#define ERR_FLASH_INVALID_ADDR        244
#define ERR_FLASH_ERROR               245
#define ERR_FLASH_BUSY                246
#define ERR_FLASH_ERR_TIMEOUT         247
#define ERR_FLASH_NOT_EMPTY           248
#define ERR_FLASH_WRITE               249
#define ERR_FLASH_READ                250
#define ERR_FLASH_ERASE               251
#define ERR_FLASH_PACKET_SIZE         252
#define ERR_FLASH_SIZE                253
#define ERR_FLASH_CRC                 254


#define ERR_MEMORY_ERASE_LENGTH       300
#define ERR_MEMORY_ERASE_ALIGN        301
#define ERR_MEMORY_ERASE_PARAM        302
#define ERR_MEMORY_ERASE_RANGE        304
#define ERR_MEMORY_READ_LENGTH        305
#define ERR_MEMORY_READ_PARAM         306
#define ERR_MEMORY_READ_RANGE         307
#define ERR_MEMORY_WRITE_LENGTH       308
#define ERR_MEMORY_WRITE_PARAM        309
#define ERR_MEMORY_WRITE_ALIGN        310
#define ERR_MEMORY_WRITE_RANGE        311














#endif /* SRC_COMMON_ERROR_CODE_H_ */
