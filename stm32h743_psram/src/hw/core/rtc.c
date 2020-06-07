/*
 * rtc.c
 *
 *  Created on: 2019. 6. 21.
 *      Author: HanCheol Cho
 */




#include "rtc.h"
#include "reset.h"
#include "cmdif.h"




RTC_HandleTypeDef RtcHandle;



#define RTC_ASYNCH_PREDIV  0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FF /* LSE as RTC clock */

#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24UL)
#define DAYS_PER_WEEK (7UL)
#define SECS_PER_WEEK (SECS_PER_DAY * DAYS_PER_WEEK)
#define SECS_PER_YEAR (SECS_PER_WEEK * 52UL)
#define SECS_YR_2000  (946684800UL) // the time at the start of y2k


//#define LEAP_YEAR(Y)     ( ((1970+Y)>0) && !((1970+Y)%4) && ( ((1970+Y)%100) || !((1970+Y)%400) ) )
#define LEAP_YEAR(Y)     ( ((2000+Y)>0) && !((2000+Y)%4) && ( ((2000+Y)%100) || !((2000+Y)%400) ) )

static  const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31}; // API starts months from 1, this array starts from 0
static uint32_t reset_time = 0;
static bool is_init = false;


static void RTC_CalendarConfig(void);
static void rtcBreakTime(time_t timeInput, RTC_DateTypeDef *p_date, RTC_TimeTypeDef *p_time);
static time_t rtcMakeTime(RTC_DateTypeDef *p_date, RTC_TimeTypeDef *p_time);


#if HW_USE_CMDIF_RTC == 1
void rtcCmdifInit(void);
void rtcCmdif(void);
#endif


bool rtcInit(void)
{
  /*##-1- Configure the RTC peripheral #######################################*/
  /* Configure RTC prescaler and RTC data registers */
  /* RTC configured as follows:
      - Hour Format    = Format 24
      - Asynch Prediv  = Value according to source clock
      - Synch Prediv   = Value according to source clock
      - OutPut         = Output Disable
      - OutPutPolarity = High Polarity
      - OutPutType     = Open Drain */
  RtcHandle.Instance = RTC;
  RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
  RtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
  RtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
  RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;


  if (HAL_RTC_Init(&RtcHandle) != HAL_OK)
  {
    logPrintf("RTC     \t\t: Fail\r\n");
    return false;
  }

  resetSetBits(rtcReadBackupData(_HW_DEF_RTC_RESET_SRC));
  resetInit();

  reset_time = (uint32_t)rtcGetTime();


  /*##-2- Check if Data stored in BackUp register1: No Need to reconfigure RTC#*/
  /* Read the Back Up Register 1 Data */
  if (HAL_RTCEx_BKUPRead(&RtcHandle, RTC_CFG_DATA_1) != 0x32F2)
  {
    /* Configure RTC Calendar */
    RTC_CalendarConfig();

    reset_time = (uint32_t)rtcGetTime();

    rtcWriteBackupData(RTC_CFG_DATA_2, reset_time);
  }

  if (resetGetBits() & (1<<_DEF_RESET_POWER))
  {
    rtcWriteBackupData(RTC_CFG_DATA_2, reset_time);
  }


  logPrintf("RTC     \t\t: OK\r\n");
  is_init = true;

#if HW_USE_CMDIF_RTC == 1
  rtcCmdifInit();
#endif

  return true;
}

bool rtcIsInit(void)
{
  return is_init;
}

time_t rtcGetTime()
{
  RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;
  time_t ret = 0;

  /* Get the RTC current Time */
  HAL_RTC_GetTime(&RtcHandle, &stimestructureget, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, RTC_FORMAT_BIN);


  ret = rtcMakeTime(&sdatestructureget, &stimestructureget);

  return ret;
}

uint32_t rtcGetSecondsFromPower(void)
{
  uint32_t pre_time;
  uint32_t cur_time;

  pre_time = rtcReadBackupData(RTC_CFG_DATA_2);
  cur_time = (uint32_t)rtcGetTime();

  return cur_time - pre_time;
}

uint32_t rtcGetSecondsFromReset(void)
{
  uint32_t pre_time;
  uint32_t cur_time;

  pre_time = reset_time;
  cur_time = (uint32_t)rtcGetTime();

  return cur_time - pre_time;
}

void rtcSetTime(time_t time_data)
{
  RTC_DateTypeDef sdatestructure;
  RTC_TimeTypeDef stimestructure;


  HAL_RTC_GetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&RtcHandle, &sdatestructure, RTC_FORMAT_BIN);


  rtcBreakTime(time_data, &sdatestructure, &stimestructure);

  //HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BCD);
  //HAL_RTC_SetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BCD);
  HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BIN);
  HAL_RTC_SetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BIN);
}

time_t rtcMakeTime(RTC_DateTypeDef *p_date, RTC_TimeTypeDef *p_time)
{
// assemble time elements into time_t
// note year argument is offset from 1970 (see macros in time.h to convert to other formats)
// previous version used full four digit year (or digits since 2000),i.e. 2009 was 2009 or 9

  int i;
  uint32_t seconds;

  // seconds from 1970 till 1 jan 00:00:00 of the given year
  seconds= p_date->Year*(SECS_PER_DAY * 365);
  for (i = 0; i < p_date->Year; i++) {
    if (LEAP_YEAR(i)) {
      seconds +=  SECS_PER_DAY;   // add extra days for leap years
    }
  }

  // add days for this year, months start from 1
  for (i = 1; i < p_date->Month; i++) {
    if ( (i == 2) && LEAP_YEAR(p_date->Year)) {
      seconds += SECS_PER_DAY * 29;
    } else {
      seconds += SECS_PER_DAY * monthDays[i-1];  //monthDay array starts from 0
    }
  }
  seconds+= (p_date->Date-1) * SECS_PER_DAY;
  seconds+= p_time->Hours * SECS_PER_HOUR;
  seconds+= p_time->Minutes * SECS_PER_MIN;
  seconds+= p_time->Seconds;
  return (time_t)seconds;
}

void rtcBreakTime(time_t timeInput, RTC_DateTypeDef *p_date, RTC_TimeTypeDef *p_time)
{
// break the given time_t into time components
// this is a more compact version of the C library localtime function
// note that year is offset from 1970 !!!

  uint8_t year;
  uint8_t month, monthLength;
  uint32_t time;
  unsigned long days;

  time = (uint32_t)timeInput;
  p_time->Seconds = time % 60;
  time /= 60; // now it is minutes
  p_time->Minutes = time % 60;
  time /= 60; // now it is hours
  p_time->Hours = time % 24;
  time /= 24; // now it is days


  year = 0;
  days = 0;
  while((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= time) {
    year++;
  }
  p_date->Year = year; // year is offset from 1970

  days -= LEAP_YEAR(year) ? 366 : 365;
  time  -= days; // now it is days in this year, starting at 0

  days=0;
  month=0;
  monthLength=0;
  for (month=0; month<12; month++) {
    if (month==1) { // february
      if (LEAP_YEAR(year)) {
        monthLength=29;
      } else {
        monthLength=28;
      }
    } else {
      monthLength = monthDays[month];
    }

    if (time >= monthLength) {
      time -= monthLength;
    } else {
        break;
    }
  }
  p_date->Month = month + 1;  // jan is month 1
  p_date->Date = time + 1;     // day of month
}


void rtcWriteBackupData(uint32_t index, uint32_t data)
{
  HAL_RTCEx_BKUPWrite(&RtcHandle, index, data);
}


uint32_t rtcReadBackupData(uint32_t index)
{
  return HAL_RTCEx_BKUPRead(&RtcHandle, index);
}







/**
  * @brief  Configure the current time and date.
  * @param  None
  * @retval None
  */
static void RTC_CalendarConfig(void)
{
  RTC_DateTypeDef sdatestructure;
  RTC_TimeTypeDef stimestructure;

  /*##-1- Configure the Date #################################################*/
  /* Set Date: Tuesday February 18th 2014 */
  sdatestructure.Year = 0x14;
  sdatestructure.Month = RTC_MONTH_FEBRUARY;
  sdatestructure.Date = 0x18;
  sdatestructure.WeekDay = RTC_WEEKDAY_TUESDAY;

  if(HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
    //Error_Handler();
  }

  /*##-2- Configure the Time #################################################*/
  /* Set Time: 02:00:00 */
  stimestructure.Hours = 0x02;
  stimestructure.Minutes = 0x00;
  stimestructure.Seconds = 0x00;
  stimestructure.TimeFormat = RTC_HOURFORMAT12_AM;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

  if (HAL_RTC_SetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
    //Error_Handler();
  }

  /*##-3- Writes a data in a RTC Backup data Register1 #######################*/
  HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_CFG_DATA_1, 0x32F2);
}







void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
  UNUSED(hrtc);

  RCC_OscInitTypeDef        RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

  /*##-1- Enables the PWR Clock and Enables access to the backup domain ###################################*/
  HAL_PWR_EnableBkUpAccess();


  /*##-2- Configure LSE as RTC clock source ###################################*/
  RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    //Error_Handler();
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    //Error_Handler();
  }

  /*##-3- Enable RTC peripheral Clocks #######################################*/
  /* Enable RTC Clock */
  __HAL_RCC_RTC_ENABLE();
}

/**
  * @brief RTC MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  * @param hrtc: RTC handle pointer
  * @retval None
  */
void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc)
{
  UNUSED(hrtc);

  /*##-1- Reset peripherals ##################################################*/
  __HAL_RCC_RTC_DISABLE();

  /*##-2- Disables the PWR Clock and Disables access to the backup domain ###################################*/
  HAL_PWR_DisableBkUpAccess();
}






#if HW_USE_CMDIF_RTC == 1
void rtcCmdifInit(void)
{
  cmdifAdd("rtc", rtcCmdif);
}

void rtcCmdif(void)
{
  bool ret = true;


  if (cmdifGetParamCnt() == 1)
  {
    if(cmdifHasString("show", 0) == true)
    {
      while(cmdifRxAvailable() == 0)
      {
        cmdifPrintf( "%d sec, %d sec\n", rtcGetSecondsFromReset(), rtcGetSecondsFromPower());
        delay(100);
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
    cmdifPrintf( "rtc show \n");
  }
}
#endif

