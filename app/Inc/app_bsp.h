#ifndef _BSP_H_
#define _BSP_H_

#include "stm32f0xx.h"
#include "LCD.h"
#include "temp.h"
#include "queue.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define NONE 0
#define TIME 1
#define DATE 2 
#define ALARM 3
#define TEMP 4
#define NOT_OK 0
#define OK 1
#define LCD_PORT GPIOC
#define RST_PIN GPIO_PIN_0
#define RS_PIN GPIO_PIN_1
#define CS_PIN GPIO_PIN_2 
#define TEMP_ALERT_PORT GPIOB 
#define TEMP_ALERT_PIN GPIO_PIN_12

extern QUEUE_HandleTypeDef MsgQueue;
extern QUEUE_HandleTypeDef HeartQueue;

typedef struct
{
	int8_t msg;	 // tipo de mensaje
	int8_t param1;	 // hora o dia
    int8_t param2;	 // minutos o mes
    int16_t param3; // segundos o año
    uint8_t param4;	 // dia de la semana
}SERIAL_MsgTypeDef;

#endif

