/*
 * micros.c
 *
 *  Created on: 2019. 3. 7.
 *      Author: HanCheol Cho
 */




#include "micros.h"



static TIM_HandleTypeDef  TimHandle;



bool microsInit(void)
{
  uint32_t uwPrescalerValue = 0;

  __HAL_RCC_TIM2_CLK_ENABLE();


  /* Set TIMx instance */
  TimHandle.Instance = TIM2;


  // Compute the prescaler value to have TIMx counter clock equal to 1Mh
  uwPrescalerValue = (uint32_t)((SystemCoreClock / 2) / 1000000) - 1;

  TimHandle.Init.Period         = 0xFFFFFFFF;
  TimHandle.Init.Prescaler      = uwPrescalerValue;
  TimHandle.Init.ClockDivision  = 0;
  TimHandle.Init.CounterMode    = TIM_COUNTERMODE_UP;

  HAL_TIM_Base_Init(&TimHandle);
  HAL_TIM_Base_Start(&TimHandle);


  return true;
}

uint32_t micros(void)
{
  return TimHandle.Instance->CNT;
}
