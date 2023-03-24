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


    /* ����KEY0 */
	   _KEY_OBJ KEY0 =
   {
	   .status           = KEY_UP,          /* �����ĳ�ʼ״̬����̧�� */
	   .effective_Level  = LOW,             /* ����ԭ��ͼ�������ǵ͵�ƽ��Ч��NPN�� */
	   .filter_Count     = 2,               /* �˲�����2�����2msɨ��һ�Σ���ô�˲�ʱ�����4ms */
	   .GPIOx            = key0_GPIO_Port,  /* KEY0��GPIO Port */
	   .GPIO_Pin         = key0_Pin,        /* KEY0��GPIO Pin */
	   .init             = key_Init,        /* ���캯�� */
   }; 
	  _KEY_OBJ KEY1 =
   {
	   .status           = KEY_UP,          /* �����ĳ�ʼ״̬����̧�� */
	   .effective_Level  = LOW,             /* ����ԭ��ͼ�������ǵ͵�ƽ��Ч��NPN�� */
	   .filter_Count     = 2,               /* �˲�����2�����2msɨ��һ�Σ���ô�˲�ʱ�����4ms */
	   .GPIOx            = key1_GPIO_Port,  /* KEY1��GPIO Port */
	   .GPIO_Pin         = key1_Pin,        /* KEY1��GPIO Pin */
	   .init             = key_Init,        /* ���캯�� */
   };
	 _KEY_OBJ KEY2 =
   {
	   .status           = KEY_UP,          /* �����ĳ�ʼ״̬����̧�� */
	   .effective_Level  = LOW,             /* ����ԭ��ͼ�������ǵ͵�ƽ��Ч��NPN�� */
	   .filter_Count     = 2,               /* �˲�����2�����2msɨ��һ�Σ���ô�˲�ʱ�����4ms */
	   .GPIOx            = key2_GPIO_Port,  /* KEY2��GPIO Port */
	   .GPIO_Pin         = key2_Pin,        /* KEY2��GPIO Pin */
	   .init             = key_Init,        /* ���캯�� */
   };
	 _KEY_OBJ KEY3 =
   {
	   .status           = KEY_UP,          /* �����ĳ�ʼ״̬����̧�� */
	   .effective_Level  = LOW,             /* ����ԭ��ͼ�������ǵ͵�ƽ��Ч��NPN�� */
	   .filter_Count     = 2,               /* �˲�����2�����2msɨ��һ�Σ���ô�˲�ʱ�����4ms */
	   .GPIOx            = key3_GPIO_Port,  /* KEY3��GPIO Port */
	   .GPIO_Pin         = key3_Pin,        /* KEY3��GPIO Pin */
	   .init             = key_Init,        /* ���캯�� */
   };

void key_init(void){
	 KEY0.init(&KEY0);                      /* ��ʼ��KEY0 */ 
   KEY1.init(&KEY1);	                    /* ��ʼ��KEY1 */ 
   KEY2.init(&KEY2);	                    /* ��ʼ��KEY2 */
	 KEY3.init(&KEY3);	                    /* ��ʼ��KEY3 */
}

uint8_t KeyScan(void){
	 KEY0.runtime(&KEY0);                   /* ɨ��KEY0 */ 
   KEY1.runtime(&KEY1);	                  /* ɨ��KEY1 */
	 KEY2.runtime(&KEY2);	                  /* ɨ��KEY2 */
	 KEY3.runtime(&KEY3);	                  /* ɨ��KEY3 */
	 
	 if(KEY0.status == KEY_LONG) return 11;          /* ���KEY0�Ƿ񱻰��� */
	 else if(KEY1.status == KEY_LONG) return 12;     /* ���KEY1�Ƿ񱻰��� */
	 else if(KEY2.status == KEY_LONG) return 13;     /* ���KEY2�Ƿ񱻰��� */
	 else if(KEY3.status == KEY_LONG) return 14;     /* ���KEY2�Ƿ񱻰��� */
	
	 else if(KEY0.status == KEY_DOWN) return 1;      /* ���KEY0�Ƿ񱻰��� */
	 else if(KEY1.status == KEY_DOWN) return 2;      /* ���KEY1�Ƿ񱻰��� */
	 else if(KEY2.status == KEY_DOWN) return 3;      /* ���KEY2�Ƿ񱻰��� */
	 else if(KEY3.status == KEY_DOWN) return 4;      /* ���KEY2�Ƿ񱻰��� */
	 else return 0;                                  /* �Ƿ�û������ */
}