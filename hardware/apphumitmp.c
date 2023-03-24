#include "apphumitmp.h"

#define AHT_ADDRESS 0x70     /*AHT10地址*/
/*AHT19复位指令，开始转换指令及读取温湿度缓冲区*/
static uint8_t mesCmd[3] = {0xac, 0x33, 0x00};
static uint8_t rstAht[3] = {0xe1, 0x08, 0x00};
static uint8_t rhTemp[6] = {0};

/******************************************************************************
 
 *函数名：getAHT10
 *函数功能：通过HAL库ICC操作函数，获取AHT10温湿度结果
 *形参：*ahtData,温湿度结构体指针
 *返回值：
 
 ******************************************************************************/
void getAHT10(ahtData_t *ahtData){
	uint32_t rht = 0;
	HAL_I2C_Master_Transmit(&hi2c1, AHT_ADDRESS, rstAht, 3, UINT8_MAX); /*复位AHT10*/
	HAL_Delay(10);
  HAL_I2C_Master_Transmit(&hi2c1, AHT_ADDRESS, mesCmd, 3, UINT8_MAX); /*AHT10开始转换*/
	HAL_Delay(100);
	HAL_I2C_Master_Receive(&hi2c1, AHT_ADDRESS, rhTemp, 6, UINT8_MAX); /*读取转换结果*/
	/*结果共6B：状态-湿度-湿度-湿度（4）温度（4）-温度-温度*/
	rht = (rhTemp[1]<<12) + (rhTemp[2]<<4) + (rhTemp[3]>>4);
	ahtData->humi = rht * 100.0 / 1024 / 1024;                               /*计算湿度*/
	rht = ((rhTemp[3] & 0x0f)<<16) + (rhTemp[4]<<8) + rhTemp[5];
	ahtData->temp = rht * 100.0 / 1024 / 1024;                               /*计算温度*/
}