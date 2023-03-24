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
