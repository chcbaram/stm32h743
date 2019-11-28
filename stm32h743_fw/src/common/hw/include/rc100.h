/*
 * rc100.h
 *
 *  Created on: 2017. 2. 13.
 *      Author: baram
 */

#ifndef RC100_H_
#define RC100_H_



#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_RC100



#define RC100_BTN_U		(1)
#define RC100_BTN_D		(2)
#define RC100_BTN_L		(4)
#define RC100_BTN_R		(8)
#define RC100_BTN_1		(16)
#define RC100_BTN_2		(32)
#define RC100_BTN_3		(64)
#define RC100_BTN_4		(128)
#define RC100_BTN_5		(256)
#define RC100_BTN_6		(512)


#define RC100_PACKET_LENGTH 			  6


#define RC100_BUFFER_MAX_CH         2



   
typedef struct
{
  bool     init;

  uint8_t  state;
  uint8_t  index;
  bool     received;
  uint16_t data;

  // for packet parsing
  //
  uint8_t  save_data;
  uint8_t  inv_data;
  uint32_t time_t;
} rc100_t;



bool rc100Init(void);
void rc100Begin(rc100_t *p_rc100);
bool rc100Update(rc100_t *p_rc100, uint8_t data);

uint16_t rc100ReadMsg(rc100_t *p_rc100);

#endif /* _USE_HW_RC100 */

#ifdef __cplusplus
}
#endif

#endif /* RC100_H_ */
