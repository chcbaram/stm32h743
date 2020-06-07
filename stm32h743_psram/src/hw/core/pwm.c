/*
 * pwm.c
 *
 *  Created on: 2019. 9. 4.
 *      Author: Baram
 */




#include "pwm.h"
#include "cmdif.h"



#if HW_USE_CMDIF_PWM == 1
void pwmCmdif(void);
#endif



static bool is_init = false;

typedef struct
{
  TIM_HandleTypeDef  TimHandle;
  TIM_OC_InitTypeDef sConfig;
  uint32_t           channel;
} pwm_tbl_t;


pwm_tbl_t  pwm_tbl[HW_PWM_MAX_CH];



uint32_t uhPrescalerValue = 0;



bool pwmInit(void)
{
  pwm_tbl_t *p_pwm;
  GPIO_InitTypeDef   GPIO_InitStruct;


  __TIM17_CLK_ENABLE();


  uhPrescalerValue = (uint32_t)((SystemCoreClock /2) / (10000 * 255)) - 1;

  p_pwm = &pwm_tbl[0];

  p_pwm->TimHandle.Instance = TIM17;
  p_pwm->TimHandle.Init.Prescaler         = uhPrescalerValue;
  p_pwm->TimHandle.Init.Period            = 255-1;
  p_pwm->TimHandle.Init.ClockDivision     = 0;
  p_pwm->TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  p_pwm->TimHandle.Init.RepetitionCounter = 0;
  p_pwm->TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  HAL_TIM_PWM_Init(&p_pwm->TimHandle);



  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF1_TIM17;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);


  p_pwm->sConfig.OCMode       = TIM_OCMODE_PWM1;
  p_pwm->sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
  p_pwm->sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
  p_pwm->sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
  p_pwm->sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

  p_pwm->sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;

  p_pwm->sConfig.Pulse = 0;
  p_pwm->channel = TIM_CHANNEL_1;

  HAL_TIM_PWM_ConfigChannel(&p_pwm->TimHandle, &p_pwm->sConfig, p_pwm->channel);
  HAL_TIM_PWM_Start(&p_pwm->TimHandle, p_pwm->channel);



  is_init = true;

#if HW_USE_CMDIF_PWM == 1
  cmdifAdd("pwm", pwmCmdif);
#endif

  return true;
}

bool pwmIsInit(void)
{
  return is_init;
}

void pwmWrite(uint8_t ch, uint16_t pwm_data)
{
  if (ch >= HW_PWM_MAX_CH) return;


  pwm_tbl[ch].sConfig.Pulse = pwm_data;
  pwm_tbl[ch].TimHandle.Instance->CCR1 = pwm_data;
}

uint16_t pwmRead(uint8_t ch)
{
  if (ch >= HW_PWM_MAX_CH) return 0;

  return pwm_tbl[ch].sConfig.Pulse;
}




#if HW_USE_CMDIF_PWM == 1
void pwmCmdif(void)
{
  bool ret = true;
  uint8_t  ch;
  uint32_t pwm;


  if (cmdifGetParamCnt() == 3)
  {
    ch = (uint8_t) cmdifGetParam(1);
    pwm = (uint8_t) cmdifGetParam(2);

    ch = constrain(ch, 0, PWM_MAX_CH);

    if(cmdifHasString("set", 0) == true)
    {
      pwmWrite(ch, pwm);
      cmdifPrintf("pwm CH%d %d\n", ch, pwm);
    }
    else
    {
      ret = false;
    }
  }
  else if (cmdifGetParamCnt() == 2)
  {
    ch = (uint8_t) cmdifGetParam(1);

    if(cmdifHasString("get", 0) == true)
    {
      cmdifPrintf("pwm CH%d %d\n", ch, pwmRead(ch));
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
    cmdifPrintf( "pwm set 0~%d 0~1000 \n", PWM_MAX_CH-1);
    cmdifPrintf( "pwm get 0~%d \n", PWM_MAX_CH-1);
  }

}
#endif
