/*
 * delay.c
 *
 *  Created on: 2019. 3. 7.
 *      Author: HanCheol Cho
 */




#include "delay.h"
#include "micros.h"



bool delayInit(void)
{
  return true;
}

void delay(uint32_t ms)
{
#ifdef _USE_HW_RTOS
  if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
  {
    osDelay(ms);
  }
  else
  {
    HAL_Delay(ms);
  }
#else
  HAL_Delay(ms);
#endif
}

void delayNs(uint32_t ns)
{
  for (volatile uint32_t i = 0; i < ns/10; i++) { }
}

#ifdef _USE_HW_MICROS
void delayUs(uint32_t us)
{
  uint32_t t_time;


  t_time = micros();

  while(1)
  {
    if ((micros()-t_time) >= us)
    {
      break;
    }
  }
}
#endif

void delayMs(uint32_t ms)
{
  HAL_Delay(ms);
}
