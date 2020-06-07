/*
 * lcd.h
 *
 *  Created on: Feb 11, 2018
 *      Author: opus
 */

#ifndef LCD_H_
#define LCD_H_

#include "hw_def.h"

#ifdef _USE_HW_LCD



bool lcdInit(void);
bool lcdIsInit(void);
void lcdReset(void);

uint8_t lcdGetBackLight(void);
void    lcdSetBackLight(uint8_t value);

uint32_t lcdReadPixel(uint16_t x_pos, uint16_t y_pos);
void lcdClear(uint32_t rgb_code);

bool lcdDrawAvailable(void);
void lcdRequestDraw(void);
void lcdUpdateDraw(void);

void lcdDisplayOff(void);
void lcdDisplayOn(void);

int32_t lcdGetWidth(void);
int32_t lcdGetHeight(void);

uint16_t *lcdGetFrameBuffer(void);
uint16_t *lcdGetCurrentFrameBuffer(void);
void lcdSetDoubleBuffer(bool enable);
bool lcdGetDoubleBuffer(void);

void lcdDrawPixel(uint16_t x_pos, uint16_t y_pos, uint32_t rgb_code);
void lcdDrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,uint16_t color);
void lcdDrawVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
void lcdDrawHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
void lcdDrawFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void lcdDrawFillScreen(uint16_t color);
void lcdDrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void lcdPrintf(int x, int y, uint16_t color,  const char *fmt, ...);

#endif /* _USE_HW_LCD */


#endif /* LCD_H_ */
