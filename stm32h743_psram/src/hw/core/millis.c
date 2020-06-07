/*
 * millis.c
 *
 *  Created on: 2019. 3. 7.
 *      Author: HanCheol Cho
 */



#include "millis.h"



bool millisInit(void)
{
  return true;
}

uint32_t millis(void)
{
  return HAL_GetTick();
}

