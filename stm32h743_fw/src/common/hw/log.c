/*
 * log.c
 *
 *  Created on: 2019. 3. 13.
 *      Author: HanCheol Cho
 */



#include "log.h"

#ifdef _USE_HW_LOG

#include "hw.h"


#define LOG_MODE_MAX  5


//-- Internal Variables
//
static bool    log_enable[LOG_MODE_MAX] = {false, false, false, false, false};
static uint8_t log_ch        = LOG_CH;

#ifdef _USE_HW_RTOS
static osSemaphoreId  sem_log;
#endif

//-- External Variables
//


//-- Internal Functions
//



//-- External Functions
//




bool logInit(void)
{
#ifdef _USE_HW_RTOS
  osSemaphoreDef(sem_log);
  sem_log = osSemaphoreCreate(osSemaphore(sem_log) , 1);
#endif
  return true;
}

void logOn(uint8_t mode)
{
  if (mode >= LOG_MODE_MAX)
  {
    return;
  }
  log_enable[mode] = true;
}

void logOff(uint8_t mode)
{
  if (mode >= LOG_MODE_MAX)
  {
    return;
  }
  log_enable[mode] = false;
}

void logSetChannel(uint8_t ch)
{
  if (ch >= UART_MAX_CH)
  {
    return;
  }
  log_ch = ch;
}

void logPrintf(uint8_t mode, const char *fmt, ...)
{
  va_list arg;
  va_start (arg, fmt);
  int32_t len;
  char print_buffer[255];

  if (log_enable[mode] == false)
  {
    return;
  }
  if (log_ch >= UART_MAX_CH)
  {
    return;
  }

  len = vsnprintf(print_buffer, 255, fmt, arg);
  va_end (arg);

#ifdef _USE_HW_RTOS
  if(osSemaphoreWait(sem_log, 100) == osOK)
  {
#endif
  switch(mode)
  {
    case _DEF_LOG_ERR:
      uartWrite(log_ch, (uint8_t *)"[ERR ] ", 7);
      break;

    case _DEF_LOG_WARN:
      uartWrite(log_ch, (uint8_t *)"[WARN] ", 7);
      break;

    case _DEF_LOG_INFO:
      uartWrite(log_ch, (uint8_t *)"[INFO] ", 7);
      break;

    case _DEF_LOG_DIAG:
      uartWrite(log_ch, (uint8_t *)"[DIAG] ", 7);
      break;

  }

  uartWrite(log_ch, (uint8_t *)print_buffer, len);

#ifdef _USE_HW_RTOS
  osSemaphoreRelease(sem_log);
  }
#endif

  return;
}


#endif
