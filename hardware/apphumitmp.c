#include "apphumitmp.h"

#define AHT_ADDRESS 0x70     /*AHT10��ַ*/
/*AHT19��λָ���ʼת��ָ���ȡ��ʪ�Ȼ�����*/
static uint8_t mesCmd[3] = {0xac, 0x33, 0x00};
static uint8_t rstAht[3] = {0xe1, 0x08, 0x00};
static uint8_t rhTemp[6] = {0};

/******************************************************************************
 
 *��������getAHT10
 *�������ܣ�ͨ��HAL��ICC������������ȡAHT10��ʪ�Ƚ��
 *�βΣ�*ahtData,��ʪ�Ƚṹ��ָ��
 *����ֵ��
 
 ******************************************************************************/
void getAHT10(ahtData_t *ahtData){
	uint32_t rht = 0;
	HAL_I2C_Master_Transmit(&hi2c1, AHT_ADDRESS, rstAht, 3, UINT8_MAX); /*��λAHT10*/
	HAL_Delay(10);
  HAL_I2C_Master_Transmit(&hi2c1, AHT_ADDRESS, mesCmd, 3, UINT8_MAX); /*AHT10��ʼת��*/
	HAL_Delay(100);
	HAL_I2C_Master_Receive(&hi2c1, AHT_ADDRESS, rhTemp, 6, UINT8_MAX); /*��ȡת�����*/
	/*�����6B��״̬-ʪ��-ʪ��-ʪ�ȣ�4���¶ȣ�4��-�¶�-�¶�*/
	rht = (rhTemp[1]<<12) + (rhTemp[2]<<4) + (rhTemp[3]>>4);
	ahtData->humi = rht * 100.0 / 1024 / 1024;                               /*����ʪ��*/
	rht = ((rhTemp[3] & 0x0f)<<16) + (rhTemp[4]<<8) + rhTemp[5];
	ahtData->temp = rht * 100.0 / 1024 / 1024;                               /*�����¶�*/
}