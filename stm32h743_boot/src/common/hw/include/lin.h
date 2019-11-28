/*
 * lin.h
 *
 *  Created on: 2019. 3. 12.
 *      Author: HanCheol Cho
 */

#ifndef SRC_HW_DRIVER_LIN_H_
#define SRC_HW_DRIVER_LIN_H_


#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_LIN

#define LIN_MAX_CH       HW_LIN_MAX_CH


typedef struct
{
  uint8_t pid;
  uint8_t length;
  uint8_t data[8];
  uint8_t chksum;
} lin_msg_t;


bool linInit(void);
bool linSend(uint8_t channel, lin_msg_t *p_send_msg);
bool linSendResp(uint8_t channel, lin_msg_t *p_send_msg, lin_msg_t *p_resp_msg, uint32_t timeout);


#endif


#ifdef __cplusplus
}
#endif


#endif /* SRC_HW_DRIVER_LIN_H_ */
