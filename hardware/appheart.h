#ifndef _APPHEART_H
#define _APPHEART_H

#include "main.h"
#include "adc.h"

#define HEART_PERIOD  20      /*心率采集周期*/
extern uint16_t usPulse[128]; /*心率采集数组*/
extern uint8_t  usPos;        /*心率数据保存位置*/

void getPulse(uint8_t *pulse, uint16_t *maxValue);
#endif