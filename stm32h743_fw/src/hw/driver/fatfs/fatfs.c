/*
 * fatfs.c
 *
 *  Created on: 2017. 2. 13.
 *      Author: baram
 */





#include "fatfs.h"

#ifdef _USE_HW_FATFS
#include "lib/FatFs/src/ff_gen_drv.h"
#include "driver/sd_diskio.h"


//-- Internal Variables
//
static bool is_init = false;

FATFS SDFatFs;    /* File system object for SD disk logical drive */
char  SDPath[4];  /* SD disk logical drive path */


//-- External Variables
//


//-- Internal Functions
//
#if HW_FATFS_USE_CMDIF == 1
void fatfsCmdifInit(void);
void fatfsCmdif(void);
#endif

static void fatfsPrintErr(FRESULT res);
static FRESULT fatfsScanFiles(char* path);



//-- External Functions
//




bool fatfsInit(void)
{
  is_init = false;



  if(FATFS_LinkDriver(&SD_Driver, SDPath) == 0)
  {
    if(f_mount(&SDFatFs, (TCHAR const*)SDPath, 0) == FR_OK)
    {
      is_init = true;
    }
  }

  if (is_init == true)
  {
    logPrintf("fatfs      \t\t: OK\n");
  }
  else
  {
    logPrintf("fatfs      \t\t: Fail\n");
  }
#if HW_FATFS_USE_CMDIF == 1
  fatfsCmdifInit();
#endif

  return is_init;
}


void fatfsPrintErr(FRESULT res)
{
  switch (res)
  {
    case FR_DISK_ERR:
      cmdifPrintf("FR_DISK_ERR\n");
      break;

    case FR_INT_ERR:
      cmdifPrintf("FR_INT_ERR\n");
      break;

    case FR_NOT_READY:
      cmdifPrintf("FR_NOT_READY\n");
      break;

    case FR_NO_FILE:
      cmdifPrintf("FR_NO_FILE\n");
      break;

    case FR_NO_PATH:
      cmdifPrintf("FR_NO_PATH\n");
      break;

    case FR_INVALID_NAME:
      cmdifPrintf("FR_INVALID_NAME\n");
      break;

    case FR_DENIED:
      cmdifPrintf("FR_DENIED\n");
      break;

    case FR_EXIST:
      cmdifPrintf("FR_EXIST\n");
      break;

    case FR_INVALID_OBJECT:
      cmdifPrintf("FR_INVALID_OBJECT\n");
      break;

    case FR_WRITE_PROTECTED:
      cmdifPrintf("FR_WRITE_PROTECTED\n");
      break;

    case FR_INVALID_DRIVE:
      cmdifPrintf("FR_INVALID_DRIVE\n");
      break;

    case FR_NOT_ENABLED:
      cmdifPrintf("FR_NOT_ENABLED\n");
      break;

    case FR_NO_FILESYSTEM:
      cmdifPrintf("FR_NO_FILESYSTEM\n");
      break;

    case FR_MKFS_ABORTED:
      cmdifPrintf("FR_MKFS_ABORTED\n");
      break;

    case FR_TIMEOUT:
      cmdifPrintf("FR_TIMEOUT\n");
      break;

    case FR_LOCKED:
      cmdifPrintf("FR_LOCKED\n");
      break;

    case FR_NOT_ENOUGH_CORE:
      cmdifPrintf("FR_NOT_ENOUGH_CORE\n");
      break;

    case FR_TOO_MANY_OPEN_FILES:
      cmdifPrintf("FR_NOT_ENOUGH_CORE\n");
      break;

    case FR_INVALID_PARAMETER:
      cmdifPrintf("FR_INVALID_PARAMETER\n");
      break;

    default:
      break;
  }
}

FRESULT fatfsScanFiles(char* path)
{
  FRESULT res;
  DIR dir;
  FILINFO fno;


  res = f_opendir(&dir, path);                       /* Open the directory */
  if (res == FR_OK)
  {
    for (;;)
    {
      res = f_readdir(&dir, &fno);                   /* Read a directory item */
      if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
      if (fno.fattrib & AM_DIR)
      {                    /* It is a directory */
        printf(" %s/%s \n", path, fno.fname);
      }
      else
      {                                       /* It is a file. */
        printf(" %s/%32s \t%d bytes\n", path, fno.fname, (int)fno.fsize);
      }
    }
    f_closedir(&dir);
  }

  return res;
}



#if HW_FATFS_USE_CMDIF == 1
void fatfsCmdifInit(void)
{
  if (cmdifIsInit() == false)
  {
    cmdifInit();
  }
  cmdifAdd("fatfs", fatfsCmdif);
}

void fatfsCmdif(void)
{
  bool ret = true;

  if (cmdifGetParamCnt() == 1 && cmdifHasString("info", 0) == true)
  {
    cmdifPrintf("fatfs init \t: %d\n", is_init);
    if (sdIsDetected() == true)
    {
      cmdifPrintf("sd card \t: Exist\n");
    }
    else
    {
      cmdifPrintf("sd card \t: Empty\n");
    }

    if (is_init == true)
    {
      FATFS *fs;
      DWORD fre_clust, fre_sect, tot_sect;
      FRESULT res;

      /* Get volume information and free clusters of drive 1 */
      res = f_getfree("", &fre_clust, &fs);
      if (res == FR_OK)
      {
        /* Get total sectors and free sectors */
        tot_sect = (fs->n_fatent - 2) * fs->csize;
        fre_sect = fre_clust * fs->csize;

        /* Print the free space (assuming 512 bytes/sector) */
        cmdifPrintf("%10lu KiB total drive space.\n%10lu KiB available.\n", tot_sect / 2, fre_sect / 2);
      }
      else
      {
        cmdifPrintf(" err : ");
        fatfsPrintErr(res);
      }
    }
  }
  else if (cmdifGetParamCnt() == 1 && cmdifHasString("dir", 0) == true)
  {
    FRESULT res;

    res = fatfsScanFiles("/");

    if (res != FR_OK)
    {
      cmdifPrintf(" err : ");
      fatfsPrintErr(res);
    }
  }
  else if (cmdifGetParamCnt() == 1 && cmdifHasString("test", 0) == true)
  {
    FRESULT fp_ret;
    FIL log_file;
    uint32_t pre_time;

    pre_time = millis();
    fp_ret = f_open(&log_file, "1.csv", FA_CREATE_ALWAYS | FA_WRITE);
    if (fp_ret == FR_OK)
    {
      f_printf(&log_file, "Num, ");
      f_printf(&log_file, "StateMotor, ");
      f_printf(&log_file, "StateSensor, ");
      f_printf(&log_file, "STO, ");
      f_printf(&log_file, "\n");

      for (int i=0; i<1024; i++)
      {
        f_printf(&log_file, "%d \n", i);
      }

      f_close(&log_file);
    }
    else
    {
      cmdifPrintf("f_open fail\r\n");
    }
    cmdifPrintf("%d ms\r\n", millis()-pre_time);
  }
  else
  {
    ret = false;
  }

  if (ret == false)
  {
    cmdifPrintf( "fatfs info \n");
    cmdifPrintf( "fatfs dir \n");
    cmdifPrintf( "fatfs test \n");
  }
}
#endif /* _USE_HW_CMDIF_FATFS */

#endif /* _USE_HW_FATFS */
