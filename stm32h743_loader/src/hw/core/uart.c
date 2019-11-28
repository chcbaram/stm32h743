/*
 * uart.c
 *
 *  Created on: 2018. 8. 25.
 *      Author: Baram
 */




#include "uart.h"
#include <conio.h>





typedef struct
{
  bool     is_open;
  bool     is_consol;
  bool     is_port_name;
  uint32_t baud;

  HANDLE   serial_handle;
  char     port_name[256];
  uint8_t  received_data;
} uart_t;



uart_t   uart_tbl[UART_MAX_CH];


static uint32_t uartOpenPC(uint8_t channel, char *port_name, uint32_t baud);




void uartInit(void)
{
  uint8_t i;


  for (i=0; i<UART_MAX_CH; i++)
  {
    uart_tbl[i].is_open   = false;
    uart_tbl[i].is_consol = false;
    uart_tbl[i].is_port_name = false;
  }
}

bool uartOpen(uint8_t channel, uint32_t baud)
{
  bool ret = false;
  uart_t *p_uart;

  switch(channel)
  {
    case _DEF_UART1:
      p_uart = &uart_tbl[channel];

      p_uart->baud      = baud;
      p_uart->is_open   = true;
      p_uart->is_consol = true;
      ret = true;
      break;

    case _DEF_UART2:
      p_uart = &uart_tbl[channel];


      if (p_uart->is_port_name == true)
      {
        if (uartOpenPC(channel, p_uart->port_name, baud) == OK)
        {
          ret = true;
        }
      }
      break;

  }

  return ret;
}

uint32_t uartOpenPC(uint8_t channel, char *port_name, uint32_t baud)
{
  uint32_t err_code  = OK;
  uart_t *p_uart = &uart_tbl[channel];

  DCB dcb;
  COMMTIMEOUTS timeouts;
  DWORD dwError;


  if (channel >= UART_MAX_CH)
  {
    return 1;
  }


  p_uart->baud = baud;


  p_uart->serial_handle = CreateFileA(p_uart->port_name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (p_uart->serial_handle == INVALID_HANDLE_VALUE)
  {
    printf("Error opening serial port!\n");
    return 2;
  }

  while(1)
  {
    dcb.DCBlength = sizeof(DCB);
    if (GetCommState(p_uart->serial_handle, &dcb) == FALSE)
    {
      err_code = 1;
      break;
    }

    // Set baudrate
    dcb.BaudRate = (DWORD)baud;
    dcb.ByteSize = 8;                    // Data bit = 8bit
    dcb.Parity   = NOPARITY;             // No parity
    dcb.StopBits = ONESTOPBIT;           // Stop bit = 1
    dcb.fParity  = NOPARITY;             // No Parity check
    dcb.fBinary  = 1;                    // Binary mode
    dcb.fNull    = 0;                    // Get Null byte
    dcb.fAbortOnError = 0;
    dcb.fErrorChar    = 0;
    // Not using XOn/XOff
    dcb.fOutX = 0;
    dcb.fInX  = 0;
    // Not using H/W flow control
    dcb.fDtrControl = DTR_CONTROL_DISABLE;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;
    dcb.fDsrSensitivity = 0;
    dcb.fOutxDsrFlow = 0;
    dcb.fOutxCtsFlow = 0;

    if (SetCommState(p_uart->serial_handle, &dcb) == FALSE)
    {
      DWORD dwError = GetLastError();
      err_code = 2;

      printf("SetCommState err: %d\n", (int)dwError);
      break;
    }

    if (SetCommMask(p_uart->serial_handle, 0) == FALSE) // Not using Comm event
    {
      err_code = 3;
      break;
    }
    if (SetupComm(p_uart->serial_handle, 4096, 4096) == FALSE) // Buffer size (Rx,Tx)
    {
      err_code = 4;
      break;
    }
    if (PurgeComm(p_uart->serial_handle, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR) == FALSE) // Clear buffer
    {
      err_code = 5;
      break;
    }
    if (ClearCommError(p_uart->serial_handle, &dwError, NULL) == FALSE)
    {
      err_code = 6;
      break;
    }

    if (GetCommTimeouts(p_uart->serial_handle, &timeouts) == FALSE)
    {
      err_code = 7;
      break;
    }
    // Timeout (Not using timeout)
    // Immediatly return
    timeouts.ReadIntervalTimeout         = 0;
    timeouts.ReadTotalTimeoutMultiplier  = 0;
    timeouts.ReadTotalTimeoutConstant    = 1; // must not be zero.
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant   = 0;
    if (SetCommTimeouts(p_uart->serial_handle, &timeouts) == FALSE)
    {
      err_code = 8;
      break;
    }
    break;
  }

  if (err_code != OK)
  {
    CloseHandle(p_uart->serial_handle);
    //printf("uartOpen err : %x \n", err_code);
  }
  else
  {
    p_uart->is_open = true;
  }

  return err_code;
}

bool uartClose(uint8_t channel)
{
  bool ret = false;
  uart_t *p_uart = &uart_tbl[channel];


  if (channel >= UART_MAX_CH)
  {
    return false;
  }
  if (uart_tbl[channel].is_open == false)
  {
    return false;
  }

  if (p_uart->is_consol == true)
  {

  }
  else
  {
    CloseHandle(p_uart->serial_handle);
    p_uart->is_open = false;
  }


  return ret;
}

void uartSetPortName(uint8_t channel, char *port_name)
{
  uart_t *p_uart = &uart_tbl[channel];


  if (channel >= UART_MAX_CH)
  {
    return;
  }

  sprintf(p_uart->port_name, "\\\\.\\%s", port_name);
  p_uart->is_port_name  = true;
}

uint32_t uartAvailable(uint8_t channel)
{
  uint32_t ret = 0;
  uart_t *p_uart = &uart_tbl[channel];


  if (channel >= UART_MAX_CH)
  {
    return 0;
  }
  if (p_uart->is_open == false)
  {
    return 0;
  }


  if (p_uart->is_consol == true)
  {
    if(kbhit())
    {
      ret = 1;
    }
  }
  else
  {
    int32_t length = 0;
    uart_t *p_uart = &uart_tbl[channel];
    DWORD dwRead = 0;
    uint8_t data;

    if (ReadFile(p_uart->serial_handle, &data, (DWORD)1, &dwRead, NULL) == TRUE)
    {
      if (dwRead != 1)
      {
        length = 0;
      }
      else
      {
        length = 1;
        p_uart->received_data = data;
      }
    }
    ret = length;
  }

  return ret;
}

void uartFlush(uint8_t channel)
{
}

void uartPutch(uint8_t channel, uint8_t ch)
{
  uartWrite(channel, &ch, 1 );
}

uint8_t uartGetch(uint8_t channel)
{
  uint8_t ret = 0;


  if (channel >= UART_MAX_CH)
  {
    return 0;
  }
  if (uart_tbl[channel].is_open == false)
  {
    return 0;
  }


  while(1)
  {
    if (uartAvailable(channel) > 0)
    {
      ret = uartRead(channel);
      break;
    }
  }

  return ret;
}

int32_t uartWrite(uint8_t channel, uint8_t *p_data, uint32_t length)
{
  int32_t ret = 0;
  uart_t *p_uart = &uart_tbl[channel];


  if (channel >= UART_MAX_CH)
  {
    return 0;
  }
  if (p_uart->is_open == false)
  {
    return 0;
  }


  if (p_uart->is_consol == true)
  {
    for (int i=0; i<length; i++)
    {
      putch(p_data[i]);
    }
  }
  else
  {
    DWORD dwWrite = 0;


    if (WriteFile(p_uart->serial_handle, p_data, (DWORD)length, &dwWrite, NULL) == FALSE)
    {
      ret = 0;
    }
    else
    {
      ret = dwWrite;
    }
  }

  return ret;
}

uint8_t uartRead(uint8_t channel)
{
  uint8_t ret = 0;
  uart_t *p_uart = &uart_tbl[channel];


  if (channel >= UART_MAX_CH)
  {
    return 0;
  }
  if (p_uart->is_open == false)
  {
    return 0;
  }


  if (p_uart->is_consol == true)
  {
    ret = getch();
  }
  else
  {
    ret = p_uart->received_data;
  }

  return ret;
}

int32_t uartPrintf(uint8_t channel, const char *fmt, ...)
{
  int32_t ret = 0;
  va_list arg;
  va_start (arg, fmt);
  int32_t len;
  char print_buffer[256];


  len = vsnprintf(print_buffer, 255, fmt, arg);
  va_end (arg);

  ret = uartWrite(channel, (uint8_t *)print_buffer, len);

  return ret;
}

