/*
 * fatfs.h
 *
 *  Created on: 2018. 2. 11.
 *      Author: baram
 */

#ifndef FATFS_H_
#define FATFS_H_



#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_FATFS

#include "lib/FatFs/src/ff_gen_drv.h"
#include "driver/sd_diskio.h"




bool fatfsInit(void);



#endif /* _USE_HW_FATFS */

#ifdef __cplusplus
}
#endif

#endif /* FATFS_H_ */
