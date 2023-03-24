/*
 * bsp_ringbuffer.c
 *
 *  Created on: 2021?2?1?
 *      Author: Administrator
 */


#include "ringbuffer.h"

/**
  * @brief  fifo???
  * @param  fifo: ??
  * @param  buffer: fifo????
  * @param  size: ?????
  * @retval None
  */
void ringbuffer_init(ringbuffer_t *fifo, uint8_t *buffer, uint16_t size)
{
    fifo->buffer = buffer;
    fifo->size = size;
    fifo->in = 0;
    fifo->out = 0;
}


/**
  * @brief  ?????????
  * @param  fifo: ??
  * @retval uint16_t: ?????
  */
uint16_t ringbuffer_getUsedSize(ringbuffer_t *fifo)
{
    if (fifo->in >= fifo->out)
        return (fifo->in - fifo->out);
    else
        return (fifo->size - fifo->out + fifo->in);
}


/**
  * @brief  ???????
  * @param  fifo: ??
  * @retval uint16_t: ????
  */
uint16_t ringbuffer_getRemainSize(ringbuffer_t *fifo)
{
    return (fifo->size - ringbuffer_getUsedSize(fifo) - 1);
}


/**
  * @brief  FIFO????
  * @param  fifo: ??
  * @retval uint8_t: 1 ?? 0 ???(???)
  */
uint8_t ringbuffer_isEmpty(ringbuffer_t *fifo)
{
    return (fifo->in == fifo->out);
}


/**
  * @brief  ??????????(???????)
  * @param  fifo: ??
  * @param  data: &#&
  * @param  len: &#&
  * @retval none
  */
void ringbuffer_in(ringbuffer_t *fifo, uint8_t *data, uint16_t len)
{
    for (int i = 0; i < len; i++)
    {
        fifo->buffer[fifo->in] = data[i];
        fifo->in = (fifo->in + 1) % fifo->size;
    }
}


/**
  * @brief  ??????????(???????,????????)
  * @param  fifo: ??
  * @param  data: &#&
  * @param  len: &#&
  * @retval uint8_t: 0 ?? 1??(????)
  */
uint8_t ringbuffer_in_check(ringbuffer_t *fifo, uint8_t *data, uint16_t len)
{
    uint16_t remainsize = ringbuffer_getRemainSize(fifo);

    if (remainsize < len) //????
        return 1;

    ringbuffer_in(fifo, data, len);

    return 0;
}


/**
  * @brief  ??????????
  * @param  fifo: ??
  * @param  buf: ????
  * @param  len: ??????
  * @retval uint16_t: ??????
  */
uint16_t ringbuffer_out(ringbuffer_t *fifo, uint8_t *buf, uint16_t len)
{
    uint16_t remainToread = ringbuffer_getUsedSize(fifo);

    if (remainToread > len)
    {
        remainToread = len;
    }

    for (int i = 0; i < remainToread; i++)
    {
        buf[i] = fifo->buffer[fifo->out];

        fifo->out = (fifo->out + 1) % fifo->size;
    }

    return remainToread;
}


