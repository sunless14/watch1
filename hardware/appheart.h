#ifndef _APPHEART_H
#define _APPHEART_H

#include "main.h"
#include "adc.h"

#define HEART_PERIOD  20      /*���ʲɼ�����*/
extern uint16_t usPulse[128]; /*���ʲɼ�����*/
extern uint8_t  usPos;        /*�������ݱ���λ��*/

void getPulse(uint8_t *pulse, uint16_t *maxValue);
#endif