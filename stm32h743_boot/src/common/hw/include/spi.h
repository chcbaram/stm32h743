/*
 * spi.h
 *
 *  Created on: 2019. 5. 2.
 *      Author: HanCheol Cho
 */

#ifndef SRC_COMMON_HW_INCLUDE_SPI_H_
#define SRC_COMMON_HW_INCLUDE_SPI_H_


#ifdef __cplusplus
 extern "C" {
#endif

#include "hw_def.h"


#ifdef _USE_HW_SPI


#define SPI_MAX_CH   HW_SPI_MAX_CH


#define SPI_CLOCK_DIV_1       0
#define SPI_CLOCK_DIV_2       SPI_BAUDRATEPRESCALER_2
#define SPI_CLOCK_DIV_4       SPI_BAUDRATEPRESCALER_4
#define SPI_CLOCK_DIV_8       SPI_BAUDRATEPRESCALER_8
#define SPI_CLOCK_DIV_16      SPI_BAUDRATEPRESCALER_16
#define SPI_CLOCK_DIV_32      SPI_BAUDRATEPRESCALER_32
#define SPI_CLOCK_DIV_64      SPI_BAUDRATEPRESCALER_64
#define SPI_CLOCK_DIV_128     SPI_BAUDRATEPRESCALER_128
#define SPI_CLOCK_DIV_256     SPI_BAUDRATEPRESCALER_256


#define SPI_MSB_FIRST         SPI_FIRSTBIT_MSB
#define SPI_LSB_FIRST         SPI_FIRSTBIT_LSB

#define SPI_MODE0             0
#define SPI_MODE1             1
#define SPI_MODE2             2
#define SPI_MODE3             3



bool      spiInit(void);

void      spiBegin(uint8_t spi_ch);
void      spiSetBitOrder(uint8_t spi_ch, uint8_t bitOrder);
void      spiSetClockDivider(uint8_t spi_ch, uint8_t clockDiv);
void      spiSetDataMode(uint8_t spi_ch, uint8_t dataMode);

bool      spiWrite(uint8_t spi_ch, uint8_t *p_data, uint32_t length);
bool      spiWrite8(uint8_t spi_ch, uint8_t data);
bool      spiWrite16(uint8_t spi_ch, uint16_t data);

bool      spiRead(uint8_t spi_ch, uint8_t *p_data, uint32_t length);
bool      spiRead8(uint8_t spi_ch, uint8_t *p_data);
bool      spiRead16(uint8_t spi_ch, uint16_t *p_data);

bool      spiTransfer(uint8_t spi_ch, uint8_t *p_tx_data, uint8_t *p_rx_data, uint32_t length);
uint8_t   spiTransfer8(uint8_t spi_ch, uint8_t data);
uint16_t  spiTransfer16(uint8_t spi_ch, uint16_t data);

void      spiSetTimeout(uint8_t ch, uint32_t timeout);
uint32_t  spiGetTimeout(uint8_t ch);

void      spiSetCS(uint8_t ch, uint8_t state);

#endif /* _USE_HW_SPI */


#ifdef __cplusplus
 }
#endif


#endif /* SRC_COMMON_HW_INCLUDE_SPI_H_ */
