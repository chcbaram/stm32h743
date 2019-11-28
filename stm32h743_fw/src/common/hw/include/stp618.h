/*
 * stp618.h
 *
 *  Created on: 2019. 3. 13.
 *      Author: HanCheol Cho
 */

#ifndef SRC_COMMON_HW_INCLUDE_STP618_H_
#define SRC_COMMON_HW_INCLUDE_STP618_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"


#ifdef _USE_HW_STP618

#define STP618_MAX_CH       HW_STP618_MAX_CH


typedef struct
{
  uint16_t tof;
  uint8_t  peak;
  uint16_t rt;
  uint8_t  noise;
  uint8_t  warn_level;
  uint16_t warn_distance;

  int32_t  distance_cm;

  uint8_t param[8];
} stp618_msg_t;


bool stp618Init(void);
bool stp618Start(uint8_t channel);
bool stp618StartByBit(uint8_t channel_bit);
bool stp618StartAll(void);
bool stp618ReadMsg(uint8_t channel, stp618_msg_t *p_msg, uint32_t timeout);
bool stp618ReadWarnMsg(uint8_t channel, stp618_msg_t *p_msg, uint32_t timeout);
bool stp618ReadParam(uint8_t channel, stp618_msg_t *p_msg, uint8_t pci, uint32_t timeout);
bool stp618WriteParam(uint8_t channel, uint8_t addr, uint8_t data, uint32_t timeout);

err_code_t stp618GetErrCode(void);
err_code_t stp618Daigonose(uint8_t level, uint8_t channel);
#endif

#ifdef __cplusplus
}
#endif


#endif /* SRC_COMMON_HW_INCLUDE_STP618_H_ */
