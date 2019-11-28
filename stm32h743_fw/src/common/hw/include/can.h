/*
 * can.h
 *
 *  Created on: 2019. 3. 11.
 *      Author: HanCheol Cho
 */

#ifndef SRC_COMMON_HW_INCLUDE_CAN_H_
#define SRC_COMMON_HW_INCLUDE_CAN_H_


#ifdef __cplusplus
 extern "C" {
#endif

#include "hw_def.h"


#ifdef _USE_HW_CAN


#define CAN_MAX_CH            HW_CAN_MAX_CH
#define CAN_MSG_RX_BUF_MAX    HW_CAN_MSG_RX_BUF_MAX
#define CAN_DATA_RX_BUF_MAX   HW_CAN_DATA_RX_BUF_MAX


typedef struct {
  uint32_t id;
  uint32_t length;
  uint8_t  data[8];
  uint8_t  format;
} can_msg_t;


bool canInit(void);
bool canOpen(uint8_t channel, uint32_t baudrate, uint8_t format);
void canClose(uint8_t channel);
bool canConfigFilter(uint8_t filter_num, uint32_t id, uint32_t mask, uint8_t format);

uint32_t canWrite(uint8_t channel, uint32_t id, uint8_t *p_data, uint32_t length, uint32_t timeout);
uint8_t  canRead(uint8_t channel);
uint32_t canAvailable(uint8_t channel);
uint32_t canWriteMsg(uint8_t channel, can_msg_t *p_msg, uint32_t timeout);
bool     canReadMsg(uint8_t channel, can_msg_t *p_msg);
uint32_t canAvailableMsg(uint8_t channel);

uint8_t  canGetErrCount(uint8_t channel);
uint32_t canGetError(uint8_t channel);
uint32_t canGetState(uint8_t channel);

void canAttachRxInterrupt(uint8_t channel, void (*handler)(can_msg_t *arg));
void canDetachRxInterrupt(uint8_t channel);


#endif /* _USE_HW_CAN */

#ifdef __cplusplus
 }
#endif



#endif /* SRC_COMMON_HW_INCLUDE_CAN_H_ */
