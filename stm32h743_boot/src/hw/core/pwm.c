/*
 * pwm.c
 *
 *  Created on: 2019. 9. 4.
 *      Author: Baram
 */




#include "pwm.h"


static bool is_init = false;

TIM_HandleTypeDef  TimHandle;
TIM_OC_InitTypeDef sConfig;


uint32_t uhPrescalerValue = 0;



bool pwmInit(void)
{
  __TIM5_CLK_ENABLE();



  uhPrescalerValue = (uint32_t)((SystemCoreClock /1) / (1000 * 255)) - 1;

  TimHandle.Instance = TIM5;

  TimHandle.Init.Prescaler         = uhPrescalerValue;
  TimHandle.Init.Period            = 255-1;
  TimHandle.Init.ClockDivision     = 0;
  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  TimHandle.Init.RepetitionCounter = 0;
  TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  HAL_TIM_PWM_Init(&TimHandle);


  GPIO_InitTypeDef   GPIO_InitStruct;
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* TIMx Peripheral clock enable */


  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM5;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


  sConfig.OCMode       = TIM_OCMODE_PWM1;
  sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
  sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
  sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
  sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

  sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;

  sConfig.Pulse = 0;
  HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1);

  is_init = true;

  return true;
}

bool pwmIsInit(void)
{
  return is_init;
}

void pwmWrite(uint8_t ch, uint16_t pwm_data)
{
  sConfig.Pulse = pwm_data;

  HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1);

  TimHandle.Instance->CCR1 = pwm_data;
}

uint16_t pwmRead(uint8_t ch)
{
  return sConfig.Pulse;
}
