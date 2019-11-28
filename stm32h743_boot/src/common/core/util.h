/*
 * util.h
 *
 *  Created on: 2017. 2. 13.
 *      Author: HanCheol Cho
 */

#ifndef UTIL_H_
#define UTIL_H_



#ifdef __cplusplus
 extern "C" {
#endif



typedef struct
{
  uint32_t range_start;
  uint32_t range_end;

  uint32_t node_addr_start;
  uint32_t node_addr_end;
  uint32_t node_length;
  uint32_t node_byte_index;
} data_range_t;




bool utilGetRange(data_range_t *p_range);
uint32_t utilConvert8ToU32 (uint8_t *p_data);
uint16_t utilConvert8ToU16 (uint8_t *p_data);

void utilUpdateCrc(uint16_t *p_crc_cur, uint8_t data_in);

#ifdef __cplusplus
}
#endif



#endif /* UTIL_H_ */
