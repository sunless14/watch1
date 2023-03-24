#ifndef _APPSTEP_H
#define _APPSTEP_H

#include "main.h"
#include "usart.h"
#include <string.h>
#include <math.h>

#define TRUE     1
#define FALSE    0
#define WAVE_NUM 4
#define PEEK_MIN_VALUE  20000

typedef struct{
	uint32_t calories;
	uint32_t distance;
	uint32_t stepCount;
}sportsInfo_t;

typedef struct{
	uint8_t height;
	uint8_t weight;
}personInfo_t;

sportsInfo_t *getStep(void);
void walkTask(void *pvParameters);
void stepTask(void *pvParameters);
sportsInfo_t *useAccToStep(void);
#endif