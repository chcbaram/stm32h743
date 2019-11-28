/*
 * dac.c
 *
 *  Created on: 2019. 11. 2.
 *      Author: Baram
 */




#include "dac.h"
#include "cmdif.h"

#ifdef _USE_HW_DAC

#define DAC_BUFFER_MAX      (1024*2)



DAC_HandleTypeDef       DacHandle;
TIM_HandleTypeDef       htim;


typedef struct
{
  DAC_ChannelConfTypeDef  sConfig;
  uint32_t                channel;
  uint8_t                 resolution;
  uint8_t                 buffer[DAC_BUFFER_MAX];
} dac_t;



static ring_buf_t tx_buf;
static uint32_t   dac_hz = 0;

static __attribute__((section(".sram_d3")))  dac_t dac_tbl[DAC_MAX_CH];



void dacCmdif(void);
void dacInitTimer(uint32_t hz);



void dacInit(void)
{
  uint32_t i;
  uint32_t j;


  for (i=0; i<DAC_MAX_CH; i++)
  {
    for (j=0; j<DAC_BUFFER_MAX; j++)
    {
      dac_tbl[i].buffer[j] = 0;
    }
  }

  tx_buf.ptr_in  = 0;
  tx_buf.ptr_out = 0;
  tx_buf.p_buf   = (uint8_t *)dac_tbl[0].buffer;
  tx_buf.length  = DAC_BUFFER_MAX;


  DacHandle.Instance = DAC1;


  HAL_DAC_Init(&DacHandle);

  dac_tbl[0].channel    = DAC_CHANNEL_1;
  dac_tbl[0].resolution = 8;


  dac_tbl[0].sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
  dac_tbl[0].sConfig.DAC_Trigger      = DAC_TRIGGER_T6_TRGO;
  dac_tbl[0].sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  dac_tbl[0].sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_ENABLE;
  dac_tbl[0].sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;


  HAL_DAC_ConfigChannel(&DacHandle, &dac_tbl[0].sConfig, dac_tbl[0].channel);
  HAL_DAC_Start_DMA(&DacHandle, dac_tbl[0].channel, (uint32_t *)dac_tbl[0].buffer, DAC_BUFFER_MAX, DAC_ALIGN_8B_R);


  dacSetup(8000);
  dacStart();

  cmdifAdd("dac", dacCmdif);
}


void dacSetup(uint32_t hz)
{
  dac_hz = hz;

  dacInitTimer(dac_hz);
}

void dacStart(void)
{
  HAL_TIM_Base_Start(&htim);
}

void dacStop(void)
{
  HAL_TIM_Base_Stop(&htim);

  tx_buf.ptr_in  = (tx_buf.length - 1) - ((DMA_Stream_TypeDef   *)DacHandle.DMA_Handle1->Instance)->NDTR;
  tx_buf.ptr_out = (tx_buf.length - 1) - ((DMA_Stream_TypeDef   *)DacHandle.DMA_Handle1->Instance)->NDTR;
}

void dacInitTimer(uint32_t hz)
{
  TIM_MasterConfigTypeDef sMasterConfig;

  __HAL_RCC_TIM6_CLK_ENABLE();

  htim.Instance      = TIM6;

  htim.Init.Period            = 10-1;
  htim.Init.Prescaler         = (uint32_t)((SystemCoreClock / 2) / (hz*10)) - 1;
  htim.Init.ClockDivision     = 0;
  htim.Init.CounterMode       = TIM_COUNTERMODE_UP;
  htim.Init.RepetitionCounter = 0;
  HAL_TIM_Base_Init(&htim);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;

  HAL_TIMEx_MasterConfigSynchronization(&htim, &sMasterConfig);
}

uint32_t dacAvailable(void)
{
  uint32_t length = 0;


  tx_buf.ptr_in = (tx_buf.length - 1) - ((DMA_Stream_TypeDef   *)DacHandle.DMA_Handle1->Instance)->NDTR;

  //*
  length = ((tx_buf.length + tx_buf.ptr_out) - tx_buf.ptr_in) % tx_buf.length;
  //*/
  length = tx_buf.length - 1 - length;

  return length;
}

void dacPutch(uint8_t data)
{
  uint32_t index;
  volatile uint32_t next_index;


  index      = tx_buf.ptr_out;
  next_index = tx_buf.ptr_out + 1;

  tx_buf.p_buf[index] = data;
  tx_buf.ptr_out      = next_index % tx_buf.length;
}

void dacWrite(uint8_t *p_data, uint32_t length)
{
  uint32_t i;


  for (i=0; i<length; i++)
  {
    dacPutch(p_data[i]);
  }
}

uint32_t dacGetDebug(void)
{
  return ((DMA_Stream_TypeDef   *)DacHandle.DMA_Handle1->Instance)->NDTR;
}


//-- dacCmdif
//
//extern const uint8_t wave_data[NUM_ELEMENTS];

void dacCmdif()
{
  bool ret = true;
  //uint8_t number;


  if (cmdifGetParamCnt() == 1)
  {
    if(cmdifHasString("demo", 0) == true)
    {
#if 0
      uint32_t index = 0;
      int i;
      uint32_t length;
      int16_t out = 0;


      dacSetup(8000);
      dacStart();

      while(cmdifRxAvailable() == 0)
      {
        delay(1);
        length = dacAvailable();
        for (i=0; i<length-1; i++)
        {
          out  = (wave_data[index] - 128) * 2;
          out += 128;

          if (out > 255) out = 255;
          if (out < 0  ) out = 0;

          dacPutch(out);

          if (index < NUM_ELEMENTS)
          {
            index++;
          }
        }
      }

      dacStop();
#endif
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
    cmdifPrintf( "dac on/off/toggle/demo number ...\n");
  }

  return;
}

volatile uint32_t dac_isr_count = 0;

void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef* hdac)
{
  dac_isr_count++;
}

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef* hdac)
{
}


void DMA2_Stream6_IRQHandler(void)
{
  HAL_DMA_IRQHandler(DacHandle.DMA_Handle1);
}

void DACx_IRQHandler(void)
{
  HAL_DAC_IRQHandler(&DacHandle);
}


void HAL_DAC_MspInit(DAC_HandleTypeDef *hdac)
{
  GPIO_InitTypeDef          GPIO_InitStruct;
  static DMA_HandleTypeDef  hdma_dac1;


  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO clock ****************************************/
  __HAL_RCC_GPIOA_CLK_ENABLE();
  /* DAC Periph clock enable */
  __HAL_RCC_DAC12_CLK_ENABLE();
  /* DMA1 clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /*##-2- Configure peripheral GPIO ##########################################*/
  /* DAC Channel1 GPIO pin configuration */
  GPIO_InitStruct.Pin   = GPIO_PIN_4;
  GPIO_InitStruct.Mode  = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*##-3- Configure the DMA ##########################################*/
  /* Set the parameters to be configured for DACx_DMA1_CHANNEL3 */
  hdma_dac1.Instance                  = DMA2_Stream6;
  hdma_dac1.Init.Request              = DMA_REQUEST_DAC1;

  hdma_dac1.Init.Direction            = DMA_MEMORY_TO_PERIPH;
  hdma_dac1.Init.PeriphInc            = DMA_PINC_DISABLE;
  hdma_dac1.Init.MemInc               = DMA_MINC_ENABLE;
  hdma_dac1.Init.PeriphDataAlignment  = DMA_PDATAALIGN_BYTE;
  hdma_dac1.Init.MemDataAlignment     = DMA_MDATAALIGN_BYTE;
  hdma_dac1.Init.Mode                 = DMA_CIRCULAR;
  hdma_dac1.Init.Priority             = DMA_PRIORITY_HIGH;
  hdma_dac1.Init.FIFOMode             = DMA_FIFOMODE_DISABLE;

  HAL_DMA_Init(&hdma_dac1);

  /* Associate the initialized DMA handle to the DAC handle */
  __HAL_LINKDMA(hdac, DMA_Handle1, hdma_dac1);

  /*##-4- Configure the NVIC for DMA #########################################*/
  /* Enable the DMA1_Channel3 IRQ Channel */
  //HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 5, 0);
  //HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);
}

/**
  * @brief  DeInitializes the DAC MSP.
  * @param  hdac: pointer to a DAC_HandleTypeDef structure that contains
  *         the configuration information for the specified DAC.
  * @retval None
  */
void HAL_DAC_MspDeInit(DAC_HandleTypeDef *hdac)
{
  __HAL_RCC_DAC12_FORCE_RESET();
  __HAL_RCC_DAC12_RELEASE_RESET();

  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4);


  //HAL_DMA_DeInit(hdac->DMA_Handle1);
  //HAL_NVIC_DisableIRQ(DMA2_Stream6_IRQn);
}



#endif
