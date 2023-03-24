#include "appstep.h"

uint8_t ucMpuData;            /*串口接收字符*/
uint8_t mpuRecBuf[30] = {0};  /*串口接收缓存 */
uint8_t mpuDataBuf[30] = {0}; /*经检验正确的数据包*/
int16_t mpuACC[3] = {0};      /*三轴加速度*/
static uint32_t peakOfValue;
static uint32_t valleyOfValue;
personInfo_t personInfo = {
	.height = 173,
	.weight = 150,
};

 /******************************************************************************
 
 *函数名：HAL_UART_RxCpltCallback
 *函数功能：串口中断完成回调函数
 *形参：*huart,用于标记引起回调的串口句柄
 *返回值：
 
 ******************************************************************************/
 void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	 static uint8_t recBuf[30] = {0}, i = 0;
	 if(huart->Instance == UART4){
		 /*数据帧：0x5a,0x5a,type,lenth,data...(lenth),checksum*/
		 recBuf[i++] = ucMpuData;
		 //printf("%d\n",ucMpuData);      /*测试*/
		 if(recBuf[0] != 0x5a) i = 0;     /*判断帧头*/
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
 
 *函数名：mpusumCheck
 *函数功能：对接受到的数据进行累加和校验，若正确则存储数据
 *形参：*data,收到的数据缓冲区首地址
 *返回值：正确返回1，错误 0
 
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
 
 *函数名：averageValue
 *函数功能：计算数组的均值，将波峰梯度化在一个范围内
 *形参：value[],数组;lenth, 数组长度
 *返回值：波峰阈值
 
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
 
 *函数名：peakValleyThread
 *函数功能：记录最近的四个波峰，波谷的差值存入tempValue[]数组，并计算波峰阈值
 *形参：value 波峰，波谷差值
 *返回值：波峰检测阈值
 
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
 
 *函数名：DetertorPeak
 *函数功能：波峰检测
 *形参：newValue 当前加速度均方值
        oldValue 上一加速度均方值
 *返回值：是否为波峰点
 
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
 
 *函数名：DetectorNewStep
 *函数功能：步伐更新，若检测到波峰，并符合时间差和阈值条件，则判定为一步
            阈值更新，若符合时间差，波峰波谷之差大于waveDelta,则纳入阈值计算
 *形参：gravityNew, 加速度当前均方值
 *返回值：sportsInfo_t指针
 
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
 
 *函数名：calculateStep
 *函数功能：计算加速度的均方值，并通过此值检测波峰，实现计步
 *形参：*pAccValue, 三轴加速度当前值指针，原始值，为转化成以g为单位
 *返回值：sportsInfo_t指针
 
 ******************************************************************************/
 
 sportsInfo_t * calculateStep(int16_t *pAccValue){
	 uint32_t gravityNew = 0;
	 gravityNew = sqrt(pAccValue[0] * pAccValue[0] + pAccValue[1] * pAccValue[1] + pAccValue[2] * pAccValue[2]);
	 return DetectorNewStep(gravityNew);
 }

 
/******************************************************************************
 
 *函数名：useAccToStep(void)
 *函数功能：从串口接收数据中计算加速度值，并记步
 *形参：
 *返回值：sportsInfo_t指针
 
 ******************************************************************************/
 
 sportsInfo_t *useAccToStep(void){
	 HAL_UART_Receive_IT(&huart4,&ucMpuData,1);
	 if(mpuSumCheck(mpuDataBuf)){
		 /*加速度数据*/
		 //printf("notion: %d\n",mpuDataBuf[2]);
		 if(mpuDataBuf[2] & 0x01){
			 mpuACC[0] = (mpuDataBuf[4]<<8) | mpuDataBuf[5];
			 mpuACC[1] = (mpuDataBuf[6]<<8) | mpuDataBuf[7];
			 mpuACC[2] = (mpuDataBuf[8]<<8) | mpuDataBuf[9];
		 }  
	 }
	 return calculateStep(mpuACC);
 }
 