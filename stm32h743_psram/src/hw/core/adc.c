/*
 * adc.c
 *
 *  Created on: 2019. 5. 17.
 *      Author: HanCheol Cho
 */




#include "adc.h"
#include "gpio.h"
#include "cmdif.h"


#ifdef _USE_HW_ADC


typedef struct
{
  bool                    is_init;
  ADC_HandleTypeDef      *hADCx;
  uint32_t                adc_channel;
} adc_tbl_t;



static adc_tbl_t adc_tbl[ADC_MAX_CH];

static ADC_HandleTypeDef hADC1;
static ADC_HandleTypeDef hADC3;
static ADC_ChannelConfTypeDef  sConfig;


#if HW_USE_CMDIF_ADC == 1
static void adcCmdif(void);
#endif


bool adcInit(void)
{
  uint32_t i;
  uint32_t ch;



  for (i=0; i<ADC_MAX_CH; i++)
  {
    adc_tbl[i].is_init = false;
  }


  hADC1.Instance                      = ADC1;
  hADC1.Init.ClockPrescaler           = ADC_CLOCK_ASYNC_DIV4;
  hADC1.Init.Resolution               = ADC_RESOLUTION_12B;
  hADC1.Init.ScanConvMode             = ADC_SCAN_DISABLE;
  hADC1.Init.EOCSelection             = ADC_EOC_SINGLE_CONV;
  hADC1.Init.LowPowerAutoWait         = DISABLE;
  hADC1.Init.ContinuousConvMode       = DISABLE;
  hADC1.Init.NbrOfConversion          = 1;
  hADC1.Init.DiscontinuousConvMode    = DISABLE;
  hADC1.Init.ExternalTrigConv         = ADC_SOFTWARE_START;
  hADC1.Init.ExternalTrigConvEdge     = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hADC1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
  hADC1.Init.Overrun                  = ADC_OVR_DATA_PRESERVED;
  hADC1.Init.LeftBitShift             = ADC_LEFTBITSHIFT_NONE;
  hADC1.Init.OversamplingMode         = DISABLE;

  if (HAL_ADC_Init(&hADC1) != HAL_OK)
  {
    return false;
  }
#if 1
  /* Run the ADC calibration in single-ended mode */
  if (HAL_ADCEx_Calibration_Start(&hADC1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
  {
    /* Calibration Error */
    return false;
  }
#endif

  hADC3.Instance                      = ADC3;
  hADC3.Init.ClockPrescaler           = ADC_CLOCK_ASYNC_DIV4;
  hADC3.Init.Resolution               = ADC_RESOLUTION_12B;
  hADC3.Init.ScanConvMode             = ADC_SCAN_DISABLE;
  hADC3.Init.EOCSelection             = ADC_EOC_SINGLE_CONV;
  hADC3.Init.LowPowerAutoWait         = DISABLE;
  hADC3.Init.ContinuousConvMode       = DISABLE;
  hADC3.Init.NbrOfConversion          = 1;
  hADC3.Init.DiscontinuousConvMode    = DISABLE;
  hADC3.Init.ExternalTrigConv         = ADC_SOFTWARE_START;
  hADC3.Init.ExternalTrigConvEdge     = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hADC3.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
  hADC3.Init.Overrun                  = ADC_OVR_DATA_PRESERVED;
  hADC3.Init.LeftBitShift             = ADC_LEFTBITSHIFT_NONE;
  hADC3.Init.OversamplingMode = DISABLE;

  if (HAL_ADC_Init(&hADC3) != HAL_OK)
  {
    return false;
  }

#if 1
  /* Run the ADC calibration in single-ended mode */
  if (HAL_ADCEx_Calibration_Start(&hADC3, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
  {
    /* Calibration Error */
    return false;
  }
#endif

  // JOY_X
  ch = 0;
  adc_tbl[ch].is_init     = true;
  adc_tbl[ch].hADCx       = &hADC1;
  adc_tbl[ch].adc_channel = ADC_CHANNEL_4;


  // JOY_Y
  ch = 1;
  adc_tbl[ch].is_init     = true;
  adc_tbl[ch].hADCx       = &hADC1;
  adc_tbl[ch].adc_channel = ADC_CHANNEL_8;


  // BAT_ADC
  ch = 2;
  adc_tbl[ch].is_init     = true;
  adc_tbl[ch].hADCx       = &hADC3;
  adc_tbl[ch].adc_channel = ADC_CHANNEL_1;



#if HW_USE_CMDIF_ADC == 1
  cmdifAdd("adc", adcCmdif);
#endif

  return true;
}

uint32_t adcRead(uint8_t ch)
{
  ADC_HandleTypeDef      *hADCx;
  uint32_t adc_value;

  if (adc_tbl[ch].is_init != true)
  {
    return 0;
  }

  hADCx = adc_tbl[ch].hADCx;


  sConfig.Channel = adc_tbl[ch].adc_channel;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_810CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(hADCx, &sConfig) != HAL_OK)
  {
    return 0;
  }
  delay(2);

  HAL_ADC_Start(hADCx);
  HAL_ADC_PollForConversion(hADCx, 10);
  adc_value = HAL_ADC_GetValue(hADCx);

  return adc_value;
}

uint32_t adcRead8(uint8_t ch)
{
  return 0;
}

uint32_t adcRead10(uint8_t ch)
{
  return 0;
}

uint32_t adcRead12(uint8_t ch)
{
  return 0;
}

uint32_t adcRead16(uint8_t ch)
{
  return 0;
}

uint32_t adcReadVoltage(uint8_t ch)
{
  return adcConvVoltage(ch, adcRead(ch));
}

uint32_t adcReadCurrent(uint8_t ch)
{

  return adcConvCurrent(ch, adcRead(ch));
}

uint32_t adcConvVoltage(uint8_t ch, uint32_t adc_value)
{
  uint32_t ret = 0;

  switch(ch)
  {
    case 0:
    case 1:
      ret  = (uint32_t)((adc_value * 3300 * 10) / (4095*10));
      ret += 5;
      ret /= 10;
      break;

    case 2:
      ret  = (uint32_t)((adc_value * 3445 * 26) / (4095*10));
      ret += 5;
      ret /= 10;
      break;

  }

  return ret;
}

uint32_t adcConvCurrent(uint8_t ch, uint32_t adc_value)
{
  return 0;
}

uint8_t  adcGetRes(uint8_t ch)
{
  return 0;
}




void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};


  if(adcHandle->Instance==ADC1)
  {
    /* ADC1 clock enable */
    __HAL_RCC_ADC12_CLK_ENABLE();

    __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP);

    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**ADC1 GPIO Configuration
    PC4     ------> ADC1_INP4
    PC5     ------> ADC1_INP8
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  }
  else if(adcHandle->Instance==ADC3)
  {
    /* ADC3 clock enable */
    __HAL_RCC_ADC3_CLK_ENABLE();

    __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP);

    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**ADC3 GPIO Configuration
    PC3_C     ------> ADC3_INP1
    */
    HAL_SYSCFG_AnalogSwitchConfig(SYSCFG_SWITCH_PC3, SYSCFG_SWITCH_PC3_OPEN);
  }
}


void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{
  if(adcHandle->Instance==ADC1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_ADC12_CLK_DISABLE();

    /**ADC1 GPIO Configuration
    PC4     ------> ADC1_INP4
    PC5     ------> ADC1_INP8
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_4|GPIO_PIN_5);
  }
  else if(adcHandle->Instance==ADC3)
  {
    /* Peripheral clock disable */
    __HAL_RCC_ADC3_CLK_DISABLE();
  }

}



#if HW_USE_CMDIF_ADC == 1
//-- adcCmdif
//
void adcCmdif(void)
{
  bool ret = true;


  if (cmdifGetParamCnt() == 1)
  {
    if (cmdifHasString("show", 0) == true)
    {
      while(cmdifRxAvailable() == 0)
      {
        for (int i=0; i<ADC_MAX_CH; i++)
        {
          cmdifPrintf("%04d ", adcRead(i));
        }
        cmdifPrintf("\r\n");
        delay(50);
      }
    }
    else
    {
      ret = false;
    }
  }
  else if (cmdifGetParamCnt() == 2)
  {
    if (cmdifHasString("show", 0) == true && cmdifHasString("voltage", 1) == true)
    {
      while(cmdifRxAvailable() == 0)
      {
        for (int i=0; i<ADC_MAX_CH; i++)
        {
          uint32_t adc_data;

          adc_data = adcReadVoltage(i);

          cmdifPrintf("%d.%d ", adc_data/10, adc_data%10);
        }
        cmdifPrintf("\r\n");
        delay(50);
      }
    }
    else
    {
      ret = false;
    }
  }
  else
  {
    ret = false;
  }


  if (ret == false)
  {
    cmdifPrintf( "adc show\n");
    cmdifPrintf( "adc show voltage\n");
  }
}
#endif

#endif
