/*
 * slot.h
 *
 *  Created on: 2019. 11. 4.
 *      Author: HanCheol Cho
 */

#ifndef SRC_COMMON_HW_INCLUDE_SLOT_H_
#define SRC_COMMON_HW_INCLUDE_SLOT_H_


#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_SLOT

#define SLOT_MAX_CH       HW_SLOT_MAX_CH


bool slotInit(void);
bool slotIsAvailable(uint8_t slot_index);
bool slotRun(uint8_t slot_index);
bool slotRunFromFlash(uint8_t slot_index);
bool slotRunFromFile(const char *file_name);
bool slotDelFromFlash(uint8_t slot_index);
bool slotRunFromFolder(uint8_t slot_index);
bool slotGetTag(uint8_t slot_index, flash_tag_t *p_tag);
bool slotGetTagFromFolder(uint8_t slot_index, flash_tag_t *p_tag);
bool slotGetTagFromFlash(uint8_t slot_index, flash_tag_t *p_tag);
void slotJumpToFw(uint32_t addr);


#endif


#ifdef __cplusplus
}
#endif



#endif /* SRC_COMMON_HW_INCLUDE_SLOT_H_ */
