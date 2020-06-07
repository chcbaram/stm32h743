



//	Deze library is door Olivier Van den Eede 2016 geschreven en aangepast voor gebruik met
//	Stm32 microcontrollers en maakt gebruik van de HAL-i2c library's.
//
//	Deze library is gemaakt om gebruik te kunnen maken van een ssd1306 gestuurd oled display.
//	Voor het gebruik moeten zeker de onderstaande defines juist ingesteld worden.
//	Zoals de gebruikte i2c poort en de groote van het scherm.
//
//	De library maakt gebruik van 2 files (fonts.c/h) Hierin staan 3 fonts beschreven.
//	Deze fonts kunnen zo gebruikt worden: 	- Font_7x10
//											- Font_11x18
//											- Font_16x26

#ifndef ssd1306
#define ssd1306

#include "hw_def.h"

#ifdef __cplusplus
 extern "C" {
#endif

#include "ssd1306_fonts.h"


// i2c port naam in main programma gegenereerd door cube
#define SSD1306_I2C_PORT		    _DEF_I2C1
// I2C address 
#define SSD1306_I2C_ADDR        (0x78>>1)
// SSD1306 width in pixels
#define SSD1306_WIDTH           128
// SSD1306 LCD height in pixels
#define SSD1306_HEIGHT          64
#define ABS(x)   ((x) > 0 ? (x) : -(x))


//
//	Enum voor de kleuren van het scherm Black en White
//
typedef enum {
	Black = 0x00, /*!< Black color, no pixel */
	White = 0x01  /*!< Pixel is set. Color depends on LCD */
} SSD1306_COLOR;

//
//	Struct om wijzigingen bij te houden
//
typedef struct {
	uint16_t CurrentX;
	uint16_t CurrentY;
	uint8_t Inverted;
	uint8_t Initialized;
} SSD1306_t;


//
//	De functies definities van de functies die gebruikt kunnen worden
//
uint8_t ssd1306Init(void);
void ssd1306_Fill(SSD1306_COLOR color);
bool ssd1306_UpdateScreen(void);
void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color);
char ssd1306_WriteChar(char ch, FontDef Font, SSD1306_COLOR color);
char ssd1306_WriteString(char* str, FontDef Font, SSD1306_COLOR color);
void ssd1306_SetCursor(uint8_t x, uint8_t y);
void ssd1306_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, SSD1306_COLOR c);
void ssd1306_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR c);
void ssd1306_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR c);
void ssd1306_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_COLOR color);
void ssd1306_DrawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_COLOR color);
void ssd1306_DrawCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR c);
void ssd1306_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR c);

void ssd1306_ON(void);
void ssd1306_OFF(void);

void ssd1306Printf(uint16_t x, uint16_t y, FontDef Font, const char *fmt, ...);
void ssd1306PrintfH(int x, int y, const char *fmt, ...);
bool ssd1306Update(void);
void ssd1306Clear(void);

#ifdef __cplusplus
 }
#endif
#endif
