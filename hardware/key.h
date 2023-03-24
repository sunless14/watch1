#ifndef _KEY_H_
#define _KEY_H_

#include "main.h"
#include "ringbuffer.h"


#define KEY_BUFFER_SIZE 8U  /* 按键FIFO对象的Buffer大小 */

/* 按键的实时状态 */
typedef enum
{
	KEY_DOWN = 0,	  /* 按键按下(相当于传感器有效) */
	KEY_UP   = 1,     /* 按键抬起(相当于传感器无效） */
	KEY_LONG = 2      /* 按键长按(相当于传感器有效） */

}_KEY_STATUS;

/* 信号的有效极性 */
typedef enum
{
	LOW  = 0,         /* 低电平有效（NPN） */
	HIGH = 1,         /* 高电平有效（PNP） */

}_KEY_EFFECTIVE_LEVEL;

/* KEY对象结构体 */
typedef struct key_obj
{
	/* 成员 */
	ringbuffer_t 				FIFO;						     /* FIFO对象 */
	uint8_t      				FIFO_Buffer[KEY_BUFFER_SIZE];    /* FIFO的内存 */
	_KEY_EFFECTIVE_LEVEL		effective_Level;                 /* 有效的极性 */ 
	_KEY_STATUS     			status;							 /* 按键的实时状态 */
	uint8_t         			filter_Count;                    /* 滤波器计数器 */
	uint8_t                     runtime_Count;                   /* 运行时计数器 */
	GPIO_TypeDef                *GPIOx;							 /* GPIO Port */
	uint16_t                    GPIO_Pin;                        /* GPIO Pin */          
	
	/* 方法 */
	void (*init)(struct key_obj *);
	void (*runtime)(struct key_obj *);
	
}_KEY_OBJ;

void key_Init(_KEY_OBJ *key);
void key_Runtime(_KEY_OBJ *key);
void key_init(void);
uint8_t KeyScan(void);


#endif