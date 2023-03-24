#ifndef _KEY_H_
#define _KEY_H_

#include "main.h"
#include "ringbuffer.h"


#define KEY_BUFFER_SIZE 8U  /* ����FIFO�����Buffer��С */

/* ������ʵʱ״̬ */
typedef enum
{
	KEY_DOWN = 0,	  /* ��������(�൱�ڴ�������Ч) */
	KEY_UP   = 1,     /* ����̧��(�൱�ڴ�������Ч�� */
	KEY_LONG = 2      /* ��������(�൱�ڴ�������Ч�� */

}_KEY_STATUS;

/* �źŵ���Ч���� */
typedef enum
{
	LOW  = 0,         /* �͵�ƽ��Ч��NPN�� */
	HIGH = 1,         /* �ߵ�ƽ��Ч��PNP�� */

}_KEY_EFFECTIVE_LEVEL;

/* KEY����ṹ�� */
typedef struct key_obj
{
	/* ��Ա */
	ringbuffer_t 				FIFO;						     /* FIFO���� */
	uint8_t      				FIFO_Buffer[KEY_BUFFER_SIZE];    /* FIFO���ڴ� */
	_KEY_EFFECTIVE_LEVEL		effective_Level;                 /* ��Ч�ļ��� */ 
	_KEY_STATUS     			status;							 /* ������ʵʱ״̬ */
	uint8_t         			filter_Count;                    /* �˲��������� */
	uint8_t                     runtime_Count;                   /* ����ʱ������ */
	GPIO_TypeDef                *GPIOx;							 /* GPIO Port */
	uint16_t                    GPIO_Pin;                        /* GPIO Pin */          
	
	/* ���� */
	void (*init)(struct key_obj *);
	void (*runtime)(struct key_obj *);
	
}_KEY_OBJ;

void key_Init(_KEY_OBJ *key);
void key_Runtime(_KEY_OBJ *key);
void key_init(void);
uint8_t KeyScan(void);


#endif