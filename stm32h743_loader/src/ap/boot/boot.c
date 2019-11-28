/*
 * boot.c
 *
 *  Created on: 2018. 8. 26.
 *      Author: Baram
 */




#include "boot.h"





#define BOOT_CMD_READ_BOOT_VERSION      0x00
#define BOOT_CMD_READ_BOOT_NAME         0x01
#define BOOT_CMD_READ_FIRM_VERSION      0x02
#define BOOT_CMD_READ_FIRM_NAME         0x03
#define BOOT_CMD_FLASH_ERASE            0x04
#define BOOT_CMD_FLASH_WRITE            0x05
#define BOOT_CMD_FLASH_VERIFY           0x06
#define BOOT_CMD_FLASH_READ             0x07
#define BOOT_CMD_JUMP_TO_FW             0x08


#define BOOT_CMD_LED_ON                 0x10


cmd_t cmd_boot;



bool bootInit(uint8_t channel, char *port_name, uint32_t baud)
{

  cmdInit(&cmd_boot);

  uartSetPortName(channel, port_name);


  uartOpen(channel, baud);
  uartPutch(channel, 0x55);
  uartPutch(channel, 0x55);
  delay(100);
  uartClose(channel);

  uartOpen(channel, baud);
  uartPutch(channel, 0x55);
  uartPutch(channel, 0x55);
  delay(100);
  uartClose(channel);


  uartOpen(channel, 1200);
  uartPrintf(channel, "BOOT 5555AAAA");
  uartClose(channel);
  delay(2500);

  uartOpen(channel, baud);
  uartPutch(channel, 0x55);
  uartPutch(channel, 0x55);
  uartClose(channel);
  delay(100);


  if (cmdBegin(&cmd_boot, channel, baud) == false)
  {
    return false;
  }

  return true;
}

uint8_t bootCmdReadBootVersion(uint8_t *p_version)
{
  bool ret;
  uint8_t errcode = OK;
  cmd_t *p_cmd = &cmd_boot;
  uint32_t i;

  ret = cmdSendCmdRxResp(p_cmd, BOOT_CMD_READ_BOOT_VERSION, NULL, 0, 500);
  if (ret == true)
  {
    printf("boot version ok\n");
    for (i=0; i<p_cmd->rx_packet.length; i++)
    {
      p_version[i] = p_cmd->rx_packet.data[i];
    }
    p_version[i] = 0;
  }
  else
  {
    errcode = p_cmd->rx_packet.error;
  }

  return errcode;
}

uint8_t bootCmdReadFirmVersion(uint8_t *p_version)
{
  bool ret;
  uint8_t errcode = OK;
  cmd_t *p_cmd = &cmd_boot;
  uint32_t i;


  ret = cmdSendCmdRxResp(p_cmd, BOOT_CMD_READ_FIRM_VERSION, NULL, 0, 500);
  if (ret == true)
  {
    for (i=0; i<p_cmd->rx_packet.length; i++)
    {
      p_version[i] = p_cmd->rx_packet.data[i];
    }
    p_version[i] = 0;
  }
  else
  {
    errcode = p_cmd->rx_packet.error;
  }

  return errcode;
}


uint8_t bootCmdReadBootName(uint8_t *p_str)
{
  bool ret;
  uint8_t errcode = OK;
  cmd_t *p_cmd = &cmd_boot;
  uint32_t i;


  ret = cmdSendCmdRxResp(p_cmd, BOOT_CMD_READ_BOOT_NAME, NULL, 0, 1000);
  if (ret == true)
  {
    for (i=0; i<p_cmd->rx_packet.length; i++)
    {
      p_str[i] = p_cmd->rx_packet.data[i];
    }
    p_str[i] = 0;
  }
  else
  {
    errcode = p_cmd->rx_packet.error;
  }

  return errcode;
}

uint8_t bootCmdReadFirmName(uint8_t *p_str)
{
  bool ret;
  uint8_t errcode = OK;
  cmd_t *p_cmd = &cmd_boot;
  uint32_t i;


  ret = cmdSendCmdRxResp(p_cmd, BOOT_CMD_READ_FIRM_NAME, NULL, 0, 1000);
  if (ret == true)
  {
    for (i=0; i<p_cmd->rx_packet.length; i++)
    {
      p_str[i] = p_cmd->rx_packet.data[i];
    }
    p_str[i] = 0;
  }
  else
  {
    errcode = p_cmd->rx_packet.error;
  }

  return errcode;
}


uint8_t bootCmdLedOn(uint8_t enable)
{
  bool ret;
  uint8_t errcode = OK;
  cmd_t *p_cmd = &cmd_boot;


  ret = cmdSendCmdRxResp(p_cmd, BOOT_CMD_LED_ON, &enable, 1, 100);
  if (ret != true)
  {
    errcode = p_cmd->rx_packet.error;
  }

  return errcode;
}


uint8_t bootCmdFlashErase(uint32_t addr, uint32_t length)
{
  bool ret;
  uint8_t errcode = OK;
  cmd_t *p_cmd = &cmd_boot;
  uint8_t data[32];


  data[0] = addr >> 0;
  data[1] = addr >> 8;
  data[2] = addr >> 16;
  data[3] = addr >> 24;

  data[4] = length >> 0;
  data[5] = length >> 8;
  data[6] = length >> 16;
  data[7] = length >> 24;


  ret = cmdSendCmdRxResp(p_cmd, BOOT_CMD_FLASH_ERASE, data, 8, 15000);
  if (ret == false)
  {
    errcode = p_cmd->rx_packet.error;
  }

  return errcode;
}

uint8_t bootCmdFlashWrite(uint32_t addr, uint8_t *p_data, uint32_t length)
{
  bool ret;
  uint8_t errcode = OK;
  cmd_t *p_cmd = &cmd_boot;
  uint32_t i;
  uint32_t index;
  uint8_t *data = p_cmd->tx_packet.data;


  data[0] = addr >> 0;
  data[1] = addr >> 8;
  data[2] = addr >> 16;
  data[3] = addr >> 24;

  data[4] = length >> 0;
  data[5] = length >> 8;
  data[6] = length >> 16;
  data[7] = length >> 24;

  index = 8;
  for (i=0; i<length; i++)
  {
    data[index++] = p_data[i];
  }

  ret = cmdSendCmdRxResp(p_cmd, BOOT_CMD_FLASH_WRITE, data, index, 1000);
  if (ret == false)
  {
    errcode = p_cmd->rx_packet.error;
  }

  return errcode;
}

uint8_t bootCmdJumpToFw(void)
{
  bool ret;
  uint8_t errcode = OK;
  cmd_t *p_cmd = &cmd_boot;


  ret = cmdSendCmdRxResp(p_cmd, BOOT_CMD_JUMP_TO_FW, NULL, 0, 1000);
  if (ret == false)
  {
    errcode = p_cmd->rx_packet.error;
  }

  return errcode;
}
