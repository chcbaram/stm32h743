/*
 * sdram.c
 *
 *  Created on: 2019. 6. 23.
 *      Author: Baram
 */




#include "sdram.h"
#include "cmdif.h"




#define SDRAM_OK         ((uint8_t)0x00)
#define SDRAM_ERROR      ((uint8_t)0x01)

#define SDRAM_DEVICE_ADDR  ((uint32_t)0xD0000000)
#define SDRAM_DEVICE_SIZE  ((uint32_t)32*1024*1024)  /* SDRAM device size in MBytes */



uint8_t BSP_SDRAM_Init(void);
uint8_t BSP_SDRAM_DeInit(void);
void    BSP_SDRAM_Initialization_sequence(uint32_t RefreshCount);
uint8_t BSP_SDRAM_ReadData(uint32_t uwStartAddress, uint32_t *pData, uint32_t uwDataSize);
uint8_t BSP_SDRAM_ReadData_DMA(uint32_t uwStartAddress, uint32_t *pData, uint32_t uwDataSize);
uint8_t BSP_SDRAM_WriteData(uint32_t uwStartAddress, uint32_t *pData, uint32_t uwDataSize);
uint8_t BSP_SDRAM_WriteData_DMA(uint32_t uwStartAddress, uint32_t *pData, uint32_t uwDataSize);
uint8_t BSP_SDRAM_Sendcmd(FMC_SDRAM_CommandTypeDef *SdramCmd);

/* These functions can be modified in case the current settings (e.g. DMA stream)
   need to be changed for specific application needs */
void    BSP_SDRAM_MspInit(SDRAM_HandleTypeDef  *hsdram, void *Params);
void    BSP_SDRAM_MspDeInit(SDRAM_HandleTypeDef  *hsdram, void *Params);


static bool is_init = false;


#if HW_USE_CMDIF_SDRAM == 1
void sdramCmdifInit(void);
void sdramCmdif(void);
#endif



bool sdramInit(void)
{
  bool ret = true;;

  if (BSP_SDRAM_Init() != 0x00)
  {
    ret = false;
  }

  if (ret == true)
  {
    logPrintf("SDRAM %dMB \t\t: OK\r\n", (int)(SDRAM_DEVICE_SIZE/1024/1024));
  }
  else
  {
    logPrintf("SDRAM  \t\t: Fail\r\n");
  }

#if HW_USE_CMDIF_SDRAM == 1
  sdramCmdifInit();
#endif

  is_init = ret;

  return ret;
}

bool sdramIsInit(void)
{
  return is_init;
}

uint32_t sdramGetAddr(void)
{
  return SDRAM_DEVICE_ADDR;
}

uint32_t sdramGetLength(void)
{
  return SDRAM_DEVICE_SIZE;
}

bool sdramTest(void)
{
  uint32_t *p_data = (uint32_t *)SDRAM_DEVICE_ADDR;
  uint32_t i;


  for (i=0; i<SDRAM_DEVICE_SIZE/4; i++)
  {
    p_data[i] = i;
  }

  for (i=0; i<SDRAM_DEVICE_SIZE/4; i++)
  {
    if (p_data[i] != i)
    {
      return false;
    }
  }

  for (i=0; i<SDRAM_DEVICE_SIZE/4; i++)
  {
    p_data[i] = 0x5555AAAA;
  }
  for (i=0; i<SDRAM_DEVICE_SIZE/4; i++)
  {
    if (p_data[i] != 0x5555AAAA)
    {
      return false;
    }
  }

  for (i=0; i<SDRAM_DEVICE_SIZE/4; i++)
  {
    p_data[i] = 0xAAAA5555;
  }
  for (i=0; i<SDRAM_DEVICE_SIZE/4; i++)
  {
    if (p_data[i] != 0xAAAA5555)
    {
      return false;
    }
  }

  return true;
}




/* #define SDRAM_MEMORY_WIDTH            FMC_SDRAM_MEM_BUS_WIDTH_8  */
#define SDRAM_MEMORY_WIDTH            FMC_SDRAM_MEM_BUS_WIDTH_16
/* #define SDRAM_MEMORY_WIDTH               FMC_SDRAM_MEM_BUS_WIDTH_32 */

#define SDCLOCK_PERIOD                   FMC_SDRAM_CLOCK_PERIOD_2
//#define SDCLOCK_PERIOD                   FMC_SDRAM_CLOCK_PERIOD_3

#define REFRESH_COUNT                    ((uint32_t)0x0603)   /* SDRAM refresh counter (100Mhz SD clock) */

#define SDRAM_TIMEOUT                    ((uint32_t)0xFFFF)

/* DMA definitions for SDRAM DMA transfer */
#define __MDMAx_CLK_ENABLE                 __HAL_RCC_MDMA_CLK_ENABLE
#define __MDMAx_CLK_DISABLE                __HAL_RCC_MDMA_CLK_DISABLE
#define SDRAM_MDMAx_CHANNEL               MDMA_Channel0
#define SDRAM_MDMAx_IRQn                  MDMA_IRQn

/**
  * @brief  FMC SDRAM Mode definition register defines
  */
#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)



SDRAM_HandleTypeDef sdramHandle;
static FMC_SDRAM_TimingTypeDef Timing;
static FMC_SDRAM_CommandTypeDef Command;



uint8_t BSP_SDRAM_Init(void)
{
  static uint8_t sdramstatus = SDRAM_OK;
  /* SDRAM device configuration */
  sdramHandle.Instance = FMC_SDRAM_DEVICE;

  /* Timing configuration for 100Mhz as SDRAM clock frequency (System clock is up to 200Mhz) */
#if 1
  Timing.LoadToActiveDelay    = 2;
  Timing.ExitSelfRefreshDelay = 7;
  Timing.SelfRefreshTime      = 4;
  Timing.RowCycleDelay        = 7;
  Timing.WriteRecoveryTime    = 2;
  Timing.RPDelay              = 2;
  Timing.RCDDelay             = 2;
#else
  Timing.LoadToActiveDelay    = 16;
  Timing.ExitSelfRefreshDelay = 16;
  Timing.SelfRefreshTime      = 16;
  Timing.RowCycleDelay        = 16;
  Timing.WriteRecoveryTime    = 16;
  Timing.RPDelay              = 16;
  Timing.RCDDelay             = 16;
#endif

  sdramHandle.Init.SDBank             = FMC_SDRAM_BANK2;
  sdramHandle.Init.ColumnBitsNumber   = FMC_SDRAM_COLUMN_BITS_NUM_9;
  sdramHandle.Init.RowBitsNumber      = FMC_SDRAM_ROW_BITS_NUM_13;
  sdramHandle.Init.MemoryDataWidth    = SDRAM_MEMORY_WIDTH;
  sdramHandle.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
  sdramHandle.Init.CASLatency         = FMC_SDRAM_CAS_LATENCY_3;
  sdramHandle.Init.WriteProtection    = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  sdramHandle.Init.SDClockPeriod      = SDCLOCK_PERIOD;
  sdramHandle.Init.ReadBurst          = FMC_SDRAM_RBURST_ENABLE;
  sdramHandle.Init.ReadPipeDelay      = FMC_SDRAM_RPIPE_DELAY_0;

  /* SDRAM controller initialization */

  BSP_SDRAM_MspInit(&sdramHandle, NULL); /* __weak function can be rewritten by the application */

  if(HAL_SDRAM_Init(&sdramHandle, &Timing) != HAL_OK)
  {
    sdramstatus = SDRAM_ERROR;
  }
  else
  {
    /* SDRAM initialization sequence */
    BSP_SDRAM_Initialization_sequence(REFRESH_COUNT);
  }

  return sdramstatus;
}

/**
  * @brief  DeInitializes the SDRAM device.
  * @retval SDRAM status
  */
uint8_t BSP_SDRAM_DeInit(void)
{
  static uint8_t sdramstatus = SDRAM_OK;
  /* SDRAM device de-initialization */
  sdramHandle.Instance = FMC_SDRAM_DEVICE;

  if(HAL_SDRAM_DeInit(&sdramHandle) != HAL_OK)
  {
    sdramstatus = SDRAM_ERROR;
  }
  else
  {
    /* SDRAM controller de-initialization */
    BSP_SDRAM_MspDeInit(&sdramHandle, NULL);
  }

  return sdramstatus;
}

/**
  * @brief  Programs the SDRAM device.
  * @param  RefreshCount: SDRAM refresh counter value
  * @retval None
  */
void BSP_SDRAM_Initialization_sequence(uint32_t RefreshCount)
{
  __IO uint32_t tmpmrd = 0;

  /* Step 1: Configure a clock configuration enable command */
  Command.CommandMode            = FMC_SDRAM_CMD_CLK_ENABLE;
  Command.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK2;
  Command.AutoRefreshNumber      = 1;
  Command.ModeRegisterDefinition = 0;

  /* Send the command */
  HAL_SDRAM_SendCommand(&sdramHandle, &Command, SDRAM_TIMEOUT);

  /* Step 2: Insert 100 us minimum delay */
  /* Inserted delay is equal to 1 ms due to systick time base unit (ms) */
  HAL_Delay(1);

  /* Step 3: Configure a PALL (precharge all) command */
  Command.CommandMode            = FMC_SDRAM_CMD_PALL;
  Command.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK2;
  Command.AutoRefreshNumber      = 1;
  Command.ModeRegisterDefinition = 0;

  /* Send the command */
  HAL_SDRAM_SendCommand(&sdramHandle, &Command, SDRAM_TIMEOUT);

  /* Step 4: Configure an Auto Refresh command */
  Command.CommandMode            = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
  Command.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK2;
  Command.AutoRefreshNumber      = 8;
  Command.ModeRegisterDefinition = 0;

  /* Send the command */
  HAL_SDRAM_SendCommand(&sdramHandle, &Command, SDRAM_TIMEOUT);

  /* Step 5: Program the external memory mode register */
  tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1          |\
                     SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |\
                     SDRAM_MODEREG_CAS_LATENCY_3           |\
                     SDRAM_MODEREG_OPERATING_MODE_STANDARD |\
                     SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

  Command.CommandMode            = FMC_SDRAM_CMD_LOAD_MODE;
  Command.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK2;
  Command.AutoRefreshNumber      = 1;
  Command.ModeRegisterDefinition = tmpmrd;

  /* Send the command */
  HAL_SDRAM_SendCommand(&sdramHandle, &Command, SDRAM_TIMEOUT);

  /* Step 6: Set the refresh rate counter */
  /* Set the device refresh rate */
  HAL_SDRAM_ProgramRefreshRate(&sdramHandle, RefreshCount);
}

/**
  * @brief  Reads an amount of data from the SDRAM memory in polling mode.
  * @param  uwStartAddress: Read start address
  * @param  pData: Pointer to data to be read
  * @param  uwDataSize: Size of read data from the memory
  * @retval SDRAM status
  */
uint8_t BSP_SDRAM_ReadData(uint32_t uwStartAddress, uint32_t *pData, uint32_t uwDataSize)
{
  if(HAL_SDRAM_Read_32b(&sdramHandle, (uint32_t *)uwStartAddress, pData, uwDataSize) != HAL_OK)
  {
    return SDRAM_ERROR;
  }
  else
  {
    return SDRAM_OK;
  }
}

/**
  * @brief  Reads an amount of data from the SDRAM memory in DMA mode.
  * @param  uwStartAddress: Read start address
  * @param  pData: Pointer to data to be read
  * @param  uwDataSize: Size of read data from the memory
  * @retval SDRAM status
  */
uint8_t BSP_SDRAM_ReadData_DMA(uint32_t uwStartAddress, uint32_t *pData, uint32_t uwDataSize)
{
  if(HAL_SDRAM_Read_DMA(&sdramHandle, (uint32_t *)uwStartAddress, pData, uwDataSize) != HAL_OK)
  {
    return SDRAM_ERROR;
  }
  else
  {
    return SDRAM_OK;
  }
}

/**
  * @brief  Writes an amount of data to the SDRAM memory in polling mode.
  * @param  uwStartAddress: Write start address
  * @param  pData: Pointer to data to be written
  * @param  uwDataSize: Size of written data from the memory
  * @retval SDRAM status
  */
uint8_t BSP_SDRAM_WriteData(uint32_t uwStartAddress, uint32_t *pData, uint32_t uwDataSize)
{
  if(HAL_SDRAM_Write_32b(&sdramHandle, (uint32_t *)uwStartAddress, pData, uwDataSize) != HAL_OK)
  {
    return SDRAM_ERROR;
  }
  else
  {
    return SDRAM_OK;
  }
}

/**
  * @brief  Writes an amount of data to the SDRAM memory in DMA mode.
  * @param  uwStartAddress: Write start address
  * @param  pData: Pointer to data to be written
  * @param  uwDataSize: Size of written data from the memory
  * @retval SDRAM status
  */
uint8_t BSP_SDRAM_WriteData_DMA(uint32_t uwStartAddress, uint32_t *pData, uint32_t uwDataSize)
{
  if(HAL_SDRAM_Write_DMA(&sdramHandle, (uint32_t *)uwStartAddress, pData, uwDataSize) != HAL_OK)
  {
    return SDRAM_ERROR;
  }
  else
  {
    return SDRAM_OK;
  }
}

/**
  * @brief  Sends command to the SDRAM bank.
  * @param  SdramCmd: Pointer to SDRAM command structure
  * @retval SDRAM status
  */
uint8_t BSP_SDRAM_Sendcmd(FMC_SDRAM_CommandTypeDef *SdramCmd)
{
  if(HAL_SDRAM_SendCommand(&sdramHandle, SdramCmd, SDRAM_TIMEOUT) != HAL_OK)
  {
    return SDRAM_ERROR;
  }
  else
  {
    return SDRAM_OK;
  }
}

/**
  * @brief  Initializes SDRAM MSP.
  * @param  hsdram SDRAM handle
  * @param  Params User parameters
  * @retval None
  */
__weak void BSP_SDRAM_MspInit(SDRAM_HandleTypeDef  *hsdram, void *Params)
{
  static MDMA_HandleTypeDef mdma_handle;
  GPIO_InitTypeDef gpio_init_structure;

  /* Enable FMC clock */
  __HAL_RCC_FMC_CLK_ENABLE();

  /* Enable chosen MDMAx clock */
  __MDMAx_CLK_ENABLE();

  /* Enable GPIOs clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();

  /* Common GPIO configuration */
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_NOPULL;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init_structure.Alternate = GPIO_AF12_FMC;



  /* GPIOA configuration */
  gpio_init_structure.Pin   = GPIO_PIN_7;
  HAL_GPIO_Init(GPIOA, &gpio_init_structure);

  /* GPIOB configuration */
  gpio_init_structure.Pin   = GPIO_PIN_5 | GPIO_PIN_6;
  HAL_GPIO_Init(GPIOB, &gpio_init_structure);


  /* GPIOD configuration */
  gpio_init_structure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8| GPIO_PIN_9 | GPIO_PIN_10 |\
                              GPIO_PIN_14 | GPIO_PIN_15;


  HAL_GPIO_Init(GPIOD, &gpio_init_structure);

  /* GPIOE configuration */
  gpio_init_structure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_7| GPIO_PIN_8 | GPIO_PIN_9 |\
                              GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 |\
                              GPIO_PIN_15;

  HAL_GPIO_Init(GPIOE, &gpio_init_structure);

  /* GPIOF configuration */
  gpio_init_structure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3 | GPIO_PIN_4 |\
                              GPIO_PIN_5 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 |\
                              GPIO_PIN_15;

  HAL_GPIO_Init(GPIOF, &gpio_init_structure);

  /* GPIOG configuration */
  gpio_init_structure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOG, &gpio_init_structure);



  /* Configure common MDMA parameters */
  mdma_handle.Init.Request = MDMA_REQUEST_SW;
  mdma_handle.Init.TransferTriggerMode = MDMA_BLOCK_TRANSFER;
  mdma_handle.Init.Priority = MDMA_PRIORITY_HIGH;
  mdma_handle.Init.Endianness = MDMA_LITTLE_ENDIANNESS_PRESERVE;
  mdma_handle.Init.SourceInc = MDMA_SRC_INC_WORD;
  mdma_handle.Init.DestinationInc = MDMA_DEST_INC_WORD;
  mdma_handle.Init.SourceDataSize = MDMA_SRC_DATASIZE_WORD;
  mdma_handle.Init.DestDataSize = MDMA_DEST_DATASIZE_WORD;
  mdma_handle.Init.DataAlignment = MDMA_DATAALIGN_PACKENABLE;
  mdma_handle.Init.SourceBurst = MDMA_SOURCE_BURST_SINGLE;
  mdma_handle.Init.DestBurst = MDMA_DEST_BURST_SINGLE;
  mdma_handle.Init.BufferTransferLength = 128;
  mdma_handle.Init.SourceBlockAddressOffset = 0;
  mdma_handle.Init.DestBlockAddressOffset = 0;


  mdma_handle.Instance = SDRAM_MDMAx_CHANNEL;

   /* Associate the DMA handle */
  __HAL_LINKDMA(hsdram, hmdma, mdma_handle);

  /* Deinitialize the stream for new transfer */
  HAL_MDMA_DeInit(&mdma_handle);

  /* Configure the DMA stream */
  HAL_MDMA_Init(&mdma_handle);

  /* NVIC configuration for DMA transfer complete interrupt */
  HAL_NVIC_SetPriority(SDRAM_MDMAx_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ(SDRAM_MDMAx_IRQn);
}

/**
  * @brief  DeInitializes SDRAM MSP.
  * @param  hsdram SDRAM handle
  * @param  Params User parameters
  * @retval None
  */
__weak void BSP_SDRAM_MspDeInit(SDRAM_HandleTypeDef  *hsdram, void *Params)
{
    static MDMA_HandleTypeDef mdma_handle;

    /* Disable NVIC configuration for DMA interrupt */
    HAL_NVIC_DisableIRQ(SDRAM_MDMAx_IRQn);

    /* Deinitialize the stream for new transfer */
    mdma_handle.Instance = SDRAM_MDMAx_CHANNEL;
    HAL_MDMA_DeInit(&mdma_handle);

    /* GPIO pins clock, FMC clock and MDMA clock can be shut down in the applications
       by surcharging this __weak function */
}





#if HW_USE_CMDIF_SDRAM == 1
void sdramCmdifInit(void)
{
  cmdifAdd("sdram", sdramCmdif);
}

void sdramCmdif(void)
{
  bool ret = true;
  uint8_t number;
  uint32_t i;
  uint32_t pre_time;


  if (cmdifGetParamCnt() == 2)
  {
    if(cmdifHasString("test", 0) == true)
    {
      uint32_t *p_data = (uint32_t *)SDRAM_DEVICE_ADDR;

      number = (uint8_t)cmdifGetParam(1);

      while(number > 0)
      {
        pre_time = micros();
        for (i=0; i<SDRAM_DEVICE_SIZE/4; i++)
        {
          p_data[i] = i;
        }
        cmdifPrintf( "Write : %d MB/s\n", SDRAM_DEVICE_SIZE / (micros()-pre_time) );


        for (i=0; i<SDRAM_DEVICE_SIZE/4; i++)
        {
          if (p_data[i] != i)
          {
            cmdifPrintf( "%d : 0x%X fail\n", i, p_data[i]);
            break;
          }
        }

        if (i == SDRAM_DEVICE_SIZE/4)
        {
          cmdifPrintf( "Count %d\n", number);
          cmdifPrintf( "Sdram %d MB OK\n\n", SDRAM_DEVICE_SIZE/1024/1024);
          for (i=0; i<SDRAM_DEVICE_SIZE/4; i++)
          {
            p_data[i] = 0x5555AAAA;
          }
        }

        number--;

        if (cmdifRxAvailable() > 0)
        {
          cmdifPrintf( "Stop test...\n");
          break;
        }
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
    cmdifPrintf( "sdram test 1~100 \n");
  }
}
#endif
