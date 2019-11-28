/*
 * boot.c
 *
 *  Created on: 2019. 6. 4.
 *      Author: Baram
 */


#include "boot.h"
#include "util.h"


extern const __attribute__((section(".version"))) uint8_t boot_ver[32];
extern const __attribute__((section(".version"))) uint8_t boot_name[32];


#define BOOT_CMD_READ_BOOT_VERSION      0x00
#define BOOT_CMD_READ_BOOT_NAME         0x01
#define BOOT_CMD_READ_FIRM_VERSION      0x02
#define BOOT_CMD_READ_FIRM_NAME         0x03
#define BOOT_CMD_FLASH_ERASE            0x04
#define BOOT_CMD_FLASH_WRITE            0x05
#define BOOT_CMD_FLASH_VERIFY           0x06
#define BOOT_CMD_FLASH_READ             0x07
#define BOOT_CMD_JUMP_TO_FW             0x08



static void bootCmdReadBootVersion(cmd_t *p_cmd);
static void bootCmdReadBootName(cmd_t *p_cmd);
static void bootCmdReadFirmVersion(cmd_t *p_cmd);
static void bootCmdReadFirmName(cmd_t *p_cmd);
static void bootCmdFlashErase(cmd_t *p_cmd);
static void bootCmdFlashWrite(cmd_t *p_cmd);
static void bootCmdFlashRead(cmd_t *p_cmd);
static void bootCmdJumpToFw(cmd_t *p_cmd);
static bool bootIsFlashRange(uint32_t addr_begin, uint32_t length);


bool bootCheckFw(void);
void bootJumpToFw(void);


static flash_tag_t  *p_fw_tag = (flash_tag_t *)FLASH_ADDR_TAG;


void bootInit(void)
{
}

void bootJumpToFw(void)
{
  void (**jump_func)(void) = (void (**)(void))(FLASH_ADDR_FW + 4);

  bspDeInit();
  (*jump_func)();
}

bool bootVerifyFw(void)
{
  void (**jump_func)(void) = (void (**)(void))(FLASH_ADDR_FW + 4);


  if ((uint32_t)(*jump_func) != 0xFFFFFFFF)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool bootVerifyCrc(void)
{
  uint32_t i;
  uint8_t *p_data;
  uint16_t fw_crc;

  if (p_fw_tag->magic_number != FLASH_MAGIC_NUMBER)
  {
    logPrintf("fw magic number \t: Fail\r\n");
    return false;
  }

  p_data = (uint8_t *)p_fw_tag->tag_flash_start;

  fw_crc = 0;

  for (i=0; i<p_fw_tag->tag_flash_length; i++)
  {
    utilUpdateCrc(&fw_crc, p_data[i]);
  }

  if (fw_crc == p_fw_tag->tag_flash_crc)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void bootCmdReadBootVersion(cmd_t *p_cmd)
{
  cmdSendResp(p_cmd, OK, (uint8_t *)boot_ver, 32);
}

void bootCmdReadBootName(cmd_t *p_cmd)
{
  cmdSendResp(p_cmd, OK, (uint8_t *)boot_name, 32);
}

void bootCmdReadFirmVersion(cmd_t *p_cmd)
{
  cmdSendResp(p_cmd, OK, p_fw_tag->version_str, 32);
}

void bootCmdReadFirmName(cmd_t *p_cmd)
{
  cmdSendResp(p_cmd, OK, p_fw_tag->board_str, 32);
}

void bootCmdFlashErase(cmd_t *p_cmd)
{
  uint8_t err_code = OK;
  uint32_t addr_begin;
  uint32_t length;


  addr_begin  = p_cmd->rx_packet.data[0]<<0;
  addr_begin |= p_cmd->rx_packet.data[1]<<8;
  addr_begin |= p_cmd->rx_packet.data[2]<<16;
  addr_begin |= p_cmd->rx_packet.data[3]<<24;

  length      = p_cmd->rx_packet.data[4]<<0;
  length     |= p_cmd->rx_packet.data[5]<<8;
  length     |= p_cmd->rx_packet.data[6]<<16;
  length     |= p_cmd->rx_packet.data[7]<<24;


  if (bootIsFlashRange(addr_begin, length) == true)
  {
    if (flashErase(addr_begin, length) == false)
    {
      err_code = ERR_FLASH_ERASE;
    }
  }
  else
  {
    err_code = ERR_FLASH_INVALID_ADDR;
  }

  cmdSendResp(p_cmd, err_code, NULL, 0);

}

void bootCmdFlashWrite(cmd_t *p_cmd)
{
  uint8_t err_code = OK;
  uint32_t addr_begin;
  uint32_t length;


  addr_begin  = p_cmd->rx_packet.data[0]<<0;
  addr_begin |= p_cmd->rx_packet.data[1]<<8;
  addr_begin |= p_cmd->rx_packet.data[2]<<16;
  addr_begin |= p_cmd->rx_packet.data[3]<<24;

  length      = p_cmd->rx_packet.data[4]<<0;
  length     |= p_cmd->rx_packet.data[5]<<8;
  length     |= p_cmd->rx_packet.data[6]<<16;
  length     |= p_cmd->rx_packet.data[7]<<24;


  if (bootIsFlashRange(addr_begin, length) == true)
  {
    if (flashWrite(addr_begin, &p_cmd->rx_packet.data[8], length) == false)
    {
      err_code = ERR_FLASH_WRITE;
    }
  }
  else
  {
    err_code = ERR_FLASH_INVALID_ADDR;
  }

  cmdSendResp(p_cmd, err_code, NULL, 0);
}

void bootCmdFlashRead(cmd_t *p_cmd)
{
  uint8_t err_code = OK;
  uint32_t addr_begin;
  uint32_t length;


  addr_begin  = p_cmd->rx_packet.data[0]<<0;
  addr_begin |= p_cmd->rx_packet.data[1]<<8;
  addr_begin |= p_cmd->rx_packet.data[2]<<16;
  addr_begin |= p_cmd->rx_packet.data[3]<<24;

  length      = p_cmd->rx_packet.data[4]<<0;
  length     |= p_cmd->rx_packet.data[5]<<8;
  length     |= p_cmd->rx_packet.data[6]<<16;
  length     |= p_cmd->rx_packet.data[7]<<24;


  if (length < (CMD_MAX_DATA_LENGTH - 5))
  {
    cmdSendResp(p_cmd, err_code, (uint8_t *)addr_begin, length);
  }
  else
  {
    cmdSendResp(p_cmd, ERR_INVALID_LENGTH, NULL, 0);
  }
}

void bootCmdJumpToFw(cmd_t *p_cmd)
{
  if (bootVerifyFw() != true)
  {
    cmdSendResp(p_cmd, ERR_INVALID_FW, NULL, 0);
  }
  else if (bootVerifyCrc() != true)
  {
    cmdSendResp(p_cmd, ERR_FLASH_CRC, NULL, 0);
  }
  else
  {
    cmdSendResp(p_cmd, OK, NULL, 0);
    delay(100);
    bootJumpToFw();
  }
}

void bootProcessCmd(cmd_t *p_cmd)
{
  switch(p_cmd->rx_packet.cmd)
  {
    case BOOT_CMD_READ_BOOT_VERSION:
      bootCmdReadBootVersion(p_cmd);
      break;

    case BOOT_CMD_READ_BOOT_NAME:
      bootCmdReadBootName(p_cmd);
      break;

    case BOOT_CMD_READ_FIRM_VERSION:
      bootCmdReadFirmVersion(p_cmd);
      break;

    case BOOT_CMD_READ_FIRM_NAME:
      bootCmdReadFirmName(p_cmd);
      break;

    case BOOT_CMD_FLASH_ERASE:
      bootCmdFlashErase(p_cmd);
      break;

    case BOOT_CMD_FLASH_WRITE:
      bootCmdFlashWrite(p_cmd);
      break;

    case BOOT_CMD_FLASH_READ:
      bootCmdFlashRead(p_cmd);
      break;

    case BOOT_CMD_JUMP_TO_FW:
      bootCmdJumpToFw(p_cmd);
      break;


    default:
      cmdSendResp(p_cmd, ERR_INVALID_CMD, NULL, 0);
      break;
  }
}

bool bootIsFlashRange(uint32_t addr_begin, uint32_t length)
{
  bool ret = false;
  uint32_t addr_end;
  uint32_t flash_start;
  uint32_t flash_end;


  addr_end = addr_begin + length - 1;


  flash_start = FLASH_ADDR_START;
  flash_end   = FLASH_ADDR_END;
  if ((addr_begin >= flash_start) && (addr_begin < flash_end) &&
      (addr_end   >= flash_start) && (addr_end   < flash_end))
  {
    ret = true;
  }

#ifdef _USE_HW_QSPI
  flash_start = qspiGetAddr();
  flash_end   = qspiGetAddr() + qspiGetLength();
  if ((addr_begin >= flash_start) && (addr_begin < flash_end) &&
      (addr_end   >= flash_start) && (addr_end   < flash_end))
  {
    ret = true;
  }
#endif

  return ret;
}
