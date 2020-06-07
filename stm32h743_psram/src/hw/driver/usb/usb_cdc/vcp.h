/*
 * vcp.h
 *
 *  Created on: 2017. 2. 13.
 *      Author: baram
 */

#ifndef VCP_H_
#define VCP_H_



#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"


bool     vcpInit(void);
bool     vcpFlush(void);
uint32_t vcpAvailable(void);
void     vcpPutch(uint8_t ch);
uint8_t  vcpGetch(void);
int32_t  vcpWrite(uint8_t *p_data, uint32_t length);
int32_t  vcpWriteTimeout(uint8_t *p_data, uint32_t length, uint32_t timeout);
uint8_t  vcpRead(void);
uint32_t vcpGetTxErrCount(void);
uint32_t vcpGetTxCount(void);
uint32_t vcpGetTxRetryCount(void);
bool     vcpIsConnected(void);
uint32_t vcpGetBaud(void);

int32_t  vcpPrintf( const char *fmt, ...);



#ifdef __cplusplus
}
#endif

#endif /* VCP_H_ */
