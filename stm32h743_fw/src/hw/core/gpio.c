/*
 * gpio.c
 *
 *  Created on: 2019. 3. 8.
 *      Author: HanCheol Cho
 */




#include "gpio.h"
#include "cmdif.h"


typedef struct
{
  GPIO_TypeDef *port;
  uint16_t      pin;
  uint8_t       mode;
  uint8_t       on_state;
  uint8_t       init_state;
} gpio_tbl_t;


gpio_tbl_t gpio_tbl[GPIO_MAX_CH] =
{
  {GPIOB, GPIO_PIN_15,  _DEF_INPUT_PULLUP,  _DEF_HIGH, _DEF_HIGH},  // 0. SDCARD_CD
};



void gpioCmdif(void);



bool gpioInit(void)
{
  uint32_t i;


  for (i=0; i<GPIO_MAX_CH; i++)
  {
    if (gpio_tbl[i].mode == _DEF_OUTPUT)
    {
      gpioPinWrite(i, gpio_tbl[i].init_state);
    }

    gpioPinMode(i, gpio_tbl[i].mode);
  }


  cmdifAdd("gpio", gpioCmdif);

  return true;
}

void gpioPinMode(uint8_t channel, uint8_t mode)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  const gpio_tbl_t* p_pin = &gpio_tbl[channel];

  GPIO_InitStruct.Pin = p_pin->pin;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;


  if (channel >= GPIO_MAX_CH)
  {
    return;
  }

  switch (mode)
  {
    case _DEF_INPUT:
      GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      break;

    case _DEF_INPUT_PULLUP:
      GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
      GPIO_InitStruct.Pull = GPIO_PULLUP;
      break;

    case _DEF_INPUT_PULLDOWN:
      GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
      GPIO_InitStruct.Pull = GPIO_PULLDOWN;
      break;

    case _DEF_OUTPUT:
      GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      break;

    case _DEF_OUTPUT_PULLUP:
      GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
      GPIO_InitStruct.Pull = GPIO_PULLUP;
      break;

    case _DEF_OUTPUT_PULLDOWN:
      GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
      GPIO_InitStruct.Pull = GPIO_PULLDOWN;
      break;
  }
  HAL_GPIO_Init(p_pin->port, &GPIO_InitStruct);
}

void gpioPinWrite(uint8_t channel, uint8_t value)
{
  GPIO_PinState pin_value;

  if (channel >= GPIO_MAX_CH)
  {
    return;
  }

  if (value > 0)
  {
    pin_value = GPIO_PIN_SET;
  }
  else
  {
    pin_value = GPIO_PIN_RESET;
  }

  HAL_GPIO_WritePin(gpio_tbl[channel].port, gpio_tbl[channel].pin, pin_value);
}

uint8_t gpioPinRead(uint8_t channel)
{
  if (HAL_GPIO_ReadPin(gpio_tbl[channel].port, gpio_tbl[channel].pin) == GPIO_PIN_SET)
  {
    return _DEF_HIGH;
  }
  else
  {
    return _DEF_LOW;
  }
}

void gpioPinToggle(uint8_t channel)
{
  if (channel >= GPIO_MAX_CH)
  {
    return;
  }

  HAL_GPIO_TogglePin(gpio_tbl[channel].port, gpio_tbl[channel].pin);
}





//-- gpioCmdif
//
void gpioCmdif(void)
{
  bool ret = true;
  uint8_t number;
  uint8_t state;


  if (cmdifGetParamCnt() == 1)
  {
    if (cmdifHasString("show", 0) == true)
    {
      while(cmdifRxAvailable() == 0)
      {
        for (int i=0; i<GPIO_MAX_CH; i++)
        {
          cmdifPrintf("%d ", gpioPinRead(i));
        }
        cmdifPrintf("\r\n");
        delay(50);
      }
    }
    else
    {
      ret = false;
    }
  }
  else if (cmdifGetParamCnt() == 2)
  {
    if (cmdifHasString("read", 0) == true)
    {
      number = (uint8_t)cmdifGetParam(1);

      while(cmdifRxAvailable() == 0)
      {
        cmdifPrintf("gpio %d : %d\r\n", number, gpioPinRead(number));
        delay(50);
      }
    }
    else
    {
      ret = false;
    }
  }
  else if (cmdifGetParamCnt() == 3)
  {
    if (cmdifHasString("write", 0) == true)
    {
      number = (uint8_t)cmdifGetParam(1);
      state = (uint8_t)cmdifGetParam(2);

      gpioPinWrite(number, state);

      cmdifPrintf("gpio write %d : %d\r\n", number, gpioPinRead(number));
    }
    if (cmdifHasString("on", 0) == true)
    {
      uint32_t on_time;

      number = (uint8_t)cmdifGetParam(1);
      on_time = (uint32_t)cmdifGetParam(2);

      gpioPinWrite(number, 1);
      delay(on_time);
      gpioPinWrite(number, 0);

      cmdifPrintf("gpio on %d : %d ms\r\n", number, on_time);
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
    cmdifPrintf( "gpio show\n");
    cmdifPrintf( "gpio read  0~%d\n", GPIO_MAX_CH-1);
    cmdifPrintf( "gpio write 0~%d [0:1]\n", GPIO_MAX_CH-1);
    cmdifPrintf( "gpio on 0~%d ms[0~3000]\n", GPIO_MAX_CH-1);
  }
}
