/**
 * @author Neo Xu (neo.xu1990@gmail.com)
 * @license The MIT License (MIT)
 * 
 * Copyright (c) 2019 Neo Xu
 * 
*/

#include "../fifo.h"
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"

#define BUFF_SIZE 128

fifo_t test_fifo;
uint8_t buff[BUFF_SIZE];

int main(void){
  uint8_t test_source_data[BUFF_SIZE*2];
  uint8_t test_buff[BUFF_SIZE*2];
  uint32_t len;
  uint32_t data;
  fifo_err_def err;
  
  printf("test started\n");
  //generate test data
  srand(clock());
  for(int i=0; i<BUFF_SIZE; i++){
    test_source_data[i] = (uint8_t)rand();
  }

  fifo_init(&test_fifo, fifo_data_8bit, buff, BUFF_SIZE);
  memcpy(test_buff, test_source_data, BUFF_SIZE);
  len = BUFF_SIZE;
  err = fifo_write(&test_fifo, test_buff, &len);
  if(err != fifo_err_ok){
    printf("fifo write error code:%d\n", err);
    return err;
  }
  //check data in fifo;
  memset(test_buff, 0, BUFF_SIZE);
  len = BUFF_SIZE;
  err = fifo_read(&test_fifo, test_buff, &len);
  if(err != fifo_err_ok){
    printf("fifo read error code:%d\n", err);
    return err;
  }
  for(int i=0; i<BUFF_SIZE; i++)
    if(test_source_data[i] != test_buff[i]){
      printf("data read error at %d, expected[%d], got[%d]\n", err, i, test_source_data[i], test_buff[i]);
      return -10;
    }
  printf("test pass\n");
  return 0;
}