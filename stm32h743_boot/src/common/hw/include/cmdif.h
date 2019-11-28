/*
 * cmdif.h
 *
 *  Created on: 2019. 3. 8.
 *      Author: HanCheol Cho
 */

#ifndef SRC_COMMON_HW_CMDIF_H_
#define SRC_COMMON_HW_CMDIF_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"


#ifdef _USE_HW_CMDIF

#define CMDIF_LIST_MAX            HW_CMDIF_LIST_MAX
#define CMDIF_CMD_STR_MAX         HW_CMDIF_CMD_STR_MAX
#define CMDIF_CMD_BUF_LENGTH      HW_CMDIF_CMD_BUF_LENGTH


typedef struct
{
  int argc;
  char **argv;
} cmdif_param_t;


typedef struct
{
  char  cmd_str[CMDIF_CMD_STR_MAX];
  void (*cmd_func)(void);
} cmdif_cmd_node_t;


typedef struct
{
  bool               init;
  uint8_t            err_code;
  uint8_t            ch;
  uint32_t           baud;
  uint16_t           index;
  bool               exit;

  cmdif_param_t      param;
  cmdif_cmd_node_t  *node;

  char  read_buffer[CMDIF_CMD_BUF_LENGTH];
  char *p_read_buffer;

} cmdif_cmd_t;



bool cmdifInit(void);
bool cmdifIsInit(void);
void cmdifOpen(uint8_t ch, uint32_t baud);
void cmdifAdd(const char *cmd_str, void (*p_func)(void));
void cmdifMain(void);
void cmdifLoop(void);

void cmdifPrintf(const char *fmt, ...);
void cmdifPrint(char *str);
void cmdifPutch(char data);
uint8_t  cmdifGetch(void);
uint32_t cmdifRxAvailable(void);


bool cmdifHasString(const char *p_str, uint8_t index);
unsigned long cmdifGetParam(uint8_t index);
uint32_t cmdifGetParamCnt(void);

#endif


#ifdef __cplusplus
}
#endif


#endif /* SRC_COMMON_HW_CMDIF_H_ */
