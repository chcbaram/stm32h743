/*
 *  rc100.c
 *
 *  Created on: 2017. 2. 13.
 *      Author: baram
 */


#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>


#include "rc100.h"

#ifdef _USE_HW_RC100

#include "hw.h"





//-- Internal Variables
//



//-- External Variables
//


//-- Internal Functions
//
#ifdef _USE_HW_CMDIF_RC100
void rc100CmdifInit(void);
int rc100Cmdif(int argc, char **argv);
#endif

//-- External Functions
//




bool rc100Init(void)
{

#ifdef _USE_HW_CMDIF_RC100
  rc100CmdifInit();
#endif

  return true;
}



void rc100Begin(rc100_t *p_rc100)
{
  p_rc100->state = 0;
  p_rc100->index = 0;
  p_rc100->received = false;
  p_rc100->init  = true;

  p_rc100->time_t = millis();
}

bool rc100Update(rc100_t *p_rc100, uint8_t data)
{
  bool ret = false;
  
  p_rc100->inv_data = ~data;
  
  
  if (millis()-p_rc100->time_t > 100)
  {
    p_rc100->state = 0;
  }
  
  
  switch(p_rc100->state)
  {
    case 0:
      if (data == 0xFF)
      {
        p_rc100->state = 1;
        p_rc100->time_t = millis();
      }
      break;
      
    case 1:
      if (data == 0x55)
      {
        p_rc100->state    = 2;
        p_rc100->received = false;
        p_rc100->data     = 0;
      }
      else
      {
        p_rc100->state = 0;
      }
      break;
      
    case 2:
      p_rc100->data      = data;
      p_rc100->save_data = data;
      p_rc100->state     = 3;
      break;

    case 3:
      if (p_rc100->save_data == p_rc100->inv_data)
      {
        p_rc100->state = 4;
      }
      else
      {
        p_rc100->state = 0;
      }
      break;

    case 4:
      p_rc100->data     |= data<<8;
      p_rc100->save_data = data;
      p_rc100->state     = 5;
      break;

    case 5:
      if (p_rc100->save_data == p_rc100->inv_data)
      {
        p_rc100->received = true;
        ret = true;
      }
      p_rc100->state = 0;
      break;
      
    default:
      p_rc100->state = 0;
      break;
  }
  
  return ret;
}

uint16_t rc100ReadMsg(rc100_t *p_rc100)
{
  return p_rc100->data;
}





#ifdef _USE_HW_CMDIF_RC100
void rc100CmdifInit(void)
{
  if (cmdifIsInit() == false)
  {
    cmdifInit();
  }
  cmdifAdd("rc100", rc100Cmdif);
}

int rc100Cmdif(int argc, char **argv)
{
  bool ret = true;
  uint8_t number;
  rc100_t rc100_rx;
  bool    rx_flag;


  if (argc < 3)
  {
    ret = false;
  }
  else
  {
    number = (uint8_t) strtoul((const char * ) argv[2], (char **)NULL, (int) 0);

    if (number > 0) number--;

    if(strcmp("begin", argv[1]) == 0)
    {
      uartOpen(number, 57600);
      rc100Begin(&rc100_rx);
      cmdifPrintf("uart ch : DEF_UART%d open\n", number+1);
    }
    else if(strcmp("raw", argv[1]) == 0)
    {
      while(cmdifRxAvailable() == 0)
      {
        rx_flag = false;
        if (uartAvailable(number))
        {
          uint8_t ch;

          ch = uartRead(number);

          rx_flag = rc100Update(&rc100_rx, ch);
          cmdifPrintf("rx : 0x%X\n", ch);
        }
      }
    }
    else if(strcmp("msg", argv[1])==0)
    {
      while(cmdifRxAvailable() == 0)
      {
        rx_flag = false;
        if (uartAvailable(number))
        {
          uint8_t ch;

          ch = uartRead(number);

          rx_flag = rc100Update(&rc100_rx, ch);
        }

        if (rx_flag == true)
        {
          cmdifPrintf("msg : %d\n", rc100ReadMsg(&rc100_rx));
        }
      }
    }
    else
    {
      ret = false;
    }
  }


  if (ret == false)
  {
    cmdifPrintf( "rc100 begin uart_num\n");
    cmdifPrintf( "rc100 raw uart_num\n");
    cmdifPrintf( "rc100 msg uart_num\n");
  }

  return 0;
}
#endif /* _USE_HW_CMDIF_RC100 */

#endif /* _USE_HW_RC100 */
