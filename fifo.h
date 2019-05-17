/**
 * @author Neo Xu (neo.xu1990@gmail.com)
 * @license The MIT License (MIT)
 * 
 * Copyright (c) 2019 Neo Xu
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
*/
#ifndef _FIFO_H_
#define _FIFO_H_
#include "stdint.h"

#define FIFO_VERSION_MAJOR 0
#define FIFO_VERSION_MINOR 0
#define FIFO_VERSION_PATCH 2

#define FIFO_DIS_INT() //disableInterrupts() //disable interrupt
#define FIFO_EN_INT()  //enableInterrupts() //enable interrupt again.

typedef enum
{
  fifo_err_ok = 0,
  fifo_err_full = -1,
  fifo_err_empty = -2,
  fifo_err_nullp = -3,  //null pointer provided.
  fifo_err_data = -4 ,  //data type is not supported.
}fifo_err_def;

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

#define FIFO_DATA_POINTER(p, fifo_data_type)  //convert pointer p to fifo_data_t\
                (0)

void fifo_init(fifo_t *fifo, fifo_data_t data_type, void *buff, uint32_t len);
uint32_t fifo_status(fifo_t *fifo);
fifo_err_def fifo_push(fifo_t *fifo, void *data);
fifo_err_def fifo_pop(fifo_t *fifo, void *data);
fifo_err_def fifo_write(fifo_t *fifo, void *buff, uint32_t *plen);
fifo_err_def fifo_read(fifo_t *fifo, void *buff, uint32_t *plen);

#endif
