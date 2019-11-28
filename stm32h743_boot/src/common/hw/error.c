/*
 * error.c
 *
 *  Created on: 2019. 3. 18.
 *      Author: HanCheol Cho
 */



#include "error.h"

#ifdef _USE_HW_ERROR

static uint32_t err_index = 0;

static err_code_t err_code_tbl[ERROR_MAX_CH] = {0,};



err_code_t errorAdd(err_code_t err_code)
{
  err_code_tbl[err_index] = err_code;
  err_index++;
  err_index %= ERROR_MAX_CH;

  return err_code;
}

void errorClear(void)
{

  err_index = 0;
  err_code_tbl[0] = OK;
}

err_code_t errorGet(void)
{
  uint32_t item_index;

  item_index = (err_index - 1) % ERROR_MAX_CH;

  return err_code_tbl[item_index];
}

#endif
