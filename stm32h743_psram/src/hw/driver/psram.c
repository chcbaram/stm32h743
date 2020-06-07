/*
 * psram.c
 *
 *  Created on: 2020. 5. 30.
 *      Author: Baram
 */




#include "psram.h"
#include "cmdif.h"



#define PSRAM_MAX_CH              1


#define PSRAM_ADDR_OFFSET         0x90000000
#define PSRAM_MAX_SIZE            (8*1024*1024)
#define PSRAM_SECTOR_SIZE         (4*1024)
#define PSRAM_PAGE_SIZE           (1024)
#define PSRAM_MAX_SECTOR          (FLASH_MAX_SIZE / FLASH_SECTOR_SIZE)


#define QSPI_OK                   ((uint8_t)0x00)
#define QSPI_ERROR                ((uint8_t)0x01)
#define QSPI_BUSY                 ((uint8_t)0x02)
#define QSPI_NOT_SUPPORTED        ((uint8_t)0x04)
#define QSPI_SUSPENDED            ((uint8_t)0x08)



static QSPI_HandleTypeDef QSPIHandle;
static uint32_t psram_addr   = PSRAM_ADDR_OFFSET;
static uint32_t psram_length = 0;

typedef struct
{
  bool     is_init;
  uint32_t id;
  uint32_t length;

} psram_tbl_t;


static psram_tbl_t psram_tbl[PSRAM_MAX_CH];



static bool psramSetup(void);
static bool psramGetVendorID(uint8_t *vendorId);
static bool psramEnterQPI(void);
static bool psramExitQPI(void);
static bool psramReset(void);
static bool psramEnterMemoyMaped(void);
static void psramPinInit(QSPI_HandleTypeDef *hqspi, void *Params);


#ifdef _USE_HW_CMDIF
void psramCmdif(void);
#endif

bool psramInit(void)
{
  bool ret = true;



  psram_tbl[0].is_init = false;
  psram_tbl[0].length = 0;


  if (psramSetup() == true)
  {
    psram_length += PSRAM_MAX_SIZE;
  }

  if (psram_length == 0)
  {
    ret = false;
  }
  else
  {
    psram_tbl[0].is_init = false;
    psram_tbl[0].id = 0;
    ret = psramGetVendorID((uint8_t *)&psram_tbl[0].id);

    if (ret == true && psram_tbl[0].id == 0x5D0D)
    {
      psram_tbl[0].is_init = true;
      psram_tbl[0].length = PSRAM_MAX_SIZE;

      psramEnterQPI();
      psramEnterMemoyMaped();
    }
    ret = psram_tbl[0].is_init;
  }

#ifdef _USE_HW_CMDIF
  cmdifAdd("psram", psramCmdif);
#endif

  return ret;
}


bool psramSetup(void)
{
  bool ret = true;


  QSPIHandle.Instance = QUADSPI;

  if (HAL_QSPI_DeInit(&QSPIHandle) != HAL_OK)
  {
    return false;
  }

  psramPinInit(&QSPIHandle, NULL);


  /* QSPI initialization */
  /* ClockPrescaler set to 1, so QSPI clock = 200MHz / (1+1) = 100MHz */
  QSPIHandle.Init.ClockPrescaler     = 1;
  QSPIHandle.Init.FifoThreshold      = 4;
  QSPIHandle.Init.SampleShifting     = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
  QSPIHandle.Init.FlashSize          = 24-1;
  QSPIHandle.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_6_CYCLE; /* Min 50ns for nonRead */
  QSPIHandle.Init.ClockMode          = QSPI_CLOCK_MODE_0;
  QSPIHandle.Init.FlashID            = QSPI_FLASH_ID_1;
  QSPIHandle.Init.DualFlash          = QSPI_DUALFLASH_DISABLE;

  if (HAL_QSPI_Init(&QSPIHandle) != HAL_OK)
  {
    return false;
  }


  if (psramReset() != true)
  {
    return false;
  }


  psramExitQPI();

  return ret;
}

uint32_t psramGetAddr(void)
{
  return psram_addr;
}

uint32_t psramGetLength(void)
{
  return psram_length;
}




#define PSRAM_CMD_LUT_SEQ_IDX_ENTERQPI          0x35
#define PSRAM_CMD_LUT_SEQ_IDX_EXITQPI           0xF5
#define PSRAM_CMD_LUT_SEQ_IDX_READID            0x9F
#define PSRAM_CMD_LUT_SEQ_IDX_READ_QPI          0xEB
#define PSRAM_CMD_LUT_SEQ_IDX_WRITE_QPI         0x38
#define PSRAM_CMD_LUT_SEQ_IDX_RESET_ENABLE      0x66
#define PSRAM_CMD_LUT_SEQ_IDX_RESET             0x99



bool psramGetVendorID(uint8_t *vendorId)
{
  bool ret = true;
  QSPI_CommandTypeDef s_command;


  /* Initialize the read flag status register command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = PSRAM_CMD_LUT_SEQ_IDX_READID;
  s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
  s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_1_LINE;
  s_command.DummyCycles       = 0;
  s_command.NbData            = 2;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return false;
  }

  /* Reception of the data */
  if (HAL_QSPI_Receive(&QSPIHandle, vendorId, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return false;
  }


  return ret;
}

bool psramReset(void)
{
  bool ret = true;
  QSPI_CommandTypeDef s_command;


  /* Initialize the read flag status register command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = PSRAM_CMD_LUT_SEQ_IDX_RESET_ENABLE;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DummyCycles       = 0;
  s_command.NbData            = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;


  if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return false;
  }

  s_command.Instruction       = PSRAM_CMD_LUT_SEQ_IDX_RESET;
  if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return false;
  }

  return ret;
}

bool psramEnterQPI(void)
{
  bool ret = true;
  QSPI_CommandTypeDef s_command;


  /* Initialize the read flag status register command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = PSRAM_CMD_LUT_SEQ_IDX_ENTERQPI;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DummyCycles       = 0;
  s_command.NbData            = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;


  if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return false;
  }

  return ret;
}

bool psramExitQPI(void)
{
  bool ret = true;
  QSPI_CommandTypeDef s_command;


  /* Initialize the read flag status register command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
  s_command.Instruction       = PSRAM_CMD_LUT_SEQ_IDX_EXITQPI;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DummyCycles       = 0;
  s_command.NbData            = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;


  if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return false;
  }

  return ret;
}

bool psramEnterMemoyMaped(void)
{
  bool ret = true;
  QSPI_CommandTypeDef s_command;
  QSPI_MemoryMappedTypeDef s_mem_mapped_cfg;


  /* Initialize the read flag status register command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
  s_command.Instruction       = PSRAM_CMD_LUT_SEQ_IDX_READ_QPI;
  s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
  s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_4_LINES;
  s_command.DummyCycles       = 6;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  //if (HAL_QSPI_MemoryMapped(&QSPIHandle, &s_command, &s_mem_mapped_cfg) != HAL_OK)
  {
    //return QSPI_ERROR;
  }


  s_command.Instruction       = PSRAM_CMD_LUT_SEQ_IDX_WRITE_QPI;
  s_command.DummyCycles       = 0;
  if (HAL_QSPI_MemoryMapped(&QSPIHandle, &s_command, &s_mem_mapped_cfg) != HAL_OK)
  {
    return QSPI_ERROR;
  }
  return ret;
}




/* Definition for QSPI clock resources */
#define QSPI_CLK_ENABLE()              __HAL_RCC_QSPI_CLK_ENABLE()
#define QSPI_CLK_DISABLE()             __HAL_RCC_QSPI_CLK_DISABLE()
#define QSPI_CLK_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
#define QSPI_BK1_CS_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()
#define QSPI_BK1_D0_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOD_CLK_ENABLE()
#define QSPI_BK1_D1_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOD_CLK_ENABLE()
#define QSPI_BK1_D2_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOE_CLK_ENABLE()
#define QSPI_BK1_D3_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOD_CLK_ENABLE()


#define QSPI_FORCE_RESET()         __HAL_RCC_QSPI_FORCE_RESET()
#define QSPI_RELEASE_RESET()       __HAL_RCC_QSPI_RELEASE_RESET()

/* Definition for QSPI Pins */
#define QSPI_CLK_PIN               GPIO_PIN_2
#define QSPI_CLK_GPIO_PORT         GPIOB
#define QSPI_CLK_GPIO_AF           GPIO_AF9_QUADSPI

/* Bank 1 */
#define QSPI_BK1_CS_PIN            GPIO_PIN_6
#define QSPI_BK1_CS_GPIO_PORT      GPIOB
#define QSPI_BK1_CS_GPIO_AF        GPIO_AF10_QUADSPI

#define QSPI_BK1_D0_PIN            GPIO_PIN_11
#define QSPI_BK1_D0_GPIO_PORT      GPIOD
#define QSPI_BK1_D0_GPIO_AF        GPIO_AF9_QUADSPI

#define QSPI_BK1_D1_PIN            GPIO_PIN_12
#define QSPI_BK1_D1_GPIO_PORT      GPIOD
#define QSPI_BK1_D1_GPIO_AF        GPIO_AF9_QUADSPI

#define QSPI_BK1_D2_PIN            GPIO_PIN_2
#define QSPI_BK1_D2_GPIO_PORT      GPIOE
#define QSPI_BK1_D2_GPIO_AF        GPIO_AF9_QUADSPI

#define QSPI_BK1_D3_PIN            GPIO_PIN_13
#define QSPI_BK1_D3_GPIO_PORT      GPIOD
#define QSPI_BK1_D3_GPIO_AF        GPIO_AF9_QUADSPI




void psramPinInit(QSPI_HandleTypeDef *hqspi, void *Params)
{
  GPIO_InitTypeDef gpio_init_structure;

  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable the QuadSPI memory interface clock */
  QSPI_CLK_ENABLE();
  /* Reset the QuadSPI memory interface */
  QSPI_FORCE_RESET();
  QSPI_RELEASE_RESET();
  /* Enable GPIO clocks */
  QSPI_CLK_GPIO_CLK_ENABLE();
  QSPI_BK1_CS_GPIO_CLK_ENABLE();
  QSPI_BK1_D0_GPIO_CLK_ENABLE();
  QSPI_BK1_D1_GPIO_CLK_ENABLE();
  QSPI_BK1_D2_GPIO_CLK_ENABLE();
  QSPI_BK1_D3_GPIO_CLK_ENABLE();


  /*##-2- Configure peripheral GPIO ##########################################*/
  /* QSPI CLK GPIO pin configuration  */
  gpio_init_structure.Pin       = QSPI_CLK_PIN;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init_structure.Pull      = GPIO_NOPULL;
  gpio_init_structure.Alternate = QSPI_CLK_GPIO_AF;
  HAL_GPIO_Init(QSPI_CLK_GPIO_PORT, &gpio_init_structure);

  /* QSPI CS GPIO pin configuration  */
  gpio_init_structure.Pin       = QSPI_BK1_CS_PIN;
  gpio_init_structure.Pull      = GPIO_PULLUP;
  gpio_init_structure.Alternate = QSPI_BK1_CS_GPIO_AF;
  HAL_GPIO_Init(QSPI_BK1_CS_GPIO_PORT, &gpio_init_structure);

  /* QSPI D0 GPIO pin configuration  */
  gpio_init_structure.Pin       = QSPI_BK1_D0_PIN;
  gpio_init_structure.Pull      = GPIO_NOPULL;
  gpio_init_structure.Alternate = QSPI_BK1_D0_GPIO_AF;
  HAL_GPIO_Init(QSPI_BK1_D0_GPIO_PORT, &gpio_init_structure);

  /* QSPI D1 GPIO pin configuration  */
  gpio_init_structure.Pin       = QSPI_BK1_D1_PIN;
  gpio_init_structure.Alternate = QSPI_BK1_D1_GPIO_AF;
  HAL_GPIO_Init(QSPI_BK1_D1_GPIO_PORT, &gpio_init_structure);

  /* QSPI D2 GPIO pin configuration  */
  gpio_init_structure.Pin       = QSPI_BK1_D2_PIN;
  gpio_init_structure.Alternate = QSPI_BK1_D2_GPIO_AF;
  HAL_GPIO_Init(QSPI_BK1_D2_GPIO_PORT, &gpio_init_structure);

  /* QSPI D3 GPIO pin configuration  */
  gpio_init_structure.Pin       = QSPI_BK1_D3_PIN;
  gpio_init_structure.Alternate = QSPI_BK1_D3_GPIO_AF;
  HAL_GPIO_Init(QSPI_BK1_D3_GPIO_PORT, &gpio_init_structure);


  /*##-3- Configure the NVIC for QSPI #########################################*/
  /* NVIC configuration for QSPI interrupt */
  HAL_NVIC_SetPriority(QUADSPI_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ(QUADSPI_IRQn);

}



#ifdef _USE_HW_CMDIF
void psramCmdif(void)
{
  bool ret = true;
  uint8_t number;
  uint32_t i;
  uint32_t pre_time;


  if (cmdifGetParamCnt() == 1)
  {
    if(cmdifHasString("info", 0) == true)
    {
      cmdifPrintf("PSRAM Add : 0x%X\n", psram_addr);
      cmdifPrintf("PSRAM Len : %dKB\n", psram_length/1024);

      for (int i=0; i<PSRAM_MAX_CH; i++)
      {
        cmdifPrintf("PSRAM CH%d, Init: %d, 0x%X, Len : %dKB\n", i+1,
                    psram_tbl[i].is_init,
                    psram_tbl[i].id,
                    psram_tbl[i].length/1024);
      }
    }
    else
    {
      ret = false;
    }
  }
  else if (cmdifGetParamCnt() == 2)
  {
    if(cmdifHasString("test", 0) == true)
    {
      uint32_t *p_data = (uint32_t *)psram_addr;

      number = (uint8_t)cmdifGetParam(1);

      while(number > 0)
      {
        pre_time = millis();
        for (i=0; i<psram_length/4; i++)
        {
          p_data[i] = i;
        }
        cmdifPrintf( "Write : %d MB/s\n", psram_length / 1000 / (millis()-pre_time) );

        volatile uint32_t data_sum = 0;
        pre_time = millis();
        for (i=0; i<psram_length/4; i++)
        {
          data_sum += p_data[i];
        }
        cmdifPrintf( "Read : %d MB/s\n", psram_length / 1000 / (millis()-pre_time) );


        for (i=0; i<psram_length/4; i++)
        {
          if (p_data[i] != i)
          {
            cmdifPrintf( "%d : 0x%X fail\n", i, p_data[i]);
            break;
          }
        }

        if (i == psram_length/4)
        {
          cmdifPrintf( "Count %d\n", number);
          cmdifPrintf( "PSRAM %d MB OK\n\n", psram_length/1024/1024);
          for (i=0; i<psram_length/4; i++)
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
    cmdifPrintf( "psram info \n");
    cmdifPrintf( "psram test 1~\n");
  }
}
#endif





