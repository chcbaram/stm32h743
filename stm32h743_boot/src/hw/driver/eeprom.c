/*
 * eeprom.c
 *
 *  Created on: 2019. 9. 24.
 *      Author: HanCheol Cho
 */




#include "eeprom.h"
#include "i2c.h"
#include "cmdif.h"


#ifdef _USE_HW_EEPROM



#if HW_USE_CMDIF_EEPROM == 1
void eepromCmdifInit(void);
void eepromCmdif(void);
#endif




static bool is_init = false;
static uint8_t i2c_ch = _DEF_I2C1;
static uint8_t i2c_addr = (0xA0>>1);
static uint8_t last_error = 0;


bool eepromInit()
{
  bool ret;


  ret = i2cBegin(i2c_ch, 400);


#if HW_USE_CMDIF_EEPROM == 1
  eepromCmdifInit();
#endif

  is_init = ret;

  return ret;
}

bool eepromIsInit(void)
{
  return is_init;
}

bool eepromValid(uint32_t addr)
{
  uint8_t data;
  bool ret;
  uint8_t sub_addr;

  sub_addr = (addr>>8) & 0x03;

  ret = i2cReadByte(i2c_ch, i2c_addr + sub_addr, addr & 0xFF, &data, 100);

  return ret;
}

uint8_t eepromReadByte(uint32_t addr)
{
  uint8_t data;
  bool ret;
  uint8_t sub_addr;

  sub_addr = (addr>>8) & 0x03;

  ret = i2cReadByte(i2c_ch, i2c_addr + sub_addr, addr & 0xFF, &data, 100);

  last_error = 0;
  if (ret != true) last_error = 1;

  return data;
}

bool eepromWriteByte(uint32_t addr, uint8_t data_in)
{
  uint8_t sub_addr;
  uint32_t pre_time;
  bool ret;

  sub_addr = (addr>>8) & 0x03;

  pre_time = millis();
  while(millis()-pre_time < 100)
  {
    ret = i2cWriteByte(i2c_ch, i2c_addr + sub_addr, addr & 0xFF, data_in, 10);
    if (ret == true)
    {
      break;
    }
  }

  return ret;
}

bool eepromRead(uint32_t addr, uint8_t *p_data, uint32_t length)
{
  bool ret = true;
  uint32_t i;


  for (i=0; i<length; i++)
  {
    p_data[i] = eepromReadByte(addr);

    if (last_error != 0)
    {
      ret = false;
      break;
    }
  }

  return ret;
}

bool eepromWrite(uint32_t addr, uint8_t *p_data, uint32_t length)
{
  bool ret;
  uint32_t i;


  for (i=0; i<length; i++)
  {
    ret = eepromWriteByte(addr, p_data[i]);
    if (ret == false)
    {
      break;
    }
  }

  return ret;
}

uint32_t eepromGetLength(void)
{
  return 1024;
}

bool eepromFormat(void)
{
  return true;
}




#if HW_USE_CMDIF_EEPROM == 1
void eepromCmdifInit(void)
{
  cmdifAdd("eeprom", eepromCmdif);
}

void eepromCmdif(void)
{
  bool ret = true;
  uint32_t i;
  uint32_t addr;
  uint32_t length;
  uint8_t  data;
  uint32_t pre_time;
  bool eep_ret;


  if (cmdifGetParamCnt() == 1)
  {
    if(cmdifHasString("info", 0) == true)
    {
      cmdifPrintf("eeprom init   : %d\n", eepromIsInit());
      cmdifPrintf("eeprom length : %d bytes\n", eepromGetLength());
    }
    else if(cmdifHasString("format", 0) == true)
    {
      if (eepromFormat() == true)
      {
        cmdifPrintf("format OK\n");
      }
      else
      {
        cmdifPrintf("format Fail\n");
      }
    }
    else
    {
      ret = false;
    }
  }
  else if (cmdifGetParamCnt() == 3)
  {
    if(cmdifHasString("read", 0) == true)
    {
      addr   = (uint32_t)cmdifGetParam(1);
      length = (uint32_t)cmdifGetParam(2);

      if (length > eepromGetLength())
      {
        cmdifPrintf( "length error\n");
      }
      for (i=0; i<length; i++)
      {
        data = eepromReadByte(addr+i);
        cmdifPrintf( "addr : %d\t 0x%02X\n", addr+i, data);
      }
    }
    else if(cmdifHasString("write", 0) == true)
    {
      addr = (uint32_t)cmdifGetParam(1);
      data = (uint8_t )cmdifGetParam(2);

      pre_time = micros();
      eep_ret = eepromWriteByte(addr, data);

      cmdifPrintf( "addr : %d\t 0x%02X %dus\n", addr, data, micros()-pre_time);
      if (eep_ret)
      {
        cmdifPrintf("OK\n");
      }
      else
      {
        cmdifPrintf("FAIL\n");
      }
    }
    else
    {
      ret = false;
    }
  }
  else
  {
    ret = false;
  }


  if (ret == false)
  {
    cmdifPrintf( "eeprom info\n");
    cmdifPrintf( "eeprom format\n");
    cmdifPrintf( "eeprom read  [addr] [length]\n");
    cmdifPrintf( "eeprom write [addr] [data]\n");
  }

}
#endif /* _USE_HW_CMDIF_EEPROM */


#endif /* _USE_HW_EEPROM */
