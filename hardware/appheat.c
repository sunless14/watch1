#include "appheart.h"

uint16_t usPulse[128];
uint8_t  usPos; 

/******************************************************************************
 
 *��������getArryMax
 *�������ܣ�ȡ��һ����������ֵ
 *�βΣ�*arry ����
        len ���鳤��
*����ֵ:uint16_t ���ֵ
 
 ******************************************************************************/
uint16_t getArryMax(uint16_t* arry, uint8_t len){
	uint16_t Max = arry[0];
	for(int i = 1; i < len; i++){
		Max = Max > arry[i]?  Max: arry[i];
	}
	return Max;
}

/******************************************************************************
 
 *��������getArryMin
 *�������ܣ�ȡ��һ���������Сֵ
 *�βΣ�*arry ����
        len ���鳤��
*����ֵ:uint16_t ��Сֵ
 
 ******************************************************************************/
uint16_t getArryMin(uint16_t* arry, uint8_t len){
	uint16_t Min = arry[0];
	for(int i = 1; i < len; i++){
		Min = Min < arry[i]?  Min: arry[i];
	}
	return Min;
}

/******************************************************************************
 
 *��������scaleData
 *�������ܣ��˳�һ�����������ڵĹ�ģ���ݣ�ֻҪ��ģ���ݣ��Է���������
 *�βΣ�
 *����ֵ��
 
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
 
 *��������calculatePulse
 *�������ܣ��������ʣ���ȡͼ�λ��ƶ���
 *�βΣ�*pulse,���ʼ�����ָ��
        *maxValue,ͼ�λ��ƶ���ָ��
 *����ֵ��
 
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
 
 *��������getPulse
 *�������ܣ��ɼ��������ݣ�ÿ���ɼ����ڼ���һ������
 *�βΣ�*pulse,���ʼ�����ָ��
        *maxValue,ͼ�λ��ƶ���ָ��
 *����ֵ��
 
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
