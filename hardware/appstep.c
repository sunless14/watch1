#include "appstep.h"

uint8_t ucMpuData;            /*串口接收字符*/
uint8_t mpuRecBuf[30] = {0};  /*串口接收缓存 */
uint8_t mpuDataBuf[30] = {0}; /*经检验正确的数据包*/
int16_t mpuACC[3] = {0};      /*三轴加速度*/

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
 
 *函数名：calculateStep(void)
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
 