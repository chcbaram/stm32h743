/*
 * ap.cpp
 *
 *  Created on: 2019. 6. 14.
 *      Author: HanCheol Cho
 */




#include "ap.h"
#include "boot/boot.h"


#define MAX_BOOT_CH           2



#define BOOT_MODE_LOADER      0
#define BOOT_MODE_CMDIF       1
#define BOOT_MODE_JUMP_FW     2


uint8_t boot_mode = BOOT_MODE_LOADER;



static cmd_t cmd_boot[MAX_BOOT_CH];


void apInit(void)
{
  uint8_t boot_param;


  uartOpen(_DEF_UART1, 57600);
  uartOpen(_DEF_UART2, 57600);

  cmdInit(&cmd_boot[0]);
  cmdBegin(&cmd_boot[0], _DEF_UART1, 57600);

  cmdInit(&cmd_boot[1]);
  cmdBegin(&cmd_boot[1], _DEF_UART2, 57600);


  cmdifOpen(_DEF_UART2, 57600);

  boot_param = rtcReadBackupData(_HW_DEF_RTC_BOOT_MODE);


  if (boot_param & (1<<7))
  {
    boot_mode = BOOT_MODE_LOADER;
    logPrintf("boot begin...\r\n");

    boot_param &= ~(1<<7);
    rtcWriteBackupData(_HW_DEF_RTC_BOOT_MODE, boot_param);

    usbInit();
    vcpInit();
    return;
  }


  if (buttonGetPressed(_DEF_BUTTON1) == true || hwGetResetCount() == 2)
  {
    boot_mode = BOOT_MODE_CMDIF;
  }
  else if (hwGetResetCount() == 1)
  {
    boot_mode = BOOT_MODE_LOADER;
  }
  else
  {
    boot_mode = BOOT_MODE_JUMP_FW;
  }


  switch(boot_mode)
  {
    case BOOT_MODE_LOADER:
      logPrintf("boot begin...\r\n");
      break;

    case BOOT_MODE_CMDIF:
      logPrintf("cmdif begin...\r\n");
      break;

    case BOOT_MODE_JUMP_FW:
      logPrintf("jump fw...\r\n");

      if (bootVerifyCrc() != true)
      {
        logPrintf("fw crc    \t\t: Fail\r\n");
        logPrintf("boot begin...\r\n");
        boot_mode = BOOT_MODE_LOADER;
      }
      else
      {
        delay(100);
        bootJumpToFw();
      }
      break;
  }

  usbInit();
  vcpInit();
}

void apMain(void)
{
  uint32_t pre_time;
  uint32_t i;


  while(1)
  {
    if (boot_mode == BOOT_MODE_LOADER)
    {
      for (i=0; i<MAX_BOOT_CH; i++)
      {
        if (cmdReceivePacket(&cmd_boot[i]) == true)
        {
          bootProcessCmd(&cmd_boot[i]);
        }
      }

      if (millis()-pre_time >= 100)
      {
        pre_time = millis();
        ledToggle(_DEF_LED1);
      }
    }
    else
    {
      cmdifMain();
      ledOn(_DEF_LED1);
    }

  }
}
