/*
 * uart.h
 *
 *  Created on: 2019. 3. 7.
 *      Author: HanCheol Cho
 */

#ifndef SRC_COMMON_HW_UART_H_
#define SRC_COMMON_HW_UART_H_


#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"


#ifdef _USE_HW_UART

#define UART_MAX_CH     HW_UART_MAX_CH


bool     uartInit(void);
bool     uartOpen(uint8_t channel, uint32_t baud);
bool     uartClose(uint8_t channel);
uint32_t uartAvailable(uint8_t channel);
void     uartFlush(uint8_t channel);
void     uartPutch(uint8_t channel, uint8_t ch);
uint8_t  uartGetch(uint8_t channel);
int32_t  uartWrite(uint8_t channel, uint8_t *p_data, uint32_t length);
uint8_t  uartRead(uint8_t channel);
int32_t  uartPrintf(uint8_t channel, const char *fmt, ...);
bool     uartSendBreak(uint8_t channel);
void     uartSetTxDoneISR(uint8_t channel, void (*func)(void));

uint32_t uartGetErrCnt(uint8_t channel);
void     uartResetErrCnt(uint8_t channel);

#endif

#ifdef __cplusplus
}
#endif



#endif /* SRC_COMMON_HW_UART_H_ */
