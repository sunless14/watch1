#include "appstep.h"

uint8_t ucMpuData;            /*���ڽ����ַ�*/
uint8_t mpuRecBuf[30] = {0};  /*���ڽ��ջ��� */
uint8_t mpuDataBuf[30] = {0}; /*��������ȷ�����ݰ�*/
int16_t mpuACC[3] = {0};      /*������ٶ�*/
static uint32_t peakOfValue;
static uint32_t valleyOfValue;
personInfo_t personInfo = {
	.height = 173,
	.weight = 150,
};

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
 
 *��������averageValue
 *�������ܣ���������ľ�ֵ���������ݶȻ���һ����Χ��
 *�βΣ�value[],����;lenth, ���鳤��
 *����ֵ��������ֵ
 
 ******************************************************************************/
 
 uint32_t averageValue(uint32_t value[], uint8_t lenth){
	 uint32_t average = 0;
	 uint8_t i = 0;
	 for(int i = 0 ; i < lenth; i++){
		 average += value[i];
	 }
	 average /= WAVE_NUM;
	 if(average >= 80000) average = 30000;
	 else if(average >= 60000) average = 20000;
	 else if(average >= 40000) average = 12000;
	 else if(average >= 20000) average = 8000;
	 else average = 5000;
	 return average;
 }
 
 /******************************************************************************
 
 *��������peakValleyThread
 *�������ܣ���¼������ĸ����壬���ȵĲ�ֵ����tempValue[]���飬�����㲨����ֵ
 *�βΣ�value ���壬���Ȳ�ֵ
 *����ֵ����������ֵ
 
 ******************************************************************************/
 
 uint32_t peakValleyThread(uint32_t value){
	 static uint8_t tempCount;
	 static uint32_t tempValue[4];
	 uint32_t tempThread = 0;
	 uint8_t i = 0;
	 if(tempCount < WAVE_NUM)			tempValue[tempCount++] = value;
	 else{
			tempThread = averageValue(tempValue, WAVE_NUM);
		  for(int i = 1; i < WAVE_NUM; i++){
				tempValue[i - 1] = tempValue[i];
			}
			tempValue[WAVE_NUM - 1] = value;
	 }
	 return tempThread;
 }
 
  /******************************************************************************
 
 *��������DetertorPeak
 *�������ܣ�������
 *�βΣ�newValue ��ǰ���ٶȾ���ֵ
        oldValue ��һ���ٶȾ���ֵ
 *����ֵ���Ƿ�Ϊ�����
 
 ******************************************************************************/
 
 uint8_t DetectorPeak(uint32_t newValue, uint32_t oldValue){
	 static uint8_t isDirectionUp, continueUpCount;
	 uint8_t continueUpCountLastPoint = 0, lastStatus = isDirectionUp;
	 if(newValue >= oldValue){
		 isDirectionUp = TRUE;
		 continueUpCount++;
	 }else{
		 continueUpCountLastPoint = continueUpCount;
		 continueUpCount = 0;
		 isDirectionUp = FALSE;
	 }
	 if((!isDirectionUp) && lastStatus && ( continueUpCountLastPoint >= 2 && oldValue >= PEEK_MIN_VALUE)){
		 peakOfValue = oldValue;
		 return TRUE;
	 }else if((!lastStatus) && isDirectionUp){
		 valleyOfValue = oldValue;
		 return FALSE;
	 }else
	   return FALSE;
 }
 
 /******************************************************************************
 
 *��������DetectorNewStep
 *�������ܣ��������£�����⵽���壬������ʱ������ֵ���������ж�Ϊһ��
            ��ֵ���£�������ʱ�����岨��֮�����waveDelta,��������ֵ����
 *�βΣ�gravityNew, ���ٶȵ�ǰ����ֵ
 *����ֵ��sportsInfo_tָ��
 
 ******************************************************************************/
 
 sportsInfo_t * DetectorNewStep(uint32_t gravityNew){
	 static uint32_t time_old;
	 static uint32_t stepTempCount,stepBy2Second;
	 float stepLenth = 0.6;
	 float walkSpeed, walkDistance, Calories;
	 uint32_t time_now, timeOfNow, timeOfLastPeak;
	 static uint32_t gravityOld;
	 static uint32_t timeOfPeak;
	 static uint32_t ThreadValue;
	 static sportsInfo_t sportsInfo;
	 if(gravityOld == 0){
			gravityOld = gravityNew;
	 }else{
			if(DetectorPeak(gravityNew, gravityOld)){
				timeOfLastPeak = timeOfPeak;
				time_now = timeOfNow = HAL_GetTick();
				
				if((timeOfNow - timeOfLastPeak >= 250) && (peakOfValue - valleyOfValue >= ThreadValue)){
					timeOfPeak = timeOfNow;
					ThreadValue = peakValleyThread(peakOfValue - valleyOfValue);
					
					stepBy2Second++;
					stepTempCount++;
					
					if((stepTempCount < 5) &&  (timeOfNow - timeOfLastPeak >= 3000)) stepTempCount = 0;
					else if((stepTempCount >= 5) &&  (timeOfNow - timeOfLastPeak <= 3000)){
						sportsInfo.stepCount += stepTempCount;
						stepTempCount = 0;
					}
					if((time_now - time_old) >= 2000){
						walkSpeed = stepBy2Second * stepLenth;
						walkDistance = stepBy2Second * stepLenth;
            Calories = 4.5f * walkSpeed * (	personInfo.weight / 2) / 1800;
            sportsInfo.calories += Calories;
            sportsInfo.distance += walkDistance;
            time_old = time_now;
            stepBy2Second = 0;						
					}
				}
			}
	 }
	 gravityOld = gravityNew;
	 return &sportsInfo;
 }

 
 /******************************************************************************
 
 *��������calculateStep
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
 