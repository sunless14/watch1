 #include "appTask.h"


    /* ����KEY0 */
	   _KEY_OBJ KEY0 =
   {
	   .status           = KEY_UP,          /* �����ĳ�ʼ״̬����̧�� */
	   .effective_Level  = LOW,             /* ����ԭ��ͼ�������ǵ͵�ƽ��Ч��NPN�� */
	   .filter_Count     = 2,               /* �˲�����2�����2msɨ��һ�Σ���ô�˲�ʱ�����4ms */
	   .GPIOx            = key0_GPIO_Port,  /* KEY1��GPIO Port */
	   .GPIO_Pin         = key0_Pin,        /* KEY1��GPIO Pin */
	   .init             = key_Init,        /* ���캯�� */
   }; 
	    /* ����KEY0 */
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
	   .GPIOx            = key2_GPIO_Port,  /* KEY1��GPIO Port */
	   .GPIO_Pin         = key2_Pin,        /* KEY1��GPIO Pin */
	   .init             = key_Init,        /* ���캯�� */
   };
uint8_t KeyScan(void){
   KEY0.init(&KEY0);                      /* ��ʼ��KEY0 */ 
   KEY1.init(&KEY1);	                    /* ��ʼ��KEY1 */ 
   KEY2.init(&KEY2);	                    /* ��ʼ��KEY2 */ 	
	 KEY0.runtime(&KEY0);                   /* ɨ��KEY0 */ 
   KEY1.runtime(&KEY1);	                  /* ɨ��KEY1 */
	 KEY2.runtime(&KEY2);	                  /* ɨ��KEY2 */
	 
	 if(KEY0.status == KEY_DOWN) return 1;           /* ���KEY0�Ƿ񱻰��� */
	 else if(KEY1.status == KEY_DOWN) return 2;      /* ���KEY1�Ƿ񱻰��� */
	 else if(KEY2.status == KEY_DOWN) return 3;      /* ���KEY2�Ƿ񱻰��� */
	 else return 0;                                       /* �Ƿ�û������ */
}


	
static TaskHandle_t led0TaskHandle = NULL;  /*led0������*/
TaskHandle_t led1TaskHandle = NULL;  /*led1������*/
static TaskHandle_t printTaskHandle = NULL; /*�����ػ�������*/
static TaskHandle_t keyTaskHandle = NULL;   /*����ɨ��������*/



static char pcToPrint[80];
xQueueHandle xQueuePrint;



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
		 vTaskDelay(pdMS_TO_TICKS(100));                     /*��ʱ100MS*/
	 }
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
			taskEXIT_CRITICAL();
	 }
 }

