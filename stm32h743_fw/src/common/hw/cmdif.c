/*
 * cmdif.c
 *
 *  Created on: 2019. 3. 8.
 *      Author: HanCheol Cho
 */




#include "cmdif.h"
#include <string.h>

#ifdef _USE_HW_CMDIF
#include "uart.h"



//-- Internal Variables
//
cmdif_cmd_node_t cmdif_cmd_node[CMDIF_LIST_MAX];
cmdif_cmd_t      cmdif_cmd;


static const char *delim = " \f\n\r\t\v";



//-- External Variables
//


//-- Internal Functions
//
void    cmdifResetBuffer(void);
int     cmdifGetCmdString(char *s, int *count);
uint8_t cmdifReadByte(char *p_data);

int parseCommandArgs(char *cmdline, char **argv);
void upperStr( char *Str );

void cmdifCmdExit(void);
void cmdifCmdShowList(void);
void cmdifCmdMemoryDump(void);
void cmdifCmdMemoryWrite32(void);



//-- External Functions
//





bool cmdifInit(void)
{
  uint32_t i;


  cmdif_cmd.index    = 0;
  cmdif_cmd.err_code = 0;
  cmdif_cmd.init     = true;
  cmdif_cmd.node     = &cmdif_cmd_node[0];


  for(i=0; i<CMDIF_LIST_MAX; i++)
  {
    cmdif_cmd.node[i].cmd_str[0] = 0;
    cmdif_cmd.node[i].cmd_func   = NULL;
  }

  cmdifResetBuffer();


  cmdifAdd("help", cmdifCmdShowList);
  cmdifAdd("exit", cmdifCmdExit);

  cmdifAdd("md",   cmdifCmdMemoryDump);
  cmdifAdd("mw32", cmdifCmdMemoryWrite32);


  return true;
}

bool cmdifIsInit(void)
{
  return cmdif_cmd.init;
}

void cmdifOpen(uint8_t ch, uint32_t baud)
{
  cmdif_cmd.ch   = ch;
  cmdif_cmd.baud = baud;

  uartOpen(ch, baud);
}

void cmdifLoop(void)
{
  cmdif_cmd.exit = false;

  while(1)
  {
    cmdifMain();

    if (cmdif_cmd.exit == true)
    {
      break;
    }
  }
}

void cmdifMain(void)
{
  int   cmdlp, argc, ret, count;
  uint32_t cmdchk = 0;
  char *argv[128];

  count = (int)cmdif_cmd.p_read_buffer - (int)cmdif_cmd.read_buffer;

  ret = cmdifGetCmdString(cmdif_cmd.p_read_buffer, &count);

  if(count !=((int)cmdif_cmd.p_read_buffer-(int)cmdif_cmd.read_buffer))
  {
    cmdif_cmd.p_read_buffer = cmdif_cmd.read_buffer+count;
  }

  if(ret ==0)
  {
    cmdifPrint("\n");
    argc = parseCommandArgs( cmdif_cmd.read_buffer, argv );

    if (argc)
    {
      upperStr( argv[0] );
      cmdlp = 0;
      cmdchk=0;

      while( cmdif_cmd.node[cmdlp].cmd_str[0] )
      {
        if( strcmp( argv[0], cmdif_cmd.node[cmdlp].cmd_str ) == 0 )
        {
          cmdchk++;
          cmdif_cmd.param.argc = argc;
          cmdif_cmd.param.argv = argv;
          cmdif_cmd.node[cmdlp].cmd_func();
          cmdifPrint( "\n");
          break;
        }
        cmdlp++;
      }
      if(cmdchk == 0)
      {
        cmdifPrint("wrong command..\n");
      }
    }
    cmdifResetBuffer();
  }
}

void cmdifAdd(const char *cmd_str, void (*p_func)(void))
{
  uint16_t index;


  if (cmdif_cmd.index >= CMDIF_LIST_MAX || cmdif_cmd.init != true || cmdif_cmd.node == NULL)
  {
    cmdif_cmd.err_code = 1;
    return;
  }

  index = cmdif_cmd.index;


  strcpy(cmdif_cmd.node[index].cmd_str, cmd_str);
  cmdif_cmd.node[index].cmd_func = p_func;

  upperStr(cmdif_cmd.node[index].cmd_str);

  cmdif_cmd.index++;
}

void cmdifCmdExit(void)
{
  cmdif_cmd.exit = true;

  cmdifPrint("exit...\n");
}

void cmdifCmdShowList(void)
{
  int cmdlp = 0;

  cmdifPrint("\n---------- cmd list ---------\n");
  while( cmdif_cmd.node[cmdlp].cmd_str[0] )
  {
    cmdifPrint(cmdif_cmd.node[cmdlp].cmd_str);
    cmdifPrint("\n");
    cmdlp++;
  }
  cmdifPrint("\n-----------------------------\n");
}

void cmdifCmdMemoryDump(void)
{
  int idx, size = 16;
  unsigned int *addr;
  int idx1, i;
  unsigned int *ascptr;
  unsigned char asc[4];

  int argc = cmdif_cmd.param.argc;
  char **argv = cmdif_cmd.param.argv;


  if(argc <2)
  {
    cmdifPrintf(">> md addr [size] \n");
    return;
  }

  if(argc>2)
  {
    size = (int) strtoul((const char * ) argv[2], (char **)NULL, (int) 0);
  }
  addr   = (unsigned int *)strtoul((const char * ) argv[1], (char **)NULL, (int) 0);
  ascptr = (unsigned int *)addr;

  cmdifPrintf ("\n   ");
  for (idx = 0; idx<size; idx++)
  {
    if((idx%4) == 0)
    {
      cmdifPrintf (" 0x%08X: ", (unsigned int)addr);
    }
    cmdifPrintf (" 0x%08X", *(addr));
    if ((idx%4) == 3)
    {
      cmdifPrintf ("  |");
      for (idx1= 0; idx1< 4; idx1++)
      {
        memcpy((char *)asc, (char *)ascptr, 4);
        for (i=0;i<4;i++)
        {
          if (asc[i] > 0x1f && asc[i] < 0x7f)
          {
            cmdifPrintf ("%c", asc[i]);
          }
          else
          {
            cmdifPrintf (".");
          }
        }
        ascptr+=1;
      }
      cmdifPrintf ("|\n   ");
    }
    addr++;
  }
  cmdifPrintf ("\n");
}

void cmdifCmdMemoryWrite32(void)
{
  unsigned long *ptrTo;
  int            lp;

  int argc = cmdif_cmd.param.argc;
  char **argv = cmdif_cmd.param.argv;


  if( argc < 3 )
  {
    cmdifPrintf( "mw32 addr value [value] ...\n");
    return;
  }

  ptrTo = (unsigned long *)strtoul( (const char * ) argv[1], (char **)NULL, 0);

  for( lp = 2; lp < argc; lp++ )
  {
    ptrTo[ lp - 2 ] = strtoul( (const char * ) argv[lp], NULL, 0);
  }
}



void cmdifResetBuffer(void)
{
  memset(cmdif_cmd.read_buffer, 0x00, CMDIF_CMD_BUF_LENGTH);
  cmdif_cmd.p_read_buffer = (char *)cmdif_cmd.read_buffer;

  cmdifPrint("cmdif>> ");
}


void cmdifPrint(char *str)
{
  uartPrintf(cmdif_cmd.ch, (const char *)str);
}

void cmdifPrintf(const char *fmt, ...)
{
  va_list arg;
  va_start (arg, fmt);
  int32_t len;
  static char print_buffer[255];


  len = vsnprintf(print_buffer, 255, fmt, arg);
  va_end (arg);

  uartWrite(cmdif_cmd.ch, (uint8_t *)print_buffer, len);
}

void cmdifPutch(char data)
{
  uartPutch(cmdif_cmd.ch, data);
}

uint8_t cmdifGetch(void)
{
  return uartGetch(cmdif_cmd.ch);
}

uint32_t cmdifRxAvailable(void)
{
  return uartAvailable(cmdif_cmd.ch);
}

uint8_t cmdifReadByte(char *p_data)
{
  uint8_t ret = 0;

  if (uartAvailable(cmdif_cmd.ch) > 0)
  {
    *p_data = uartRead(cmdif_cmd.ch);
    ret = 1;
  }

  return ret;
}

int parseCommandArgs(char *cmdline, char **argv)
{
  char *tok;
  char *next_ptr;
  int argc = 0;

  argv[argc] = NULL;

  for (tok = strtok_r(cmdline, delim, &next_ptr); tok; tok = strtok_r(NULL, delim, &next_ptr))
  {
    argv[argc++] = tok;
  }

  return argc;
}


int cmdifGetCmdString(char *s, int *count)
{
  int ret = -1;
  char  c;
  int cnt = *count;

  *count = 0;


  while(cmdifReadByte(&c))
  {
    if( c == 0xd /* CR */ )
    {
      *s = 0;
      ret = 0;
      break;
    }

    switch( c )
    {
      case 0x08 : // BS
        if (cnt > 0)
        {
          cnt--; *s-- = ' ';
          cmdifPrint("\b \b");
        }
        break;

      default:
        cnt++;
        *s++ = c;
        cmdifPutch(c);
        break;
    }
  }

  *count = cnt;
  return ret;
}

void upperStr( char *Str )
{
   while( *Str ){ *Str = toupper( *Str ); Str++; }
}

bool cmdifHasString(const char *p_str, uint8_t index)
{
  if ((cmdif_cmd.param.argc - 1) <= index)
  {
    return false;
  }

  if(strcmp(p_str, cmdif_cmd.param.argv[index+1]) == 0)
  {
    return true;
  }

  return false;
}

unsigned long cmdifGetParam(uint8_t index)
{
  if ((cmdif_cmd.param.argc - 1) <= index)
  {
    return 0;
  }


  return  strtoul((const char * ) cmdif_cmd.param.argv[index+1], (char **)NULL, (int) 0);
}

uint32_t cmdifGetParamCnt(void)
{
  return cmdif_cmd.param.argc - 1;
}




#endif
