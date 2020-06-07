/*
 * osd.h
 *
 *  Created on: 2019. 11. 14.
 *      Author: Baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_OSD_H_
#define SRC_COMMON_HW_INCLUDE_OSD_H_


#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_OSD



bool osdInit(void);
void osdUpdate(void);

int32_t osdGetWidth(void);
int32_t osdGetHeight(void);
uint32_t osdGetStrWidth(const char *fmt, ...);

void osdDrawPixel(uint16_t x_pos, uint16_t y_pos, uint32_t rgb_code);
void osdClear(uint32_t rgb_code);
void osdClear(uint32_t rgb_code);
void osdDisplayOff(void);
void osdDisplayOn(void);
void osdDrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,uint16_t color);
void osdDrawFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void osdDrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void osdDrawFillScreen(uint16_t color);
void osdPrintf(int x, int y, uint16_t color,  const char *fmt, ...);


#endif


#ifdef __cplusplus
}
#endif


#endif /* SRC_COMMON_HW_INCLUDE_OSD_H_ */
