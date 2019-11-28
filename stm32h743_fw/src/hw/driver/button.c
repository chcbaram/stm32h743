/*
 * button.c
 *
 *  Created on: 2019. 9. 1.
 *      Author: Baram
 */




#include "button.h"
#include "swtimer.h"
#include "cmdif.h"


typedef struct
{
  GPIO_TypeDef *port;
  uint32_t      pin;
  GPIO_PinState on_state;
  GPIO_PinState off_state;
} button_tbl_t;



button_tbl_t button_port_tbl[BUTTON_MAX_CH] =
{
  {GPIOE, GPIO_PIN_3, GPIO_PIN_RESET, GPIO_PIN_SET},
  {GPIOC, GPIO_PIN_5, GPIO_PIN_RESET, GPIO_PIN_SET},
};



typedef struct
{
  bool        pressed;
  bool        pressed_event;
  uint16_t    pressed_cnt;
  uint32_t    pressed_start_time;
  uint32_t    pressed_end_time;

  bool        released;
  bool        released_event;
  uint32_t    released_start_time;
  uint32_t    released_end_time;

} button_t;


static button_t button_tbl[BUTTON_MAX_CH];


#ifdef HW_USE_CMDIF_BUTTON
void buttonCmdifInit(void);
void buttonCmdif(void);
#endif

static bool is_enable = true;
static bool buttonGetPin(uint8_t ch);


void button_isr(void *arg)
{
  uint8_t i;


  for (i=0; i<BUTTON_MAX_CH; i++)
  {

    if (buttonGetPin(i))
    {
      if (button_tbl[i].pressed == false)
      {
        button_tbl[i].pressed_event = true;
        button_tbl[i].pressed_start_time = millis();
      }

      button_tbl[i].pressed = true;
      button_tbl[i].pressed_cnt++;

      button_tbl[i].pressed_end_time = millis();

      button_tbl[i].released = false;
    }
    else
    {
      if (button_tbl[i].pressed == true)
      {
        button_tbl[i].released_event = true;
        button_tbl[i].released_start_time = millis();
      }

      button_tbl[i].pressed  = false;
      button_tbl[i].released = true;

      button_tbl[i].released_end_time = millis();
    }
  }
}



bool buttonInit(void)
{
  uint32_t i;
  GPIO_InitTypeDef  GPIO_InitStruct;
  swtimer_handle_t h_button_timer;


  GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  for (i=0; i<BUTTON_MAX_CH; i++)
  {
    GPIO_InitStruct.Pin = button_port_tbl[i].pin;
    HAL_GPIO_Init(button_port_tbl[i].port, &GPIO_InitStruct);
  }

  for (i=0; i<BUTTON_MAX_CH; i++)
  {
    button_tbl[i].pressed_cnt    = 0;
    button_tbl[i].pressed        = 0;
    button_tbl[i].released       = 0;
    button_tbl[i].released_event = 0;
  }

  h_button_timer = swtimerGetHandle();
  swtimerSet(h_button_timer, 1, LOOP_TIME, button_isr, NULL );
  swtimerStart(h_button_timer);

#ifdef HW_USE_CMDIF_BUTTON
  buttonCmdifInit();
#endif


  return true;
}

void buttonResetTime(uint8_t ch)
{
  button_tbl[ch].pressed_start_time    = 0;
  button_tbl[ch].pressed_end_time      = 0;
  button_tbl[ch].released_start_time   = 0;
  button_tbl[ch].released_end_time     = 0;
}

bool buttonGetPin(uint8_t ch)
{
  if (ch >= BUTTON_MAX_CH)
  {
    return false;
  }

  if (HAL_GPIO_ReadPin(button_port_tbl[ch].port, button_port_tbl[ch].pin) == button_port_tbl[ch].on_state)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void buttonEnable(bool enable)
{
  is_enable = enable;
}

bool buttonGetPressed(uint8_t ch)
{
  if (ch >= BUTTON_MAX_CH || is_enable == false)
  {
    return false;
  }

  return button_tbl[ch].pressed;
}

bool buttonOsdGetPressed(uint8_t ch)
{
  if (ch >= BUTTON_MAX_CH)
  {
    return false;
  }

  return button_tbl[ch].pressed;
}

bool buttonGetPressedEvent(uint8_t ch)
{
  bool ret;


  if (ch >= BUTTON_MAX_CH || is_enable == false) return false;

  ret = button_tbl[ch].pressed_event;

  button_tbl[ch].pressed_event = 0;

  return ret;
}

uint32_t buttonGetPressedTime(uint8_t ch)
{
  volatile uint32_t ret;


  if (ch >= BUTTON_MAX_CH || is_enable == false) return 0;


  ret = button_tbl[ch].pressed_end_time - button_tbl[ch].pressed_start_time;

  return ret;
}


bool buttonGetReleased(uint8_t ch)
{
  bool ret;


  if (ch >= BUTTON_MAX_CH || is_enable == false) return false;

  ret = button_tbl[ch].released;

  return ret;
}

bool buttonGetReleasedEvent(uint8_t ch)
{
  bool ret;


  if (ch >= BUTTON_MAX_CH || is_enable == false) return false;

  ret = button_tbl[ch].released_event;

  button_tbl[ch].released_event = 0;

  return ret;
}

uint32_t buttonGetReleasedTime(uint8_t ch)
{
  volatile uint32_t ret;


  if (ch >= BUTTON_MAX_CH || is_enable == false) return 0;


  ret = button_tbl[ch].released_end_time - button_tbl[ch].released_start_time;

  return ret;
}



#if HW_USE_CMDIF_BUTTON == 1
void buttonCmdifInit(void)
{
  cmdifAdd("button", buttonCmdif);
}

void buttonCmdif(void)
{
  bool ret = true;
  uint8_t ch;
  uint32_t i;


  if (cmdifGetParamCnt() == 1)
  {
    if(cmdifHasString("show", 0) == true)
    {
      while(cmdifRxAvailable() == 0)
      {
        for (i=0; i<BUTTON_MAX_CH; i++)
        {
          cmdifPrintf("%d", buttonGetPressed(i));
        }
        cmdifPrintf("\r");
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
    ch = (uint8_t)cmdifGetParam(1);

    if (ch > 0)
    {
      ch--;
    }

    if (cmdifHasString("time", 0) == true)
    {
      while(cmdifRxAvailable() == 0)
      {
        if(buttonGetPressed(ch))
        {
          cmdifPrintf("BUTTON%d, Time :  %d ms\n", ch+1, buttonGetPressedTime(ch));
        }
      }
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
    cmdifPrintf( "button [show/time] channel(1~%d) ...\n", BUTTON_MAX_CH);
  }
}
#endif
