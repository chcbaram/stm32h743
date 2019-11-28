/*
 * reset.c
 *
 *  Created on: 2019. 6. 21.
 *      Author: HanCheol Cho
 */




#include "reset.h"


static uint8_t reset_status = 0x00;
static uint8_t reset_bits   = 0x00;


void resetInit(void)
{
  if (reset_bits & (1<<_DEF_RESET_WDG))
  {
    reset_status = _DEF_RESET_WDG;
  }
  else if (reset_bits & (1<<_DEF_RESET_SOFT))
  {
    reset_status = _DEF_RESET_SOFT;
  }
  else if (reset_bits & (1<<_DEF_RESET_POWER))
  {
    reset_status = _DEF_RESET_POWER;
  }
  else
  {
    reset_status = _DEF_RESET_PIN;
  }
}

void resetLog(void)
{
  if (reset_bits & (1<<_DEF_RESET_POWER))
  {
    logPrintf("ResetFrom \t\t: Power\r\n");
  }
  if (reset_bits & (1<<_DEF_RESET_PIN))
  {
    logPrintf("ResetFrom \t\t: Pin\r\n");
  }
  if (reset_bits & (1<<_DEF_RESET_WDG))
  {
    logPrintf("ResetFrom \t\t: Wdg\r\n");
  }
  if (reset_bits & (1<<_DEF_RESET_SOFT))
  {
    logPrintf("ResetFrom \t\t: Soft\r\n");
  }
}

void resetSetBits(uint8_t data)
{
  reset_bits = data;
}

void resetRunSoftReset(void)
{
  SysTick->CTRL = 0;
  __set_CONTROL(0x00);
  __set_MSP(*(__IO uint32_t*)FLASH_BANK1_BASE);
  SCB->VTOR = FLASH_BANK1_BASE;

  HAL_NVIC_SystemReset();
}

void resetClearFlag(void)
{
  __HAL_RCC_CLEAR_RESET_FLAGS();
}

uint8_t resetGetStatus(void)
{
  return reset_status;
}

uint8_t resetGetBits(void)
{
  return reset_bits;
}
