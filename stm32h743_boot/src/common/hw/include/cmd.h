/*
 * cmd.h
 *
 *  Created on: 2019. 6. 3.
 *      Author: Baram
 */

#ifndef SRC_HW_DRIVER_CMD_H_
#define SRC_HW_DRIVER_CMD_H_



#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_CMD



#define CMD_MAX_DATA_LENGTH       HW_CMD_MAX_DATA_LENGTH


typedef struct
{
  uint8_t   cmd;
  uint8_t   option;
  uint8_t   error;
  uint16_t  length;
  uint8_t   check_sum;
  uint8_t   check_sum_recv;
  uint8_t   buffer[CMD_MAX_DATA_LENGTH];
  uint8_t   *data;
} cmd_packet_t;


typedef struct
{
  uint8_t   ch;
  bool      is_init;
  uint32_t  baud;
  uint8_t   state;
  uint32_t  pre_time;
  uint32_t  index;

  cmd_packet_t  rx_packet;
  cmd_packet_t  tx_packet;
} cmd_t;


void cmdInit(cmd_t *p_cmd);
bool cmdBegin(cmd_t *p_cmd, uint8_t ch, uint32_t baud);
bool cmdReceivePacket(cmd_t *p_cmd);
void cmdSendCmd(cmd_t *p_cmd, uint8_t cmd, uint8_t *p_data, uint32_t length);
void cmdSendResp(cmd_t *p_cmd, uint8_t err_code, uint8_t *p_data, uint32_t length);
bool cmdSendCmdRxResp(cmd_t *p_cmd, uint8_t cmd, uint8_t *p_data, uint32_t length, uint32_t timeout);

#endif

#ifdef __cplusplus
}
#endif

#endif /* SRC_HW_DRIVER_CMD_H_ */
