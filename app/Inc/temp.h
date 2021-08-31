#ifndef _TEMP_H_
#define _TEMP_H_

#include "app_bsp.h"

#define CONFIG_REGISTER 0x01
#define UPPER_REGISTER 0x02
#define LOWER_REGISTER 0x03
#define CRITICAL_REGISTER 0x04
#define TEMPERATURE_REGISTER 0x05

typedef struct
{
    I2C_HandleTypeDef *I2cHandler;
    GPIO_TypeDef *AlertPort;
    uint32_t AlertPin;
    uint8_t SensorAddress;
}TEMP_HandleTypeDef;

void MOD_TEMP_Init( TEMP_HandleTypeDef *htemp );
void MOD_TEMP_MspInit( TEMP_HandleTypeDef *htemp );
uint16_t MOD_TEMP_Read( TEMP_HandleTypeDef *htemp );
void MOD_TEMP_SetAlarms( TEMP_HandleTypeDef *htemp, uint16_t lower, uint16_t upper  );
void MOD_TEMP_DisableAlarm( TEMP_HandleTypeDef *htemp ); 


#endif