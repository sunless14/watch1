#ifndef _APPTASK_H_
#define _APPTASK_H_

#include "main.h"
#include "cmsis_os.h"
#include "stdio.h"
#include "key.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

static void led0Task(void * pvParameters);
static void led1Task(void * pvParameters);
static void printTask(void * pvParameters);
static void keyTask(void * pvParameters);
void appStartTask(void);
#endif