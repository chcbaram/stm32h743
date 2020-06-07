/*
 * i2c.h
 *
 *  Created on: 2019. 3. 21.
 *      Author: HanCheol Cho
 */

#ifndef SRC_COMMON_HW_INCLUDE_I2C_H_
#define SRC_COMMON_HW_INCLUDE_I2C_H_


#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_I2C

#define I2C_MAX_CH       HW_I2C_MAX_CH


  bool i2cInit(void);
  bool i2cBegin(uint8_t ch, uint32_t freq_khz);
  void i2cReset(uint8_t ch);
  bool i2cRecovery(uint8_t ch);
  bool i2cReadByte (uint8_t ch, uint16_t dev_addr, uint16_t reg_addr, uint8_t *p_data, uint32_t timeout);
  bool i2cReadBytes(uint8_t ch, uint16_t dev_addr, uint16_t reg_addr, uint8_t *p_data, uint32_t length, uint32_t timeout);
  bool i2cRead16Byte (uint8_t ch, uint16_t dev_addr, uint16_t reg_addr, uint8_t *p_data, uint32_t timeout);
  bool i2cRead16Bytes(uint8_t ch, uint16_t dev_addr, uint16_t reg_addr, uint8_t *p_data, uint32_t length, uint32_t timeout);

  bool i2cWriteByte (uint8_t ch, uint16_t dev_addr, uint16_t reg_addr, uint8_t data, uint32_t timeout);
  bool i2cWriteBytes(uint8_t ch, uint16_t dev_addr, uint16_t reg_addr, uint8_t *p_data, uint32_t length, uint32_t timeout);
  bool i2cWrite16Byte(uint8_t ch, uint16_t dev_addr, uint16_t reg_addr, uint8_t data, uint32_t timeout);
  bool i2cWrite16Bytes(uint8_t ch, uint16_t dev_addr, uint16_t reg_addr, uint8_t *p_data, uint32_t length, uint32_t timeout);

  bool i2cReadData(uint8_t ch, uint16_t dev_addr, uint8_t *p_data, uint32_t length, uint32_t timeout);
  bool i2cWriteData(uint8_t ch, uint16_t dev_addr, uint8_t *p_data, uint32_t length, uint32_t timeout);

  bool i2cReadBytesTF(uint8_t ch, uint16_t dev_addr, uint16_t reg_addr, uint8_t *p_data, uint32_t length, uint32_t timeout);
  bool i2cWriteBytesTF(uint8_t ch, uint16_t dev_addr, uint16_t reg_addr, uint8_t *p_data, uint32_t length, uint32_t timeout);

  void     i2cSetTimeout(uint8_t ch, uint32_t timeout);
  uint32_t i2cGetTimeout(uint8_t ch);

  void     i2cClearErrCount(uint8_t ch);
  uint32_t i2cGetErrCount(uint8_t ch);


#endif


#ifdef __cplusplus
}
#endif


#endif /* SRC_COMMON_HW_INCLUDE_I2C_H_ */
