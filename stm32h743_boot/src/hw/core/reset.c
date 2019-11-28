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
  uint8_t ret = 0;

  if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET)
  {
   ret |= (1<<_DEF_RESET_PIN);
  }
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST) != RESET)
  {
   ret |= (1<<_DEF_RESET_POWER);
  }
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET)
  {
   ret |= (1<<_DEF_RESET_POWER);
  }
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDG1RST) != RESET)
  {
   ret |= (1<<_DEF_RESET_WDG);
  }
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDG1RST) != RESET)
  {
   ret |= (1<<_DEF_RESET_WDG);
  }
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST) != RESET)
  {
   ret |= (1<<_DEF_RESET_SOFT);
  }

  reset_bits = ret;

  __HAL_RCC_CLEAR_RESET_FLAGS();


  if (ret & (1<<_DEF_RESET_WDG))
  {
    reset_status = _DEF_RESET_WDG;
  }
  else if (ret & (1<<_DEF_RESET_SOFT))
  {
    reset_status = _DEF_RESET_SOFT;
  }
  else if (ret & (1<<_DEF_RESET_POWER))
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

void resetRunSoftReset(void)
{
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
