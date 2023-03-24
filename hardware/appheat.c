#include "appheart.h"

uint16_t usPulse[128];
uint8_t  usPos; 

/******************************************************************************
 
 *函数名：getArryMax
 *函数功能：取得一个数组的最大值
 *形参：*arry 数组
        len 数组长度
*返回值:uint16_t 最大值
 
 ******************************************************************************/
uint16_t getArryMax(uint16_t* arry, uint8_t len){
	uint16_t Max = arry[0];
	for(int i = 1; i < len; i++){
		Max = Max > arry[i]?  Max: arry[i];
	}
	return Max;
}

/******************************************************************************
 
 *函数名：getArryMin
 *函数功能：取得一个数组的最小值
 *形参：*arry 数组
        len 数组长度
*返回值:uint16_t 最小值
 
 ******************************************************************************/
uint16_t getArryMin(uint16_t* arry, uint8_t len){
	uint16_t Min = arry[0];
	for(int i = 1; i < len; i++){
		Min = Min < arry[i]?  Min: arry[i];
	}
	return Min;
}

/******************************************************************************
 
 *函数名：scaleData
 *函数功能：滤除一个测量周期内的共模数据，只要差模数据，以发大波形曲线
 *形参：
 *返回值：
 
 ******************************************************************************/
void scaleData(){
	uint8_t i;
	uint16_t usMax, usMin, usDelter;
	usMax = getArryMax(usPulse, 128);
	usMin = getArryMin(usPulse, 128);
	usDelter = usMax - usMin;
	if(usDelter < 200){
		for(int i = 0; i  < 128; i++){
			usPulse[i] = usPulse[i] / 2;
		}
	}else{
		for(int i = 0; i < 128; i++){
			usPulse[i] = usDelter *(usPulse[i] -usMin) / usDelter;
		}
	}		
}

/******************************************************************************
 
 *函数名：calculatePulse
 *函数功能：计算心率，获取图形绘制顶点
 *形参：*pulse,心率计算结果指针
        *maxValue,图形绘制顶点指针
 *返回值：
 
 ******************************************************************************/
static void calculatePulse(uint8_t *pulse, uint16_t *maxValue){
	uint8_t i, firstTime, secondTime;
	uint8_t PrePluse, Pulse;
	uint16_t usMax, usMin, usMid;
	
	usMax = getArryMax(usPulse, 128);
	usMin = getArryMin(usPulse, 128);
	usMid = (usMax + usMin) / 2;
	*maxValue = usMax;
	firstTime = secondTime = 0;
	
	for(int i = 0; i < 128; i++){
		PrePluse = Pulse;
		Pulse = (usPulse[i] > usMid)? 1: 0;
		if(PrePluse == 0 && Pulse == 1){
			if(!firstTime) firstTime = i;
			if(firstTime && firstTime < i){
				secondTime = i;
				break;
			}
		}
	}
	if(secondTime - firstTime > 0)
		*pulse = 60 * 1000 / ((secondTime -firstTime)* HEART_PERIOD);
	else
		*pulse = 0;
}
/******************************************************************************
 
 *函数名：getPulse
 *函数功能：采集心率数据，每个采集周期计算一次心率
 *形参：*pulse,心率计算结果指针
        *maxValue,图形绘制顶点指针
 *返回值：
 
 ******************************************************************************/
void getPulse(uint8_t *pulse, uint16_t *maxValue){
	uint16_t usData;
	*pulse = 0;
	*maxValue = 0;
	usData = HAL_ADC_GetValue(&hadc1);
	printf("%d\n",usData);
	usPulse[usPos++] = usData;
	if(usPos >= 128){
		usPos = 0;
		scaleData();
		calculatePulse(pulse,maxValue);
	}
}
