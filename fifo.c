/**
 * @author Neo Xu (neo.xu1990@gmail.com)
 * @license The MIT License (MIT)
 * 
 * Copyright (c) 2019 Neo Xu
 * 
*/
#include "fifo.h"
#include "string.h"

#define FIFO_WRITE(fifo, data)\
  if(fifo->data_type == fifo_data_8bit)\
    ((uint8_t*)(fifo->buff))[fifo->write_index] = *(uint8_t*)data;\
  else if(fifo->data_type == fifo_data_16bit)\
    ((uint16_t*)(fifo->buff))[fifo->write_index] = *(uint16_t*)data;\
  else if(fifo->data_type == fifo_data_32bit)\
    ((uint32_t*)(fifo->buff))[fifo->write_index] = *(uint32_t*)data;\
  else if(fifo->data_type == fifo_data_64bit)\
    ((uint64_t*)(fifo->buff))[fifo->write_index] = *(uint64_t*)data;\
  else return fifo_err_data;

#define FIFO_READ(fifo, data)\
  if(fifo->data_type == fifo_data_8bit)\
    *(uint8_t*)data = ((uint8_t*)(fifo->buff))[fifo->read_index];\
  else if(fifo->data_type == fifo_data_16bit)\
    *(uint16_t*)data = ((uint16_t*)(fifo->buff))[fifo->read_index];\
  else if(fifo->data_type == fifo_data_32bit)\
    *(uint32_t*)data = ((uint32_t*)(fifo->buff))[fifo->read_index];\
  else if(fifo->data_type == fifo_data_64bit)\
    *(uint64_t*)data = ((uint64_t*)(fifo->buff))[fifo->read_index];\
  else return fifo_err_data;

/**
 * @brief initialize an empty fifo.
 * @para fifo: the structure to hold fifo info.
 * @para data_type: the data type that store into fifo.
 * @para buff: the buffer used to store data.
 * @para len: the buffer length in data_type.
 * @return none.
*/
void fifo_init(fifo_t *fifo, fifo_data_t data_type, void *buff, uint32_t len)
{
  fifo->buff = buff;
  fifo->buff_size = len;
  fifo->data_type = data_type;
  fifo->write_index = 0;
  fifo->read_index = 0;
  fifo->data_count = 0;
}

/**
 * @brief get current fifo status(data count in fifo)
 * @return count of data in fifo.
*/
uint32_t fifo_status(fifo_t *fifo)
{
  return fifo->data_count;
}

/**
 * @brief Check if FIFO is full
 * @return return 1 if FIFO is full, otherwise, return 0.
*/
int32_t fifo_is_full(fifo_t *fifo){
  if(fifo->data_count == fifo->buff_size)
    return 1;
  return 0;
}

/**
 * @brief write one data to fifo.
 * @param data: the pointer to data to write.
 * @return fifo_err_ok if succeeded.
*/
fifo_err_t fifo_push(fifo_t *fifo, void* data){
  if(fifo->data_count >= fifo->buff_size)
    return fifo_err_full; //fifo is already full
  //write one data to fifo and modify windex and data_count
  FIFO_WRITE(fifo, data)
  //enter critical area
  FIFO_ENTER_CRITICAL();
  fifo->write_index ++;
  if(fifo->write_index == fifo->buff_size)
    fifo->write_index = 0;
  fifo->data_count ++;
  FIFO_EXIT_CRITICAL();
  //exit critical area
  return fifo_err_ok;
}

/**
 * @brief read one byte from fifo.
 * @para data: the pointer to save data read.
 * @return return fifo_err_ok if succeeded
*/
fifo_err_t fifo_pop(fifo_t *fifo, void *data){
  if(data == 0) return fifo_err_nullp;
  if(fifo->data_count){ //we have data to read.
    FIFO_READ(fifo, data)
    //enter critical area.
    FIFO_ENTER_CRITICAL();
    fifo->read_index ++;
    if(fifo->read_index == fifo->buff_size)
      fifo->read_index = 0;  //rewind to zero.
    fifo->data_count --;
    FIFO_EXIT_CRITICAL();
    //exit critical area.
    return fifo_err_ok;
  }
  return fifo_err_empty;
}

/**
 * @brief write a buffer to fifo.
 * @param buff: the pointer to data to be stored to fifo.
 * @param plen: the pointer to store how many data is in buffer,
 *        and used to store the actual stored data count.
 * @return fifo_err_ok if succeeded. If not all data are written, also return fifo_err_ok,
 *        read plen to determine the actual written data count.
*/
fifo_err_t fifo_write(fifo_t *fifo, void *buff, uint32_t *plen){
  uint32_t write_len = *plen;
  uint32_t temp;
  if(fifo->data_count >= fifo->buff_size)
    return fifo_err_full;
  /* step1, how many data we can write? */
  write_len = write_len > fifo->buff_size-fifo->data_count?\
              fifo->buff_size-fifo->data_count : write_len;
  /* step2, how many data can be written directly to the buffer. */
  temp = fifo->buff_size - fifo->write_index; //we write from 'write_index' to end of buffer.
  if(write_len < temp){ //good, we can write all data at once.
    uint8_t byte_size = 1<<fifo->data_type;
    memcpy((uint8_t*)(fifo->buff)+fifo->write_index*byte_size, buff, write_len*byte_size);
    /* !modify critical pointers. */
    FIFO_ENTER_CRITICAL();
    fifo->write_index += write_len;
    fifo->data_count += write_len;
    FIFO_EXIT_CRITICAL();
    /* exit critical area. */
  }
  else{ //we need two steps to write.
    //firstly, write data until to end of buffer.
    uint8_t byte_size = 1<<fifo->data_type;
    memcpy((uint8_t*)(fifo->buff)+fifo->write_index*byte_size, buff, temp*byte_size);
    //then write from start of buffer.
    if(write_len != temp) //we do need the second write.
      memcpy(fifo->buff, ((uint8_t*)buff) + temp*byte_size, (write_len-temp)*byte_size);
    /* !modify critical pointers. */
    FIFO_ENTER_CRITICAL();
    fifo->write_index = write_len-temp;
    fifo->data_count += write_len;
    FIFO_EXIT_CRITICAL();
    /* exit critical area. */
  }
  *plen = write_len;
  return fifo_err_ok;
}

/**
 * @brief read from fifo to a buffer.
 * @param buff: the buffer used to store data read.
 * @param plen: the pointer to say how many data expected to read. It also stores the
 *        actual data count read to buffer.
 * @return fifo_err_ok if succeeded.
*/
fifo_err_t fifo_read(fifo_t *fifo, void *buff, uint32_t *plen)
{
  uint32_t read_len, temp;
  if(fifo->data_count == 0)
    return fifo_err_empty;
  read_len = fifo->data_count > *plen?\
              *plen: fifo->data_count;
  /* How many data we can read directly from buffer? */
  temp = fifo->buff_size - fifo->read_index;
  if(read_len < temp){  //good, we can read directly.
    uint8_t byte_size = 1<<fifo->data_type;
    memcpy(buff, (uint8_t*)(fifo->buff)+fifo->read_index*byte_size, read_len*byte_size);
    //enter critical area.
    FIFO_ENTER_CRITICAL();
    fifo->read_index += read_len;
    fifo->data_count -= read_len;
    FIFO_EXIT_CRITICAL();
    //exit critical area.
  }
  else{//we need two steps to read all data.
    uint8_t byte_size = 1<<fifo->data_type;
    memcpy(buff, (uint8_t*)(fifo->buff)+fifo->read_index*byte_size, temp*byte_size);
    //then read from buffer start.
    if(read_len != temp)
      memcpy((uint8_t*)buff+temp*byte_size, (uint8_t*)(fifo->buff), (read_len-temp)*byte_size);
    /* enter critical area */
    FIFO_ENTER_CRITICAL();
    fifo->read_index = read_len - temp;
    fifo->data_count -= read_len;
    FIFO_EXIT_CRITICAL();
    //exit critical area.
  }
  *plen = read_len;
  return fifo_err_ok;
}
