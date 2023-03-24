#include "key.h"


/*************************************************************************************************************
* ������: key_Init

* ����:   ��ʼ����������

* ���룺*key: ��������ĵ�ַ

* �������

* ���أ���

note:
      1.�����ʼ���󣬰����Ķ�����ܱ�����ʹ�á�
 */
void key_Init(_KEY_OBJ *key)
{
	ringbuffer_init(&key->FIFO,key->FIFO_Buffer,KEY_BUFFER_SIZE);  /* ��ʼ��FIFO */
	
	/* ����ָ�� */
	key->runtime = key_Runtime;
}

/*************************************************************************************************************
* ������: key_Scan

* ����: ͨ����ȡGPIO�ĵ�ƽ����ȡ������״̬

* ���룺*key: ��������ĵ�ַ

* �������

* ���أ���

note:
 */
static void key_Scan(_KEY_OBJ *key)
{
	GPIO_PinState level = GPIO_PIN_RESET;
	_KEY_STATUS status = KEY_UP;
	
	level = HAL_GPIO_ReadPin(key->GPIOx,key->GPIO_Pin);  /* ��ȡGPIO�ĵ�ƽ */
	
	/* �жϰ�������Ч���� */
	/* �����ǵ͵�ƽ��Ч */
	if(LOW == key->effective_Level)
	{
		/* ����GPIO�Ƿ��鵽�͵�ƽ */
		if(GPIO_PIN_RESET == level)
		{
			status = KEY_DOWN;	/* ���������� */
		}
		else
		{
			status = KEY_UP;	/* ����̧�� */
		}
	}
    /* �����Ǹߵ�ƽ��Ч */	
	else
	{
		/* ����GPIO�Ƿ��⵽�ߵ�ƽ */
		if(GPIO_PIN_SET == level)
		{
			status = KEY_DOWN;  /* ���������� */
		}
		else
		{
			status = KEY_UP;    /* ����̧�� */
		}
	}
	
	ringbuffer_in(&key->FIFO,(uint8_t *)&status,1);  /* ����״̬����FIFO */
}

/*************************************************************************************************************
* ������: key_Runtime

* ����: ����ʱ�����ã�ʵ�ְ�������˲�(״̬��ԭ��

* ���룺*key: ��������ĵ�ַ

* �������

* ���أ���

note:
	 1������˲�ʱ�� = ��ʱ��������T * ���������Աfilter_Count
		���磬��ʱ��������T = 5ms ����Աfilter_Count = 2 ,����˲�ʱ�� = 5ms * 2 = 10ms
 */
void key_Runtime(_KEY_OBJ *key)
{
	_KEY_STATUS status = KEY_UP;
	
	key_Scan(key);  /* ��ȡ������״̬������FIFO */
	
	ringbuffer_out(&key->FIFO,(uint8_t *)&status,1);  /* ��FIFO��ȡ��1������״̬ */
	
	/* ״̬�� */
	switch(key->status)
	{
		/* ״̬����״̬һ����ţ̌�� */
		case KEY_UP:
			if(KEY_DOWN == status)  
			{
				key->runtime_Count++;  /* ���������£��ۼ�һ������ */
			}
			else
			{
				/* ��λ */
				key->runtime_Count = 0; 
				key->status = KEY_UP;  
			}
			
			/* ������ = �趨�������� */
			if(key->filter_Count == key->runtime_Count)
			{
				key->status = KEY_DOWN;  /* ������һ��״̬���׶� */
			}				
			break;
			
		/* ״̬����״̬������ť�����£� */
		case KEY_DOWN:
			if(KEY_DOWN == status)
			{
				key->runtime_Count++;  /* ���������£��ۼ�һ������ */ 
			}
			else
			{
				/* ��λ */
				key->runtime_Count = 0;
				key->status = KEY_UP;
			}
			
			/* ������ = �趨��������2�� */
			if(key->filter_Count * 2 == key->runtime_Count)
			{
				key->status = KEY_LONG;  /* ������һ��״̬���׶� */
			}
			break;
			
		/* ״̬����״̬������ť�������� */
		case KEY_LONG:
			if(KEY_UP == status)
			{
			    /* ��λ */
				key->runtime_Count = 0;
				key->status = KEY_UP;
			}
			break;
		default:
			break;
	}

}
