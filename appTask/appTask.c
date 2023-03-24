 #include "appTask.h"


    /* 按键KEY0 */
	   _KEY_OBJ KEY0 =
   {
	   .status           = KEY_UP,          /* 按键的初始状态都是抬起 */
	   .effective_Level  = LOW,             /* 根据原理图，按键是低电平有效（NPN） */
	   .filter_Count     = 2,               /* 滤波次数2，如果2ms扫描一次，那么滤波时间等于4ms */
	   .GPIOx            = key0_GPIO_Port,  /* KEY1的GPIO Port */
	   .GPIO_Pin         = key0_Pin,        /* KEY1的GPIO Pin */
	   .init             = key_Init,        /* 构造函数 */
   }; 
	    /* 按键KEY0 */
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
	   .GPIOx            = key2_GPIO_Port,  /* KEY1的GPIO Port */
	   .GPIO_Pin         = key2_Pin,        /* KEY1的GPIO Pin */
	   .init             = key_Init,        /* 构造函数 */
   };
uint8_t KeyScan(void){
   KEY0.init(&KEY0);                      /* 初始化KEY0 */ 
   KEY1.init(&KEY1);	                    /* 初始化KEY1 */ 
   KEY2.init(&KEY2);	                    /* 初始化KEY2 */ 	
	 KEY0.runtime(&KEY0);                   /* 扫描KEY0 */ 
   KEY1.runtime(&KEY1);	                  /* 扫描KEY1 */
	 KEY2.runtime(&KEY2);	                  /* 扫描KEY2 */
	 
	 if(KEY0.status == KEY_DOWN) return 1;           /* 检测KEY0是否被按下 */
	 else if(KEY1.status == KEY_DOWN) return 2;      /* 检测KEY1是否被按下 */
	 else if(KEY2.status == KEY_DOWN) return 3;      /* 检测KEY2是否被按下 */
	 else return 0;                                       /* 是否都没被按下 */
}


	
static TaskHandle_t led0TaskHandle = NULL;  /*led0任务句柄*/
TaskHandle_t led1TaskHandle = NULL;  /*led1任务句柄*/
static TaskHandle_t printTaskHandle = NULL; /*串口守护任务句柄*/
static TaskHandle_t keyTaskHandle = NULL;   /*按键扫描任务句柄*/



static char pcToPrint[80];
xQueueHandle xQueuePrint;



 /******************************************************************************
 
 *函数名：led0task
 *函数功能：使led0闪烁，指示程序是否运行
 *形参：
 *返回值：
  *任务优先级：3
 
 ******************************************************************************/
 static void led0Task(void * pvParameters){
	 while(1){
		 HAL_GPIO_TogglePin(led0_GPIO_Port,led0_Pin);        /*led0闪烁*/
		 
		 vTaskDelay(pdMS_TO_TICKS(500));                     /*延时500MS*/
	 }
 }

  /******************************************************************************
 
 *函数名：led1task
 *函数功能：检测事件标志位，bit2、bit1和bit0同时置1时才点亮led2，并输出信息到串口
 *形参：
 *返回值：
  *任务优先级：3
 
 ******************************************************************************/
 static void led1Task(void * pvParameters){
	 uint32_t uNotifyValue;
	 uint8_t eveFlag1 = 0, eveFlag2 = 0, eveFlag3 = 0; 
	 while(1){
		 HAL_GPIO_WritePin(led2_GPIO_Port, led2_Pin, GPIO_PIN_SET);   /*熄灭led2*/
		 xTaskNotifyWait(0x00, 0xffffffff, &uNotifyValue, portMAX_DELAY);
		 if(uNotifyValue & 0x01) eveFlag1 = 1;
		 if(uNotifyValue & 0x02) eveFlag2 = 1;
		 if(uNotifyValue & 0x04) eveFlag3 = 1;
		 
		 if(eveFlag1 && eveFlag2 && eveFlag3){
			 eveFlag1 = eveFlag2 = eveFlag3 = 0;
		   sprintf(pcToPrint,"key0 key1 tim 2 \r\n\r\n");
		   xQueueSendToBack(xQueuePrint, pcToPrint, 0);
		 }
		 
		 HAL_GPIO_WritePin(led2_GPIO_Port, led2_Pin, GPIO_PIN_RESET);  /*点亮led2*/
		 vTaskDelay(pdMS_TO_TICKS(1000));                              /*延时1S*/		 	                         
	 }
 }
 
  /******************************************************************************
 
 *函数名：keyTask
 *函数功能：按键扫描任务，根据键值执行相应操作
 *形参：
 *返回值：
 *任务优先级：4
 
 ******************************************************************************/
 static void keyTask(void * pvParameters){
	 uint8_t keyValue;
	 extern TIM_HandleTypeDef htim2;
	 while(1){
		 keyValue = KeyScan();
		 if(keyValue == 1){
			 sprintf(pcToPrint,"key0, bit0...\r\n\r\n");
			 xQueueSendToBack(xQueuePrint, pcToPrint, 0);
			 xTaskNotify(led1TaskHandle, 0x01, eSetBits);
		 }else if(keyValue == 2){
			 sprintf(pcToPrint,"key1, bit1...\r\n\r\n");
			 xQueueSendToBack(xQueuePrint, pcToPrint, 0);
			 xTaskNotify(led1TaskHandle, 0x02, eSetBits);
		 }else if(keyValue == 3){
			 HAL_TIM_Base_Start_IT(&htim2);
			 sprintf(pcToPrint,"key2,start tim2...\r\n\r\n");
			 xQueueSendToBack(xQueuePrint, pcToPrint, 0);    		 
	   }
		 vTaskDelay(pdMS_TO_TICKS(100));                     /*延时100MS*/
	 }
 }

 /******************************************************************************
 
 *函数名：printTask
 *函数功能：串口守护任务
 *形参：
 *返回值：
 *任务优先级：3
 
 ******************************************************************************/
 static void printTask(void * pvParameters){                                   
	 char pcToWrite[80];
	 while(1){
		 xQueueReceive(xQueuePrint, pcToWrite, portMAX_DELAY);
		 printf("%s", pcToWrite);
	 }
 }
 
 /******************************************************************************
 
 *函数名：appStartTask
 *函数功能：开始任务函数，用于创建其它任务
 *形参：
 *返回值：
 
 ******************************************************************************/
void appStartTask(void){
	 xQueuePrint =  xQueueCreate(2, sizeof(pcToPrint));
	 if(xQueuePrint != NULL){
			taskENTER_CRITICAL();
			xTaskCreate(led0Task,        /*任务函数*/
									"led0Task",      /*任务名*/
									128,             /*任务堆栈大小，单位为word，128word = 4B*/
									NULL,            /*任务参数*/
									3,               /*任务优先级，越小越高*/
									&led0TaskHandle);/*任务句柄*/
	 
			xTaskCreate(led1Task,        /*任务函数*/
									"led1Task",      /*任务名*/
									128,             /*任务堆栈大小，单位为word，128word = 4B*/
									NULL,            /*任务参数*/
									3,               /*任务优先级，越小越高*/
									&led1TaskHandle);/*任务句柄*/
			xTaskCreate(printTask,       /*任务函数*/
									"printTask",     /*任务名*/
									128,             /*任务堆栈大小，单位为word，128word = 4B*/
									NULL,            /*任务参数*/
									3,               /*任务优先级，越小越高*/
									&printTaskHandle);/*任务句柄*/
		 xTaskCreate(keyTask,          /*任务函数*/
									"keyTask",       /*任务名*/
									128,             /*任务堆栈大小，单位为word，128word = 4B*/
									NULL,            /*任务参数*/
									4,               /*任务优先级，越小越高*/
									&keyTaskHandle); /*任务句柄*/
			taskEXIT_CRITICAL();
	 }
 }

