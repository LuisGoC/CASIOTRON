#include "app_bsp.h"
#include "temp.h"

/**
  * @brief This function initializes the temperature sensor.
  * @param void
  * @retval None
  */
void MOD_TEMP_Init( TEMP_HandleTypeDef *htemp )
{
    MOD_TEMP_MspInit( htemp );  
}

/**
  * @brief  Initializes the TEMP MSP.
  * @param  htemp TEMP handle
  * @retval None
  */
__weak void MOD_TEMP_MspInit( TEMP_HandleTypeDef *htemp )
{

}

/**
  * @brief This function reads the temperature register from the sensor.
  * @param htemp pointer to a TEMP_HandleTypeDef structure that contains the configuration information 
  *              for the temperature sensor.
  * @retval 16 bit unsigned integer temperature register. 
  */
uint16_t MOD_TEMP_Read( TEMP_HandleTypeDef *htemp )
{
    uint8_t readData[2] = {0};
    uint16_t registerRead = 0;
    uint8_t registerAddress = TEMPERATURE_REGISTER; 

    HAL_I2C_Master_Transmit(htemp->I2cHandler, htemp->SensorAddress, &registerAddress, 1, 1000);
    HAL_I2C_Master_Receive(htemp->I2cHandler, htemp->SensorAddress, readData, 2, 1000);
    
    registerRead = readData[0];
    registerRead = registerRead << 8;
    registerRead = registerRead | readData[1];

    return registerRead;
}

/**
  * @brief This function writes the lower, upper and critical registers of the sensor.
  * @param htemp pointer to a TEMP_HandleTypeDef structure that contains the configuration information 
  *              for the temperature sensor.
  * @retval none 
  */
void MOD_TEMP_SetAlarms( TEMP_HandleTypeDef *htemp, uint16_t lower, uint16_t upper  )
{
    uint8_t writeData[3] = {0};
    
    writeData[0] = LOWER_REGISTER;
    writeData[1] = lower >> 8;
    writeData[2] = lower;

    HAL_I2C_Master_Transmit(htemp->I2cHandler, htemp->SensorAddress, writeData, 3, 1000);
    
    writeData[0] = UPPER_REGISTER;
    writeData[1] = upper >> 8;
    writeData[2] = upper;

    HAL_I2C_Master_Transmit(htemp->I2cHandler, htemp->SensorAddress, writeData, 3, 1000);

    writeData[0] = CRITICAL_REGISTER;
    writeData[1] = ((upper >> 4) + 5) >> 4;
    writeData[2] = ((upper >> 4) +5);

    HAL_I2C_Master_Transmit(htemp->I2cHandler, htemp->SensorAddress, writeData, 3, 1000);

    writeData[0] = CONFIG_REGISTER;
    writeData[1] = 0x00;
    writeData[2] = 0x08;

    HAL_I2C_Master_Transmit(htemp->I2cHandler, htemp->SensorAddress, writeData, 3, 1000);
}

/**
  * @brief This function disables the temperature alarm, writing the config register to zeros.
  * @param htemp pointer to a TEMP_HandleTypeDef structure that contains the configuration information 
  *              for the temperature sensor.
  * @retval none 
  */
void MOD_TEMP_DisableAlarm( TEMP_HandleTypeDef *htemp )
{
    uint8_t writeData[3] = {CONFIG_REGISTER, 0x00, 0x00};
    
    HAL_I2C_Master_Transmit(htemp->I2cHandler, htemp->SensorAddress, writeData, 3, 1000);  
}