/*
 * uart.c
 *
 *  Created on: 2019. 3. 7.
 *      Author: HanCheol Cho
 */




#include "uart.h"
#include "qbuffer.h"
#include "vcp.h"


/*
  _DEF_UART1
      USART1
        - RX PB15: DMA1_Stream0
        - TX PB14:

  _DEF_UART2
      VCP
*/



#define UART_MODE_POLLING       0
#define UART_MODE_INTERRUPT     1
#define UART_MODE_DMA           2
#define UART_MODE_VCP           3

#define UART_HW_NONE            0
#define UART_HW_STM32_VCP       1
#define UART_HW_STM32_UART      2
#define UART_HW_MAX14830        3


#define UART_RX_BUF_LENGTH      (1024+7)




typedef struct
{
  bool     is_open;
  uint8_t  ch;
  uint32_t baud;
  uint8_t  tx_mode;
  uint8_t  rx_mode;
  uint8_t  hw_driver;

  uint8_t  rx_buf[UART_RX_BUF_LENGTH];

  qbuffer_t qbuffer_rx;

  DMA_HandleTypeDef  *hdma_tx;
  DMA_HandleTypeDef  *hdma_rx;
  UART_HandleTypeDef *handle;

  bool  tx_done;
  void  (*txDoneISR)(void);

  uint32_t err_cnt;
} uart_t;



static __attribute__((section(".sram_d3")))  uart_t uart_tbl[UART_MAX_CH];


UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;


void uartStartRx(uint8_t channel);
void uartRxHandler(uint8_t channel);



bool uartInit(void)
{
  uint8_t i;


  for (i=0; i<UART_MAX_CH; i++)
  {
    uart_tbl[i].is_open  = false;
    uart_tbl[i].rx_mode  = UART_MODE_POLLING;
    uart_tbl[i].tx_mode  = UART_MODE_POLLING;
    uart_tbl[i].tx_done  = false;
    uart_tbl[i].txDoneISR = NULL;
    uart_tbl[i].err_cnt  = 0;
    uart_tbl[i].hw_driver = UART_HW_NONE;
  }

  return true;
}

bool uartOpen(uint8_t channel, uint32_t baud)
{
  bool ret = false;
  uart_t *p_uart;


  if (channel >= UART_MAX_CH)
  {
    return false;
  }

  switch(channel)
  {
    case _DEF_UART1:
      p_uart = &uart_tbl[channel];

      p_uart->baud      = baud;
      p_uart->hw_driver = UART_HW_STM32_UART;
      p_uart->rx_mode   = UART_MODE_DMA;
      p_uart->tx_mode   = UART_MODE_POLLING;

      p_uart->hdma_rx = &hdma_usart1_rx;
      p_uart->handle =  &huart1;
      p_uart->handle->Instance = USART1;

      p_uart->handle->Init.BaudRate     = baud;
      p_uart->handle->Init.WordLength   = UART_WORDLENGTH_8B;
      p_uart->handle->Init.StopBits     = UART_STOPBITS_1;
      p_uart->handle->Init.Parity       = UART_PARITY_NONE;
      p_uart->handle->Init.Mode         = UART_MODE_TX_RX;
      p_uart->handle->Init.HwFlowCtl    = UART_HWCONTROL_NONE;
      p_uart->handle->Init.OverSampling = UART_OVERSAMPLING_16;
      p_uart->handle->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
      p_uart->handle->Init.ClockPrescaler = UART_PRESCALER_DIV1;
      p_uart->handle->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_AUTOBAUDRATE_INIT;
      p_uart->handle->AdvancedInit.AutoBaudRateEnable = UART_ADVFEATURE_AUTOBAUDRATE_ENABLE;
      p_uart->handle->AdvancedInit.AutoBaudRateMode = UART_ADVFEATURE_AUTOBAUDRATE_ON0X55FRAME;


      qbufferCreate(&p_uart->qbuffer_rx, p_uart->rx_buf, UART_RX_BUF_LENGTH);

      HAL_UART_DeInit(p_uart->handle);
      HAL_UART_Init(p_uart->handle);

      p_uart->is_open  = true;

      uartStartRx(channel);
      ret = true;
      break;

    case _DEF_UART2:
      p_uart = &uart_tbl[channel];

      p_uart->baud     = baud;
      p_uart->is_open  = true;

      p_uart->rx_mode  = UART_MODE_VCP;
      p_uart->tx_mode  = UART_MODE_VCP;
      p_uart->hw_driver = UART_HW_STM32_VCP;
      uartStartRx(channel);
      ret = true;
      break;
  }

  return ret;
}

void uartSetTxDoneISR(uint8_t channel, void (*func)(void))
{
  uart_tbl[channel].txDoneISR = func;
}

void uartStartRx(uint8_t channel)
{
  uart_t *p_uart = &uart_tbl[channel];

  if (p_uart->rx_mode == UART_MODE_INTERRUPT)
  {
    HAL_UART_Receive_IT(p_uart->handle, p_uart->rx_buf, 1);
  }
  if (p_uart->rx_mode == UART_MODE_DMA)
  {
    HAL_UART_Receive_DMA(p_uart->handle, (uint8_t *)p_uart->qbuffer_rx.p_buf, p_uart->qbuffer_rx.length);
  }
}

bool uartClose(uint8_t channel)
{
  bool ret = false;


  if (channel >= UART_MAX_CH)
  {
    return false;
  }


  if (uart_tbl[channel].is_open == true && uart_tbl[channel].hw_driver == UART_HW_STM32_UART)
  {
    if(HAL_UART_DeInit(uart_tbl[channel].handle) == HAL_OK)
    {
      ret = true;
    }
  }


  return ret;
}

uint32_t uartAvailable(uint8_t channel)
{
  uint32_t ret;
  uart_t *p_uart = &uart_tbl[channel];


  if (channel >= UART_MAX_CH)
  {
    return 0;
  }

  if (p_uart->rx_mode == UART_MODE_VCP)
  {
    ret = vcpAvailable();
  }
  if (p_uart->rx_mode == UART_MODE_INTERRUPT)
  {
    ret = qbufferAvailable(&uart_tbl[channel].qbuffer_rx);
  }
  if (p_uart->rx_mode == UART_MODE_DMA)
  {
    p_uart->qbuffer_rx.ptr_in = p_uart->qbuffer_rx.length - ((DMA_Stream_TypeDef *)p_uart->hdma_rx->Instance)->NDTR;
    ret = qbufferAvailable(&p_uart->qbuffer_rx);
  }

  return ret;
}

void uartFlush(uint8_t channel)
{
  if (uart_tbl[channel].rx_mode == UART_MODE_VCP)
  {
    vcpFlush();
  }
  if (uart_tbl[channel].rx_mode == UART_MODE_INTERRUPT)
  {
    qbufferFlush(&uart_tbl[channel].qbuffer_rx);
  }
  if (uart_tbl[channel].rx_mode == UART_MODE_DMA)
  {
    uart_tbl[channel].qbuffer_rx.ptr_in  = uart_tbl[channel].qbuffer_rx.length - ((DMA_Stream_TypeDef *)uart_tbl[channel].hdma_rx->Instance)->NDTR;
    uart_tbl[channel].qbuffer_rx.ptr_out = uart_tbl[channel].qbuffer_rx.ptr_in;
  }
}

void uartPutch(uint8_t channel, uint8_t ch)
{
  uartWrite(channel, &ch, 1 );
}

uint8_t uartGetch(uint8_t channel)
{
  uint8_t ret = 0;


  while(1)
  {
    if (uartAvailable(channel) > 0)
    {
      ret = uartRead(channel);
      break;
    }
  }

  return ret;
}

int32_t uartWrite(uint8_t channel, uint8_t *p_data, uint32_t length)
{
  int32_t ret = 0;
  uart_t *p_uart = &uart_tbl[channel];


  if (p_uart->tx_mode == UART_MODE_VCP)
  {
    vcpWrite(p_data, length);
  }
  if (p_uart->tx_mode == UART_MODE_POLLING)
  {
    if (HAL_UART_Transmit(p_uart->handle, (uint8_t*)p_data, length, 1000) == HAL_OK)
    {
      ret = length;
    }
  }
  if (p_uart->tx_mode == UART_MODE_DMA)
  {
    if (HAL_UART_Transmit_DMA(p_uart->handle, p_data, length) == HAL_OK)
    {
      ret = length;
    }
  }
  return ret;
}

uint8_t uartRead(uint8_t channel)
{
  uint8_t ret = 0;
  uart_t *p_uart = &uart_tbl[channel];


  if (p_uart->rx_mode == UART_MODE_VCP)
  {
    ret = vcpRead();
  }
  if (p_uart->rx_mode == UART_MODE_INTERRUPT)
  {
    qbufferRead(&p_uart->qbuffer_rx, &ret, 1);
  }
  if (p_uart->rx_mode == UART_MODE_DMA)
  {
    qbufferRead(&p_uart->qbuffer_rx, &ret, 1);
  }
  return ret;
}

bool uartSendBreak(uint8_t channel)
{
  return true;
}


int32_t uartPrintf(uint8_t channel, const char *fmt, ...)
{
  int32_t ret = 0;
  va_list arg;
  va_start (arg, fmt);
  int32_t len;
  char print_buffer[256];


  len = vsnprintf(print_buffer, 255, fmt, arg);
  va_end (arg);

  ret = uartWrite(channel, (uint8_t *)print_buffer, len);

  return ret;
}

void uartRxHandler(uint8_t channel)
{
  uart_t *p_uart = &uart_tbl[channel];


  if(p_uart->rx_mode == UART_MODE_INTERRUPT)
  {
    qbufferWrite(&p_uart->qbuffer_rx, &p_uart->rx_buf[0], 1);

    __HAL_UNLOCK(p_uart->handle);
    uartStartRx(channel);
  }
}

void uartErrHandler(uint8_t channel)
{
  uartStartRx(channel);
  uartFlush(channel);

  uart_tbl[channel].err_cnt++;
}

uint32_t uartGetErrCnt(uint8_t channel)
{
  return uart_tbl[channel].err_cnt;
}

void uartResetErrCnt(uint8_t channel)
{
  uartStartRx(channel);
  uartFlush(channel);

  uart_tbl[channel].err_cnt = 0;
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
}




void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
  if (UartHandle->Instance == uart_tbl[_DEF_UART1].handle->Instance)
  {
    uartErrHandler(_DEF_UART1);
  }
  if (UartHandle->Instance == uart_tbl[_DEF_UART3].handle->Instance)
  {
    uartErrHandler(_DEF_UART3);
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == uart_tbl[_DEF_UART1].handle->Instance)
  {
    uartRxHandler(_DEF_UART1);
  }
}


void USART1_IRQHandler(void)
{
   HAL_UART_IRQHandler(&huart1);
}
void DMA1_Stream0_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_usart1_rx);
}



void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if(uartHandle->Instance==USART1)
  {
    uart_t *p_uart = &uart_tbl[_DEF_UART1];


    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PB15     ------> USART1_RX
    PB14     ------> USART1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_15|GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF4_USART1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    if (p_uart->rx_mode == UART_MODE_DMA)
    {
      /* USART1 DMA Init */
      /* USART1_RX Init */
      __HAL_RCC_DMA1_CLK_ENABLE();

      hdma_usart1_rx.Instance = DMA1_Stream0;
      hdma_usart1_rx.Init.Request = DMA_REQUEST_USART1_RX;
      hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
      hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
      hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
      hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
      hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
      hdma_usart1_rx.Init.Mode = DMA_CIRCULAR;
      hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;
      hdma_usart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
      if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
      {
        //Error_Handler();
      }

      __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart1_rx);

      /* DMA1_Stream0_IRQn interrupt configuration */
      //HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0x06, 0);
      //HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
    }

    /* USART1 interrupt Init */
    //HAL_NVIC_SetPriority(USART1_IRQn, 0x06, 0);
    //HAL_NVIC_EnableIRQ(USART1_IRQn);
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
    __HAL_RCC_USART1_FORCE_RESET();
    __HAL_RCC_USART1_RELEASE_RESET();

    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();
    __HAL_RCC_DMA1_CLK_DISABLE();


    /**USART1 GPIO Configuration
    PB15     ------> USART1_RX
    PB14     ------> USART1_TX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_15|GPIO_PIN_14);

    /* USART1 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
    HAL_NVIC_DisableIRQ(DMA1_Stream0_IRQn);
  }
}
