/*
 * usb.h
 *
 *  Created on: 2018. 3. 16.
 *      Author: HanCheol Cho
 */

#ifndef SRC_HW_USB_CDC_USB_H_
#define SRC_HW_USB_CDC_USB_H_


#include "hw_def.h"


//#define USE_USB_FS


#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_cdc_interface.h"




void usbInit(void);
void usbDeInit(void);

#endif /* SRC_HW_USB_CDC_USB_H_ */
