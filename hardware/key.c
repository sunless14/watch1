#include "key.h"


/*************************************************************************************************************
* 函数名: key_Init

* 功能:   初始化按键对象

* 输入：*key: 按键对象的地址

* 输出：无

* 返回：无

note:
      1.必须初始化后，按键的对象才能被正常使用。
 */
void key_Init(_KEY_OBJ *key)
{
	ringbuffer_init(&key->FIFO,key->FIFO_Buffer,KEY_BUFFER_SIZE);  /* 初始化FIFO */
	
	/* 函数指针 */
	key->runtime = key_Runtime;
}

/*************************************************************************************************************
* 函数名: key_Scan

* 功能: 通过读取GPIO的电平，获取按键的状态

* 输入：*key: 按键对象的地址

* 输出：无

* 返回：无

note:
 */
static void key_Scan(_KEY_OBJ *key)
{
	GPIO_PinState level = GPIO_PIN_RESET;
	_KEY_STATUS status = KEY_UP;
	
	level = HAL_GPIO_ReadPin(key->GPIOx,key->GPIO_Pin);  /* 读取GPIO的电平 */
	
	/* 判断按键的有效极性 */
	/* 按键是低电平有效 */
	if(LOW == key->effective_Level)
	{
		/* 看看GPIO是否检查到低电平 */
		if(GPIO_PIN_RESET == level)
		{
			status = KEY_DOWN;	/* 按键被按下 */
		}
		else
		{
			status = KEY_UP;	/* 按键抬起 */
		}
	}
    /* 按键是高电平有效 */	
	else
	{
		/* 看看GPIO是否检测到高电平 */
		if(GPIO_PIN_SET == level)
		{
			status = KEY_DOWN;  /* 按键被按下 */
		}
		else
		{
			status = KEY_UP;    /* 按键抬起 */
		}
	}
	
	ringbuffer_in(&key->FIFO,(uint8_t *)&status,1);  /* 按键状态放入FIFO */
}

/*************************************************************************************************************
* 函数名: key_Runtime

* 功能: 被定时器调用，实现按键软件滤波(状态机原理）

* 输入：*key: 按键对象的地址

* 输出：无

* 返回：无

note:
	 1、软件滤波时间 = 定时器的周期T * 按键对象成员filter_Count
		比如，定时器的周期T = 5ms ，成员filter_Count = 2 ,软件滤波时间 = 5ms * 2 = 10ms
 */
void key_Runtime(_KEY_OBJ *key)
{
	_KEY_STATUS status = KEY_UP;
	
	key_Scan(key);  /* 获取按键的状态，放入FIFO */
	
	ringbuffer_out(&key->FIFO,(uint8_t *)&status,1);  /* 从FIFO里取出1个按键状态 */
	
	/* 状态机 */
	switch(key->status)
	{
		/* 状态机的状态一（按钮抬起） */
		case KEY_UP:
			if(KEY_DOWN == status)  
			{
				key->runtime_Count++;  /* 按键被按下，累加一个周期 */
			}
			else
			{
				/* 复位 */
				key->runtime_Count = 0; 
				key->status = KEY_UP;  
			}
			
			/* 周期数 = 设定的周期数 */
			if(key->filter_Count == key->runtime_Count)
			{
				key->status = KEY_DOWN;  /* 进入下一个状态机阶段 */
			}				
			break;
			
		/* 状态机的状态二（按钮被按下） */
		case KEY_DOWN:
			if(KEY_DOWN == status)
			{
				key->runtime_Count++;  /* 按键被按下，累加一个周期 */ 
			}
			else
			{
				/* 复位 */
				key->runtime_Count = 0;
				key->status = KEY_UP;
			}
			
			/* 周期数 = 设定的周期数2倍 */
			if(key->filter_Count * 2 == key->runtime_Count)
			{
				key->status = KEY_LONG;  /* 进入下一个状态机阶段 */
			}
			break;
			
		/* 状态机的状态三（按钮被长按） */
		case KEY_LONG:
			if(KEY_UP == status)
			{
			    /* 复位 */
				key->runtime_Count = 0;
				key->status = KEY_UP;
			}
			break;
		default:
			break;
	}

}


    /* 按键KEY0 */
	   _KEY_OBJ KEY0 =
   {
	   .status           = KEY_UP,          /* 按键的初始状态都是抬起 */
	   .effective_Level  = LOW,             /* 根据原理图，按键是低电平有效（NPN） */
	   .filter_Count     = 2,               /* 滤波次数2，如果2ms扫描一次，那么滤波时间等于4ms */
	   .GPIOx            = key0_GPIO_Port,  /* KEY0的GPIO Port */
	   .GPIO_Pin         = key0_Pin,        /* KEY0的GPIO Pin */
	   .init             = key_Init,        /* 构造函数 */
   }; 
	  _KEY_OBJ KEY1 =
   {
	   .status           = KEY_UP,          /* 按键的初始状态都是抬起 */
	   .effective_Level  = LOW,             /* 根据原理图，按键是低电平有效（NPN） */
	   .filter_Count     = 2,               /* 滤波次数2，如果2ms扫描一次，那么滤波时间等于4ms */
	   .GPIOx            = key1_GPIO_Port,  /* KEY1的GPIO Port */
	   .GPIO_Pin         = key1_Pin,        /* KEY1的GPIO Pin */
	   .init             = key_Init,        /* 构造函数 */
   };
	 _KEY_OBJ KEY2 =
   {
	   .status           = KEY_UP,          /* 按键的初始状态都是抬起 */
	   .effective_Level  = LOW,             /* 根据原理图，按键是低电平有效（NPN） */
	   .filter_Count     = 2,               /* 滤波次数2，如果2ms扫描一次，那么滤波时间等于4ms */
	   .GPIOx            = key2_GPIO_Port,  /* KEY2的GPIO Port */
	   .GPIO_Pin         = key2_Pin,        /* KEY2的GPIO Pin */
	   .init             = key_Init,        /* 构造函数 */
   };
	 _KEY_OBJ KEY3 =
   {
	   .status           = KEY_UP,          /* 按键的初始状态都是抬起 */
	   .effective_Level  = LOW,             /* 根据原理图，按键是低电平有效（NPN） */
	   .filter_Count     = 2,               /* 滤波次数2，如果2ms扫描一次，那么滤波时间等于4ms */
	   .GPIOx            = key3_GPIO_Port,  /* KEY3的GPIO Port */
	   .GPIO_Pin         = key3_Pin,        /* KEY3的GPIO Pin */
	   .init             = key_Init,        /* 构造函数 */
   };

void key_init(void){
	 KEY0.init(&KEY0);                      /* 初始化KEY0 */ 
   KEY1.init(&KEY1);	                    /* 初始化KEY1 */ 
   KEY2.init(&KEY2);	                    /* 初始化KEY2 */
	 KEY3.init(&KEY3);	                    /* 初始化KEY3 */
}

uint8_t KeyScan(void){
	 KEY0.runtime(&KEY0);                   /* 扫描KEY0 */ 
   KEY1.runtime(&KEY1);	                  /* 扫描KEY1 */
	 KEY2.runtime(&KEY2);	                  /* 扫描KEY2 */
	 KEY3.runtime(&KEY3);	                  /* 扫描KEY3 */
	 
	 if(KEY0.status == KEY_LONG) return 11;          /* 检测KEY0是否被按下 */
	 else if(KEY1.status == KEY_LONG) return 12;     /* 检测KEY1是否被按下 */
	 else if(KEY2.status == KEY_LONG) return 13;     /* 检测KEY2是否被按下 */
	 else if(KEY3.status == KEY_LONG) return 14;     /* 检测KEY2是否被按下 */
	
	 else if(KEY0.status == KEY_DOWN) return 1;      /* 检测KEY0是否被按下 */
	 else if(KEY1.status == KEY_DOWN) return 2;      /* 检测KEY1是否被按下 */
	 else if(KEY2.status == KEY_DOWN) return 3;      /* 检测KEY2是否被按下 */
	 else if(KEY3.status == KEY_DOWN) return 4;      /* 检测KEY2是否被按下 */
	 else return 0;                                  /* 是否都没被按下 */
}