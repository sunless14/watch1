#ifndef _APPHUMITMP_H
#define _APPHUMITMP_H

#include "i2c.h"
#include "main.h"
typedef struct{
	float humi;
	float temp;
}ahtData_t;

void getAHT10(ahtData_t* ahtData);
#endif