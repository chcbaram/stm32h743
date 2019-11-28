/*
 * joypad.h
 *
 *  Created on: 2019. 11. 14.
 *      Author: Baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_JOYPAD_H_
#define SRC_COMMON_HW_INCLUDE_JOYPAD_H_


#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_JOYPAD


enum JoypadKey
{
  JOYPAD_UP,
  JOYPAD_DOWN,
  JOYPAD_LEFT,
  JOYPAD_RIGHT
};


bool joypadInit(void);
void joypadUpdate(void);

int32_t joypadGetX(void);
int32_t joypadGetY(void);
int32_t joypadGetKey(void);

#endif


#ifdef __cplusplus
}
#endif


#endif /* SRC_COMMON_HW_INCLUDE_JOYPAD_H_ */
