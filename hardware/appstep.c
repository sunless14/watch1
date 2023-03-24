#include "appstep.h"

uint8_t ucMpuData;            /*���ڽ����ַ�*/
uint8_t mpuRecBuf[30] = {0};  /*���ڽ��ջ��� */
uint8_t mpuDataBuf[30] = {0}; /*��������ȷ�����ݰ�*/
int16_t mpuACC[3] = {0};      /*������ٶ�*/

 /******************************************************************************
 
 *��������HAL_UART_RxCpltCallback
 *�������ܣ������ж���ɻص�����
 *�βΣ�*huart,���ڱ������ص��Ĵ��ھ��
 *����ֵ��
 
 ******************************************************************************/
 void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	 static uint8_t recBuf[30] = {0}, i = 0;
	 if(huart->Instance == UART4){
		 /*����֡��0x5a,0x5a,type,lenth,data...(lenth),checksum*/
		 recBuf[i++] = ucMpuData;
		 //printf("%d\n",ucMpuData);      /*����*/
		 if(recBuf[0] != 0x5a) i = 0;     /*�ж�֡ͷ*/
		 if((i == 2) && (recBuf[1] != 0x5a)) i = 0;
		 if(i > 29) i = 0;
		 
		 if(i>4){
			 if( i == recBuf[3] + 5){
				 memcpy(mpuRecBuf, recBuf, i);
				 i = 0;
			 }
		 }
	 }
	 HAL_UART_Receive_IT(&huart4, &ucMpuData,1);
 }
 
  /******************************************************************************
 
 *��������mpusumCheck
 *�������ܣ��Խ��ܵ������ݽ����ۼӺ�У�飬����ȷ��洢����
 *�βΣ�*data,�յ������ݻ������׵�ַ
 *����ֵ����ȷ����1������ 0
 
 ******************************************************************************/
 uint8_t mpuSumCheck(uint8_t *data){
	 uint8_t sum = 0, number = 0, i = 0;
	 number = mpuRecBuf[3] + 5;
	 if(number > 30) return 0;
	 for(i = 0; i < number - 1; i++){
		 sum += mpuRecBuf[i];
	 }
	 if(sum == mpuRecBuf[number - 1]){
		 memcpy(data, mpuRecBuf,number);
		 return TRUE;
	 }else return FALSE;
 }
 
 /******************************************************************************
 
 *��������calculateStep(void)
 *�������ܣ�������ٶȵľ���ֵ����ͨ����ֵ��Ⲩ�壬ʵ�ּƲ�
 *�βΣ�*pAccValue, ������ٶȵ�ǰֵָ�룬ԭʼֵ��Ϊת������gΪ��λ
 *����ֵ��sportsInfo_tָ��
 
 ******************************************************************************/
 sportsInfo_t * calculateStep(int16_t *pAccValue){
	 uint32_t gravityNew = 0;
	 gravityNew = sqrt(pAccValue[0] * pAccValue[0] + pAccValue[1] * pAccValue[1] + pAccValue[2] * pAccValue[2]);
	 return DetectorNewStep(gravityNew);
 } 
/******************************************************************************
 
 *��������useAccToStep(void)
 *�������ܣ��Ӵ��ڽ��������м�����ٶ�ֵ�����ǲ�
 *�βΣ�
 *����ֵ��sportsInfo_tָ��
 
 ******************************************************************************/
 sportsInfo_t *useAccToStep(void){
	 HAL_UART_Receive_IT(&huart4,&ucMpuData,1);
	 if(mpuSumCheck(mpuDataBuf)){
		 /*���ٶ�����*/
		 //printf("notion: %d\n",mpuDataBuf[2]);
		 if(mpuDataBuf[2] & 0x01){
			 mpuACC[0] = (mpuDataBuf[4]<<8) | mpuDataBuf[5];
			 mpuACC[1] = (mpuDataBuf[6]<<8) | mpuDataBuf[7];
			 mpuACC[2] = (mpuDataBuf[8]<<8) | mpuDataBuf[9];
		 }  
	 }
	 return calculateStep(mpuACC);
 }
 