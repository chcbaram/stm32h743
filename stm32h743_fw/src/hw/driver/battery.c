/*
 * battery.c
 *
 *  Created on: 2019. 11. 14.
 *      Author: Baram
 */




#include "battery.h"
#include "adc.h"


#define BAT_ADC_MAX_COUNT     100


static int32_t bat_level = 0;
static int32_t bat_vol;
static uint8_t  adc_ch = 2;
static uint32_t bat_max = 410;
static uint32_t bat_min = 310;

static uint32_t adc_data[BAT_ADC_MAX_COUNT];


bool batteryInit(void)
{
  uint32_t i;

  for (i=0; i<BAT_ADC_MAX_COUNT; i++)
  {
    adc_data[i] = 0;
  }

  return true;
}

void batteryUpdate(void)
{
  static uint32_t pre_time;
  static uint32_t index = 0;
  static uint32_t last_level = 0;
  uint32_t i;
  int32_t sum;
  int32_t value = 0;
  int32_t vol = 0;


  if (millis()-pre_time >= 10)
  {
    pre_time = millis();

    adc_data[index] = adcReadVoltage(adc_ch);
    index = (index + 1) % BAT_ADC_MAX_COUNT;


    sum = 0;
    for (i=0; i<BAT_ADC_MAX_COUNT; i++)
    {
      sum += adc_data[i];
    }

    bat_vol = sum/BAT_ADC_MAX_COUNT;
    vol   = constrain(bat_vol, bat_min, bat_max);

    value = map(vol, bat_min, bat_max, 0, 100);

    if (abs(value-last_level) > 5 || value == 100)
    {
      bat_level = value;
      last_level = value;
    }
  }
}

int32_t batteryGetLevel(void)
{
  return bat_level;
}

int32_t batteryGetVoltage(void)
{
  return bat_vol;
}
