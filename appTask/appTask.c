 #include "appTask.h"
	
static TaskHandle_t led0TaskHandle = NULL;       /*led0������*/
TaskHandle_t led1TaskHandle = NULL;              /*led1������*/
static TaskHandle_t printTaskHandle = NULL;      /*�����ػ�������*/
static TaskHandle_t keyTaskHandle = NULL;        /*����ɨ��������*/
static TaskHandle_t mainMenuTaskHandle = NULL;   /*ʱ����ʾ������*/
static TaskHandle_t timeSetTaskHandle = NULL;    /*ʱ����ʾ������*/


static RTC_TimeTypeDef rtcTime;          /*��ǰʱ��*/
static RTC_DateTypeDef rtcDate;          /*��ǰ����*/
static char cDateTime[40];
static char pcToPrint[80];
xQueueHandle xQueuePrint;
xQueueHandle queueKeyHandle;             /*�����������*/



 /******************************************************************************
 
 *��������showMainMenu
 *�������ܣ���������������ˢ�£���ȡʱ�䲢ˢ����ʾ
 *�βΣ�
 *����ֵ��
 *�������ȼ���
 
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
		 vTaskDelay(pdMS_TO_TICKS(500));                     /*��ʱ500MS*/
	 }
 }

 /******************************************************************************
 
 *��������timeSetTask
 *�������ܣ���������������ˢ�£���ȡʱ�䲢ˢ����ʾ
 *�βΣ�
 *����ֵ��
 *�������ȼ���
 
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
		 vTaskDelay(pdMS_TO_TICKS(500));                     /*��ʱ500MS*/
	 }
 }

  
 /******************************************************************************
 
 *��������led0task
 *�������ܣ�ʹled0��˸��ָʾ�����Ƿ�����
 *�βΣ�
 *����ֵ��
  *�������ȼ���3
 
 ******************************************************************************/
 static void led0Task(void * pvParameters){
	 while(1){
		 HAL_GPIO_TogglePin(led0_GPIO_Port,led0_Pin);        /*led0��˸*/
		 
		 vTaskDelay(pdMS_TO_TICKS(500));                     /*��ʱ500MS*/
	 }
 }

  /******************************************************************************
 
 *��������led1task
 *�������ܣ�����¼���־λ��bit2��bit1��bit0ͬʱ��1ʱ�ŵ���led2���������Ϣ������
 *�βΣ�
 *����ֵ��
  *�������ȼ���3
 
 ******************************************************************************/
 static void led1Task(void * pvParameters){
	 uint32_t uNotifyValue;
	 uint8_t eveFlag1 = 0, eveFlag2 = 0, eveFlag3 = 0; 
	 while(1){
		 HAL_GPIO_WritePin(led2_GPIO_Port, led2_Pin, GPIO_PIN_SET);   /*Ϩ��led2*/
		 xTaskNotifyWait(0x00, 0xffffffff, &uNotifyValue, portMAX_DELAY);
		 if(uNotifyValue & 0x01) eveFlag1 = 1;
		 if(uNotifyValue & 0x02) eveFlag2 = 1;
		 if(uNotifyValue & 0x04) eveFlag3 = 1;
		 
		 if(eveFlag1 && eveFlag2 && eveFlag3){
			 eveFlag1 = eveFlag2 = eveFlag3 = 0;
		   sprintf(pcToPrint,"key0 key1 tim 2 \r\n\r\n");
		   xQueueSendToBack(xQueuePrint, pcToPrint, 0);
		 }
		 
		 HAL_GPIO_WritePin(led2_GPIO_Port, led2_Pin, GPIO_PIN_RESET);  /*����led2*/
		 vTaskDelay(pdMS_TO_TICKS(1000));                              /*��ʱ1S*/		 	                         
	 }
 }
 
  /******************************************************************************
 
 *��������keyTask
 *�������ܣ�����ɨ�����񣬸��ݼ�ִֵ����Ӧ����
 *�βΣ�
 *����ֵ��
 *�������ȼ���4
 
 ******************************************************************************/
 static void keyTask(void * pvParameters){
	 uint8_t keyValue;
	 extern TIM_HandleTypeDef htim2;
 }

 /******************************************************************************
 
 *��������printTask
 *�������ܣ������ػ�����
 *�βΣ�
 *����ֵ��
 *�������ȼ���3
 
 ******************************************************************************/
 static void printTask(void * pvParameters){                                   
	 char pcToWrite[80];
	 while(1){
		 xQueueReceive(xQueuePrint, pcToWrite, portMAX_DELAY);
		 printf("%s", pcToWrite);
	 }
 }
 
 /******************************************************************************
 
 *��������appStartTask
 *�������ܣ���ʼ�����������ڴ�����������
 *�βΣ�
 *����ֵ��
 
 ******************************************************************************/
void appStartTask(void){
	 xQueuePrint =  xQueueCreate(2, sizeof(pcToPrint));
	 queueKeyHandle = xQueueCreate(2,sizeof(uint8_t));
	 if(xQueuePrint != NULL){
			taskENTER_CRITICAL();
			xTaskCreate(led0Task,        /*������*/
									"led0Task",      /*������*/
									128,             /*�����ջ��С����λΪword��128word = 4B*/
									NULL,            /*�������*/
									3,               /*�������ȼ���ԽСԽ��*/
									&led0TaskHandle);/*������*/
			xTaskCreate(led1Task,        /*������*/
									"led1Task",      /*������*/
									128,             /*�����ջ��С����λΪword��128word = 4B*/
									NULL,            /*�������*/
									3,               /*�������ȼ���ԽСԽ��*/
									&led1TaskHandle);/*������*/
			xTaskCreate(printTask,       /*������*/
									"printTask",     /*������*/
									128,             /*�����ջ��С����λΪword��128word = 4B*/
									NULL,            /*�������*/
									3,               /*�������ȼ���ԽСԽ��*/
									&printTaskHandle);/*������*/
		 xTaskCreate(keyTask,          /*������*/
									"keyTask",       /*������*/
									128,             /*�����ջ��С����λΪword��128word = 4B*/
									NULL,            /*�������*/
									4,               /*�������ȼ���ԽСԽ��*/
									&keyTaskHandle); /*������*/
		xTaskCreate(MainMenuTask,      /*������*/
									"MainMenu",      /*������*/
									512,             /*�����ջ��С����λΪword��128word = 4B*/
									NULL,            /*�������*/
									2,               /*�������ȼ���ԽСԽ��*/
									&mainMenuTaskHandle); /*������*/
								
			taskEXIT_CRITICAL();
	 }
 }

