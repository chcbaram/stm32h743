/*
 * resize.h
 *
 *  Created on: 2019. 9. 7.
 *      Author: Baram
 */

#ifndef SRC_HW_CORE_RESIZE_H_
#define SRC_HW_CORE_RESIZE_H_


#ifdef __cplusplus
extern "C" {
#endif


#include "def.h"

typedef struct
{
  int32_t  w;
  int32_t  h;
  uint16_t *p_data;
} resize_image_t;


void resizeImage(resize_image_t *src, resize_image_t *dest);
void resizeImageNearest(resize_image_t *src, resize_image_t *dest);
void resizeImageFast(resize_image_t *src, resize_image_t *dest);


#ifdef __cplusplus
}
#endif


#endif /* SRC_HW_CORE_RESIZE_H_ */
