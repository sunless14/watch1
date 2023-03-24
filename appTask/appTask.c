 #include "appTask.h"
	
static TaskHandle_t led0TaskHandle = NULL;       /*led0任务句柄*/
TaskHandle_t led1TaskHandle = NULL;              /*led1任务句柄*/
static TaskHandle_t printTaskHandle = NULL;      /*串口守护任务句柄*/
static TaskHandle_t keyTaskHandle = NULL;        /*按键扫描任务句柄*/
static TaskHandle_t mainMenuTaskHandle = NULL;   /*时间显示任务句柄*/
static TaskHandle_t timeSetTaskHandle = NULL;    /*时间显示任务句柄*/


static RTC_TimeTypeDef rtcTime;          /*当前时间*/
static RTC_DateTypeDef rtcDate;          /*当前日期*/
static char cDateTime[40];
static char pcToPrint[80];
xQueueHandle xQueuePrint;
xQueueHandle queueKeyHandle;             /*按键处理队列*/



 /******************************************************************************
 
 *函数名：showMainMenu
 *函数功能：任务函数，主界面刷新，读取时间并刷新显示
 *形参：
 *返回值：
 *任务优先级：
 
 ******************************************************************************/
static void MainMenuTask(void * pvParameters){
	 uint8_t keyValue;
	 ahtData_t ahtData;
	 char  week[7][7] = {"Mon","Tues","Wed", "Thurs", "Fri", "Sat", "Sun"};
	 while(1){
		 HAL_RTC_GetTime(&hrtc, &rtcTime, RTC_FORMAT_BIN);
		 HAL_RTC_GetDate(&hrtc, &rtcDate, RTC_FORMAT_BIN);
		 
		 getAHT10(&ahtData);
		 
		 LCD_Clear(WHITE);
		 LCD_ShowString(16,0,16,"beijing time",1);
		 sprintf(cDateTime,"20%02d-%02d-%02d %s",rtcDate.Year, rtcDate.Month, rtcDate.Date, week[rtcDate.WeekDay]);
		 LCD_ShowString(4, 16, 16,cDateTime, 1);
		 sprintf(cDateTime,"%02d:%02d:%02d",rtcTime.Hours, rtcTime.Minutes, rtcTime.Seconds);
		 LCD_ShowString(32, 32, 16,cDateTime, 1);
		 sprintf(cDateTime,"%3.1f",ahtData.temp);
		 LCD_ShowString(16, 48, 16,"temp", 1);
		 LCD_ShowString(64, 48, 16,cDateTime, 1);
		 sprintf(cDateTime,"%2d%%",(uint8_t)ahtData.humi);
		 LCD_ShowString(16, 64, 16,"humi", 1);
		 LCD_ShowString(64, 64, 16,cDateTime, 1);
		 vTaskDelay(pdMS_TO_TICKS(500));                     /*延时500MS*/
	 }
 }

 /******************************************************************************
 
 *函数名：timeSetTask
 *函数功能：任务函数，主界面刷新，读取时间并刷新显示
 *形参：
 *返回值：
 *任务优先级：
 
 ******************************************************************************/
static void timeSetTask(void * pvParameters){
	 static uint8_t timeSetPosi = 0;
	 uint8_t keyValue,flashFlag = 0;
	 int8_t dataTime[7] = {0};
	 ahtData_t ahtData;
	 char  week[7][7] = {"Mon","Tues","Wed", "Thurs", "Fri", "Sat", "Sun"};
	 while(1){
		 HAL_RTC_GetTime(&hrtc, &rtcTime, RTC_FORMAT_BIN);
		 HAL_RTC_GetDate(&hrtc, &rtcDate, RTC_FORMAT_BIN);
		 
		 getAHT10(&ahtData);
		 
		 LCD_Clear(WHITE);
		 LCD_ShowString(16,0,16,"beijing time",1);
		 sprintf(cDateTime,"20%02d-%02d-%02d %s",rtcDate.Year, rtcDate.Month, rtcDate.Date, week[rtcDate.WeekDay]);
		 LCD_ShowString(4, 16, 16,cDateTime, 1);
		 sprintf(cDateTime,"%02d:%02d:%02d",rtcTime.Hours, rtcTime.Minutes, rtcTime.Seconds);
		 LCD_ShowString(32, 32, 16,cDateTime, 1);
		 sprintf(cDateTime,"%3.1f",ahtData.temp);
		 LCD_ShowString(16, 48, 16,"temp", 1);
		 LCD_ShowString(64, 48, 16,cDateTime, 1);
		 sprintf(cDateTime,"%2d%%",(uint8_t)ahtData.humi);
		 LCD_ShowString(16, 64, 16,"humi", 1);
		 LCD_ShowString(64, 64, 16,cDateTime, 1);
		 vTaskDelay(pdMS_TO_TICKS(500));                     /*延时500MS*/
	 }
 }

  
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
	 queueKeyHandle = xQueueCreate(2,sizeof(uint8_t));
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
		xTaskCreate(MainMenuTask,      /*任务函数*/
									"MainMenu",      /*任务名*/
									512,             /*任务堆栈大小，单位为word，128word = 4B*/
									NULL,            /*任务参数*/
									2,               /*任务优先级，越小越高*/
									&mainMenuTaskHandle); /*任务句柄*/
								
			taskEXIT_CRITICAL();
	 }
 }

