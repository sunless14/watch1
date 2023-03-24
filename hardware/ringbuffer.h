#ifndef _RINGBUFFER_H_
#define _RINGBUFFER_H_

#include "main.h"

#define FIFO_BUFFER_SIZE 0x800  /* FIFO??? */

/*?????????*/
typedef struct
{
    uint8_t  *buffer;
    uint16_t size;	/* ???????? */
    uint16_t in;    /* ????????? */
    uint16_t out;   /* ????????? */

} ringbuffer_t;


void ringbuffer_init(ringbuffer_t *fifo, uint8_t *buffer, uint16_t size);

uint16_t ringbuffer_getUsedSize(ringbuffer_t *fifo);
uint16_t ringbuffer_getRemainSize(ringbuffer_t *fifo);
uint8_t ringbuffer_isEmpty(ringbuffer_t *fifo);

void ringbuffer_in(ringbuffer_t *fifo, uint8_t *data, uint16_t len);
uint8_t ringbuffer_in_check(ringbuffer_t *fifo, uint8_t *data, uint16_t len);
uint16_t ringbuffer_out(ringbuffer_t *fifo, uint8_t *buf, uint16_t len);



#endif /* BSP_RINGBUFFER_H_ */





