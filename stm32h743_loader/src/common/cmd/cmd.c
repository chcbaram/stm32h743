/*
 *  cmd.c
 *
 *  Created on: 2016. 7. 13.
 *      Author: Baram
 */

#include "cmd.h"
#include "hw.h"


#define CMD_STX                   0x02
#define CMD_ETX                   0x03

#define CMD_STATE_WAIT_STX          0
#define CMD_STATE_WAIT_CMD          1
#define CMD_STATE_WAIT_OPTION_ERROR 2
#define CMD_STATE_WAIT_ERROR        3
#define CMD_STATE_WAIT_LENGTH_L     4
#define CMD_STATE_WAIT_LENGTH_H     5
#define CMD_STATE_WAIT_DATA         6
#define CMD_STATE_WAIT_CHECKSUM     7
#define CMD_STATE_WAIT_ETX          8



uint8_t cmd_tx_buffer[16*1024];



static void cmdPutch(uint8_t ch, uint8_t data);





void cmdInit(cmd_t *p_cmd)
{
  p_cmd->init  = false;
  p_cmd->state = CMD_STATE_WAIT_STX;

  p_cmd->rx_packet.error = 0;
  p_cmd->tx_packet.error = 0;
}

bool cmdBegin(cmd_t *p_cmd, uint8_t ch, uint32_t baud)
{
  p_cmd->ch   = ch;
  p_cmd->baud = baud;
  p_cmd->init = true;
  p_cmd->state = CMD_STATE_WAIT_STX;

  p_cmd->save_time[0] = millis();
  p_cmd->save_time[1] = millis();

  return uartOpen(ch, baud);
}

void cmdPutch(uint8_t ch, uint8_t data)
{
  uartPutch(ch, data);
}

void cmdWrite(uint8_t ch, uint8_t *p_data, uint32_t length)
{
  uartWrite(ch, p_data, length);
}

bool cmdReceivePacket(cmd_t *p_cmd)
{
  bool     ret = false;
  uint8_t  ch;
  uint32_t index;


  //-- 명령어 수신
  //
  if( uartAvailable(p_cmd->ch) )
  {
    ch = uartRead(p_cmd->ch);
  }
  else
  {
    return false;
  }


  //-- 바이트간 타임아웃 설정(500ms)
  //
  if((millis()-p_cmd->save_time[0]) > 500)
  {
    p_cmd->state        = CMD_STATE_WAIT_STX;
  }
  p_cmd->save_time[0] = millis();


  //-- 명령어 상태
  //
  switch(p_cmd->state)
  {
    //-- STX 문자 기다리는 상태
    //
    case CMD_STATE_WAIT_STX:

      // 시작 문자를 기다림
      if( ch == CMD_STX )
      {
        p_cmd->state               = CMD_STATE_WAIT_CMD;
        p_cmd->rx_packet.check_sum = 0x00;
        p_cmd->rx_packet.length    = 0;
      }
      break;

    //-- 명령어 기다리는 상태
    //
    case CMD_STATE_WAIT_CMD:
      p_cmd->rx_packet.cmd        = ch;
      p_cmd->rx_packet.check_sum ^= ch;
      p_cmd->state                = CMD_STATE_WAIT_OPTION_ERROR;
      break;

    case CMD_STATE_WAIT_OPTION_ERROR:
      p_cmd->rx_packet.option     = ch;
      p_cmd->rx_packet.error      = ch;
      p_cmd->rx_packet.check_sum ^= ch;
      p_cmd->state                = CMD_STATE_WAIT_LENGTH_L;
      break;

    //-- 데이터 사이즈 기다리는 상태
    //
    case CMD_STATE_WAIT_LENGTH_L:
      p_cmd->rx_packet.length     = ch;
      p_cmd->rx_packet.check_sum ^= ch;
      p_cmd->state                = CMD_STATE_WAIT_LENGTH_H;
      break;

    case CMD_STATE_WAIT_LENGTH_H:
      p_cmd->rx_packet.length    |= ch<<8;
      p_cmd->rx_packet.check_sum ^= ch;
      p_cmd->state                = CMD_STATE_WAIT_LENGTH_H;

      if (p_cmd->rx_packet.length <= CMD_MAX_DATA_LENGTH)
      {
        if (p_cmd->rx_packet.length > 0)
        {
          p_cmd->rx_packet.index = 0;
          p_cmd->state = CMD_STATE_WAIT_DATA;
        }
        else
        {
          p_cmd->state = CMD_STATE_WAIT_CHECKSUM;
        }
      }
      else
      {
        p_cmd->state = CMD_STATE_WAIT_STX;
      }
      break;

    //-- 데이터를 기다리는 상태
    //
    case CMD_STATE_WAIT_DATA:

      index = p_cmd->rx_packet.index;

      p_cmd->rx_packet.check_sum ^= ch;
      index = p_cmd->rx_packet.index;
      p_cmd->rx_packet.data[index] = ch;

      p_cmd->rx_packet.index++;

      if (p_cmd->rx_packet.index >= p_cmd->rx_packet.length)
      {
        p_cmd->state = CMD_STATE_WAIT_CHECKSUM;
      }
      break;

    //-- 체크섬을 기다리는 상태
    //
    case CMD_STATE_WAIT_CHECKSUM:


      p_cmd->rx_packet.check_sum_recv = ch;
      p_cmd->state                    = CMD_STATE_WAIT_ETX;
      break;

    //-- ETX 기다리는 상태
    //
    case CMD_STATE_WAIT_ETX:

      if (ch == CMD_ETX)
      {
        if (p_cmd->rx_packet.check_sum_recv == p_cmd->rx_packet.check_sum)
        {
          ret = true;
        }
      }
      p_cmd->state = CMD_STATE_WAIT_STX;
      break;
  }

  return ret;
}

void cmdSendResp(cmd_t *p_cmd, uint8_t err_code, uint8_t *p_data, uint32_t length)
{
  uint32_t i;
  uint8_t  ch;
  uint8_t  check_sum = 0;
  uint8_t  data;

  ch = p_cmd->ch;




  p_cmd->tx_packet.cmd   = p_cmd->rx_packet.cmd;
  p_cmd->tx_packet.error = err_code;

  if (p_data != NULL)
  {
    for(i=0; i<length; i++)
    {
      p_cmd->tx_packet.data[i] = p_data[i];
    }
  }
  p_cmd->tx_packet.length = length;


  cmdPutch(ch, CMD_STX);
  cmdPutch(ch, p_cmd->tx_packet.cmd);
  check_sum ^= p_cmd->tx_packet.cmd;

  cmdPutch(ch, p_cmd->tx_packet.error);
  check_sum ^= p_cmd->tx_packet.error;

  data = p_cmd->tx_packet.length & 0xFF;
  cmdPutch(ch, data); check_sum ^= data;
  data = (p_cmd->tx_packet.length>>8) & 0xFF;
  cmdPutch(ch, data); check_sum ^= data;


  for( i=0; i<p_cmd->tx_packet.length && i<CMD_MAX_DATA_LENGTH; i++ )
  {
    cmdPutch(ch, p_cmd->tx_packet.data[i]);
    check_sum ^= p_cmd->tx_packet.data[i];
  }

  cmdPutch(ch, check_sum);
  cmdPutch(ch, CMD_ETX);
}

void cmdSendCmd(cmd_t *p_cmd, uint8_t cmd, uint8_t *p_data, uint32_t length)
{
  uint32_t i;
  uint8_t  ch;
  uint8_t  check_sum = 0;
  uint8_t  data;
  uint32_t index;

  ch = p_cmd->ch;


  p_cmd->tx_packet.cmd    = cmd;
  p_cmd->tx_packet.option = 0xFF;

  if (p_data != NULL)
  {
    for(i=0; i<length; i++)
    {
      p_cmd->tx_packet.data[i] = p_data[i];
    }
  }
  p_cmd->tx_packet.length = length;


  index = 0;

  cmd_tx_buffer[index++] = CMD_STX;
  cmd_tx_buffer[index++] = p_cmd->tx_packet.cmd;
  check_sum ^= p_cmd->tx_packet.cmd;

  cmd_tx_buffer[index++] = p_cmd->tx_packet.option;
  check_sum ^= p_cmd->tx_packet.option;

  data = p_cmd->tx_packet.length & 0xFF;
  cmd_tx_buffer[index++] = data;
  check_sum ^= data;
  data = (p_cmd->tx_packet.length>>8) & 0xFF;
  cmd_tx_buffer[index++] = data;
  check_sum ^= data;


  for( i=0; i<p_cmd->tx_packet.length && i<CMD_MAX_DATA_LENGTH; i++ )
  {
    cmd_tx_buffer[index++] = p_cmd->tx_packet.data[i];
    check_sum ^= p_cmd->tx_packet.data[i];
  }

  cmd_tx_buffer[index++] = check_sum;
  cmd_tx_buffer[index++] = CMD_ETX;

  cmdWrite(ch, cmd_tx_buffer, index);
}

bool cmdSendCmdRxResp(cmd_t *p_cmd, uint8_t cmd, uint8_t *p_data, uint32_t length, uint32_t timeout)
{
  bool  ret = true;
  uint8_t err_code = OK;
  uint32_t time_pre;


  cmdSendCmd(p_cmd, cmd, p_data, length);

  time_pre = millis();

  while(1)
  {
    if (cmdReceivePacket(p_cmd) == true)
    {
      err_code = p_cmd->rx_packet.error;
      break;
    }

    if (millis()-time_pre >= timeout)
    {
      p_cmd->rx_packet.error = ERR_TIMEOUT;
      err_code = ERR_TIMEOUT;
      break;
    }
  }

  if (err_code != OK)
  {
    ret = false;
  }

  return ret;
}
