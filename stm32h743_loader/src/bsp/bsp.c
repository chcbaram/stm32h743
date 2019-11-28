/*
 * bsp.c
 *
 *  Created on: 2018. 8. 25.
 *      Author: Baram
 */



#include "bsp.h"





void bspInit(void)
{
}



void delay(uint32_t delay_ms)
{
  uint32_t time_pre;

  time_pre = millis();
  while(1)
  {
    if (millis()-time_pre >= delay_ms)
    {
      break;
    }
  }
}


#if defined (__WIN32__) || (__WIN64__)
uint32_t millis(void)
{
  double ret;

  LARGE_INTEGER freq, counter;

  QueryPerformanceCounter(&counter);
  QueryPerformanceFrequency(&freq);

  ret = (double)counter.QuadPart / (double)freq.QuadPart * 1000.0;

  return (uint32_t)ret;
}
#endif
