/*
 * ap.c
 *
 *  Created on: 2018. 8. 25.
 *      Author: Baram
 */




#include "ap.h"
#include "util.h"
#include <unistd.h>



#define FLASH_TAG_SIZE      0x400




flash_tag_t    fw_tag;



int32_t getFileSize(char *file_name);
bool addTagToBin(char *src_filename, char *dst_filename);



void apInit(void)
{

}

void apMain(int argc, char *argv[])
{
  bool ret;
  char *port_name;
  char *file_name;
  char *file_type;
  int baud;
  int file_size;
  uint8_t  block_buf[FLASH_TX_BLOCK_LENGTH];
  uint8_t  version_str[128];
  uint8_t  board_str[128];
  static FILE *fp;
  uint8_t errcode;
  uint32_t time_pre;
  bool flash_write_done;
  uint32_t addr;
  size_t readbytes;
  uint32_t percent;
  uint32_t len;
  uint32_t pre_percent = 100;
  uint32_t start_addr = 0;
  uint32_t file_run = 0;

  uint32_t flash_begin;
  uint32_t flash_end;
  char dst_filename[256];
  int slot_number;


  setbuf(stdout, NULL);


  if (argc != 7)
  {
    printf("stm32h743_loader.exe com1 115200 type[fw:image] 0x8040000 file_name run[0:1]\n");
    return;
  }

  printf("\nstm32h743_loader... V191128R1\n\n");

  port_name = (char *)argv[ 1 ];
  baud      = strtol( argv[ 2 ], NULL, 10 );
  file_type = (char *)argv[ 3 ];
  start_addr= (uint32_t)strtoul( argv[ 4 ], NULL, 0 );
  file_name = (char *)argv[ 5 ];
  file_run  = (uint32_t)strtol( argv[ 6 ], NULL, 0 );

  if (start_addr <= 15)
  {
    slot_number = start_addr;
    start_addr = 0x90000000 + 0x200000 * slot_number;

    printf("slot number : %d\n", slot_number);
    printf("     addr   : 0x%X\n", start_addr);
  }

  if(strcmp(file_type, "fw") == 0)
  {
    printf("\r\n@ Make binary (Add Tag)...\r\n");
    strcpy(dst_filename, file_name);
    strcat(dst_filename, ".fw");

    if(addTagToBin(file_name, dst_filename) != true)
    {
      fprintf( stderr, "  Add tag info to binary Fail! \n");
      exit( 1 );
    }

    file_size = getFileSize(dst_filename);


    printf("\nfirmware downloader...\n\n");

    printf("file open\n");
    printf("  file name \t: %s \n", file_name);
    printf("  file size \t: %d B\n", file_size);
    printf("  file addr \t: 0x%X\n", fw_tag.addr_fw);
    printf("  file board\t: %s\n", (char *)fw_tag.board_str);
    printf("  file name \t: %s\n", (char *)fw_tag.name_str);
    printf("  file ver  \t: %s\n", (char *)fw_tag.version_str);

    printf("  file magic\t: 0x%X\n", fw_tag.magic_number);


    if (fw_tag.magic_number != 0x5555AAAA)
    {
      printf("MagicNumber Fail, Wrong Image.\n");
      return;
    }
  }
  else
  {
    strcpy(dst_filename, file_name);

    file_size = getFileSize(dst_filename);


    printf("\nimage downloader...\n\n");

    printf("file open\n");
    printf("  file name \t: %s \n", file_name);
    printf("  file size \t: %d B\n", file_size);
    printf("  file addr \t: 0x%X\n", start_addr);
  }



  printf("port open \t: %s\n", port_name);
  printf("     baud \t: %d\n", baud);

  ret = bootInit(_DEF_UART2, port_name, baud);
  if (ret == true)
  {
    printf("bootInit \t: OK\n");
  }
  else
  {
    printf("bootInit  Fail\n");
    return;
  }




  flash_begin = start_addr;
  flash_end   = start_addr + file_size;


  while(1)
  {
    //-- 보드 이름 확인
    //
    errcode = bootCmdReadBootName(board_str);
    if (errcode == OK)
    {
      printf("boot name \t: %s\n", board_str);
    }
    else
    {
      printf("bootCmdReadBoardName faild \n");
      break;
    }


    //-- 버전 확인
    //
    errcode = bootCmdReadBootVersion(version_str);
    if (errcode == OK)
    {
      printf("boot version \t: %s\n", &version_str[0]);
    }
    else
    {
      printf("bootCmdReadBootVersion faild \n");
      break;
    }

    errcode = bootCmdReadFirmVersion(version_str);
    if (errcode == OK)
    {
      printf("firm version \t: %s\n", &version_str[0]);
    }
    else
    {
      printf("bootCmdReadFirmVersion faild \n");
      break;
    }

    /*
    if (strcmp((const char *)board_str, (const char *)info.board_str) != 0)
    {
      printf("board name is not equal. \n");
      break;
    }
    */

    //-- Flash Erase
    //
    printf("erase fw...\n");
    time_pre = millis();
    errcode = bootCmdFlashErase(flash_begin, flash_end - flash_begin);
    if (errcode == OK)
    {
      printf("erase fw ret \t: OK (%d ms)\n", millis()-time_pre);
    }
    else
    {
      printf("bootCmdFlashEraseFw faild : %d\n", errcode);
      break;
    }


    //-- Flash Write
    //
    if( ( fp = fopen( dst_filename, "rb" ) ) == NULL )
    {
      fprintf( stderr, "Unable to open %s\n", file_name );
      exit(1);
    }


    flash_write_done = false;
    addr = flash_begin;
    time_pre = millis();
    while(1)
    {
      if( !feof( fp ) )
      {
        readbytes = fread( block_buf, 1, FLASH_TX_BLOCK_LENGTH, fp );
        percent = (addr+readbytes-flash_begin)*100/file_size;

        if ((percent%10) == 0 && percent != pre_percent)
        {
          if (percent == 0)
          {
            printf("flash fw \t: %d%% ", percent);
          }
          else
          {
            printf("%d%% ", percent);
          }
          pre_percent = percent;
        }
      }
      else
      {
        break;
      }

      if( readbytes == 0 )
      {
        break;
      }
      else
      {
        len = readbytes;
      }


      for (int retry=0; retry<3; retry++)
      {
        errcode = bootCmdFlashWrite(addr, block_buf, len);
        if( errcode == OK )
        {
          break;
        }
        else
        {
          printf("bootCmdFlashWrite Fail \t: %d \n", errcode);
        }
      }
      if( errcode != OK )
      {
        break;
      }

      addr += len;

      if ((addr-flash_begin) == file_size)
      {
        flash_write_done = true;
        break;
      }
    }
    fclose(fp);

    printf("\r\n");

    if( errcode != OK || flash_write_done == false )
    {
      printf("flash fw fail \t: %d\n", errcode);
      break;
    }
    else
    {
      printf("flash fw ret \t: OK (%d ms) \n", millis()-time_pre);
    }


    if( errcode != OK || flash_write_done == false )
    {
      printf("Download \t: Fail\n");
      return;
    }
    else
    {
      printf("Download \t: OK\n");
    }


    errcode = bootCmdJumpToFw();
    if (errcode == OK)
    {
        printf("jump to fw \t: OK\n");
    }
    else
    {
      printf("jump to fw fail : %d\n", errcode);
    }

    break;
  }


  uartClose(_DEF_UART2);
}

int32_t getFileSize(char *file_name)
{
  int32_t ret = -1;
  static FILE *fp;


  if( ( fp = fopen( file_name, "rb" ) ) == NULL )
  {
    fprintf( stderr, "Unable to open %s\n", file_name );
    exit( 1 );
  }
  else
  {
    fread( (void *)&fw_tag, 1, sizeof(flash_tag_t), fp );

    fseek( fp, 0, SEEK_END );
    ret = ftell( fp );
    fseek( fp, 0, SEEK_SET );
    fclose(fp);
  }

  return ret;
}

bool addTagToBin(char *src_filename, char *dst_filename)
{
  FILE    *p_fd;
  uint8_t *buf;
  size_t   src_len;
  uint16_t t_crc = 0;
  flash_tag_t *p_tag;


  if (!strcmp(src_filename, dst_filename))
  {
    fprintf( stderr, "  src file(%s) and dst file(%s) is same! \n", src_filename, dst_filename );
    exit( 1 );
  }


  /* Open src file */
  if ((p_fd = fopen(src_filename, "rb")) == NULL)
  {
    fprintf( stderr, "  unable to open src file(%s)\n", src_filename );
    exit( 1 );
  }

  fseek( p_fd, 0, SEEK_END );
  src_len = ftell( p_fd );
  fseek( p_fd, 0, SEEK_SET );

  if ((buf = (uint8_t *) calloc(src_len, sizeof(uint8_t))) == NULL)
  {
    fclose(p_fd);
    fprintf( stderr, "  malloc Error \n");
    exit( 1 );
  }


  /* Copy read fp to buf */
  if(fread( &buf[0], 1, src_len, p_fd ) != src_len)
  {
    fclose(p_fd);
    free(buf);
    fprintf( stderr, "  length is wrong! \n" );
    exit( 1 );
  }
  fclose(p_fd);


  /* Calculate CRC16 */
  size_t i;
  for (i = 0; i<src_len - FLASH_TAG_SIZE; i++)
  {
    utilUpdateCrc(&t_crc, buf[FLASH_TAG_SIZE + i]);
  }

  p_tag = (flash_tag_t *)buf;


  if (p_tag->magic_number == 0x5555AAAA)
  {
    free(buf);
    fprintf( stderr, "  already magic number\n");
    return true;
  }
  else if (p_tag->magic_number != 0xAAAA5555)
  {
    free(buf);
    fprintf( stderr, "  wrong magic number 0x%X \n", p_tag->magic_number);
    return false;
  }

  p_tag->magic_number     = 0x5555AAAA;
  p_tag->tag_flash_start  = p_tag->addr_fw;
  p_tag->tag_flash_end    = p_tag->addr_fw + (src_len - FLASH_TAG_SIZE);
  p_tag->tag_flash_length = p_tag->tag_flash_end - p_tag->tag_flash_start;
  p_tag->tag_length       = FLASH_TAG_SIZE;
  strcpy((char *)p_tag->tag_date_str, __DATE__);
  strcpy((char *)p_tag->tag_time_str, __TIME__);
  p_tag->tag_flash_crc = t_crc;


  /* Store data to dst file */
  if ((p_fd = fopen(dst_filename, "wb")) == NULL)
  {
    free(buf);
    fprintf( stderr, "  unable to open dst file(%s)\n", dst_filename );
    exit( 1 );
  }

  if(fwrite(buf, 1, src_len, p_fd) != src_len)
  {
    fclose(p_fd);
    free(buf);
    //_unlink(dst_filename);
    fprintf( stderr, "  total write fail! \n" );
    exit( 1 );
  }

  printf("  created file  : %s (%d KB)\n", dst_filename, (int)((src_len)/1024) );
  printf("  tag fw start  : 0x%08X \n", p_tag->tag_flash_start);
  printf("  tag fw end    : 0x%08X \n", p_tag->tag_flash_end);
  printf("  tag crc       : 0x%04X \n", p_tag->tag_flash_crc);
  printf("  tag date      : %s \n", p_tag->tag_date_str);
  printf("  tag time      : %s \n", p_tag->tag_time_str);

  fclose(p_fd);
  free(buf);

  return true;
}
