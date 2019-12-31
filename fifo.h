/**
 * @author Neo Xu (neo.xu1990@gmail.com)
 * @license The MIT License (MIT)
 * 
 * Copyright (c) 2019 Neo Xu
 * 
*/
#ifndef _FIFO_H_
#define _FIFO_H_
#include "stdint.h"
#include "fifo_conf.h"

#define FIFO_VERSION_MAJOR 0
#define FIFO_VERSION_MINOR 0
#define FIFO_VERSION_PATCH 3

typedef enum
{
  fifo_err_ok = 0,
  fifo_err_full = -1,
  fifo_err_empty = -2,
  fifo_err_nullp = -3,  //null pointer provided.
  fifo_err_data = -4 ,  //data type is not supported.
}fifo_err_t;

typedef enum{
  fifo_data_8bit = 0,
  fifo_data_16bit, //half word
  fifo_data_32bit,
  fifo_data_64bit,  //need compiler's support..
}fifo_data_t;

typedef struct
{
  void *buff;
  fifo_data_t  data_type;
  uint32_t buff_size;
  uint32_t write_index;
  uint32_t read_index;
  uint32_t data_count;
}fifo_t;

void fifo_init(fifo_t *fifo, fifo_data_t data_type, void *buff, uint32_t len);
uint32_t fifo_status(fifo_t *fifo);
fifo_err_t fifo_push(fifo_t *fifo, void *data);
fifo_err_t fifo_pop(fifo_t *fifo, void *data);
fifo_err_t fifo_write(fifo_t *fifo, void *buff, uint32_t *plen);
fifo_err_t fifo_read(fifo_t *fifo, void *buff, uint32_t *plen);

#endif
