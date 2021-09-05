#include "app_bsp.h"
#include "LCD.h"

/**
  * @brief This function does the LCD initialization routine.
  * @param hlcd pointer to a LCD_HandleTypeDef structure that contains the configuration information 
  *              for the LCD.
  * @retval None
  */
void MOD_LCD_Init(LCD_HandleTypeDef *hlcd)
{
    MOD_LCD_MspInit(hlcd);
    
    HAL_GPIO_WritePin(hlcd->CsPort, hlcd->CsPin, SET); 
    HAL_GPIO_WritePin(hlcd->RstPort, hlcd->RstPin, RESET);
    HAL_Delay(2);
    HAL_GPIO_WritePin(hlcd->RstPort, hlcd->RstPin, SET);
    HAL_Delay(20);
    MOD_LCD_Command(hlcd, 0x30);
    HAL_Delay(2);
    MOD_LCD_Command(hlcd, 0x30);
    MOD_LCD_Command(hlcd, 0x30);
    MOD_LCD_Command(hlcd, 0x39);
    MOD_LCD_Command(hlcd, 0x56);
    MOD_LCD_Command(hlcd, 0x6d);
    HAL_Delay(200);
    MOD_LCD_Command(hlcd, 0x70);
    MOD_LCD_Command(hlcd, 0x0C);
    MOD_LCD_Command(hlcd, 0x06);
    MOD_LCD_Command(hlcd, 0x01);
    HAL_Delay(10);
}

/**
  * @brief  Initializes the LCD MSP.
  * @param  hlcd LCD handle
  * @retval None
  */
__weak void MOD_LCD_MspInit(LCD_HandleTypeDef *hlcd)
{
    (void)hlcd;
}

/**
  * @brief This function transmits a one-byte command to the LCD via SPI.
  * @param hlcd pointer to a LCD_HandleTypeDef structure that contains the configuration information 
  *              for the LCD.
  * @param cmd one-byte integer value command.
  * @retval None
  */
void MOD_LCD_Command(LCD_HandleTypeDef *hlcd, uint8_t cmd )
{
    HAL_GPIO_WritePin(hlcd->CsPort, hlcd->CsPin, RESET);
    HAL_GPIO_WritePin(hlcd->RsPort, hlcd->RsPin, RESET);
    HAL_SPI_Transmit(hlcd->SpiHandler, &cmd, 1, 1000);
    HAL_GPIO_WritePin(hlcd->CsPort, hlcd->CsPin, SET);
}

/**
  * @brief This function transmits a one-byte data to the LCD via SPI.
  * @param hlcd pointer to a LCD_HandleTypeDef structure that contains the configuration information 
  *              for the LCD.
  * @param cmd one-byte integer value data.
  * @retval None
  */
void MOD_LCD_Data(LCD_HandleTypeDef *hlcd, uint8_t data )
{
    HAL_GPIO_WritePin(hlcd->CsPort, hlcd->CsPin, RESET);
    HAL_GPIO_WritePin(hlcd->RsPort, hlcd->RsPin, SET);
    HAL_SPI_Transmit(hlcd->SpiHandler, &data, 1, 1000);
    HAL_GPIO_WritePin(hlcd->CsPort, hlcd->CsPin, SET);
}

/**
  * @brief This function transmits full character string data to the LCD via SPI.
  * @param hlcd pointer to a LCD_HandleTypeDef structure that contains the configuration information 
  *              for the LCD.
  * @param str pointer to a string data.
  * @retval None
  */
void MOD_LCD_String(LCD_HandleTypeDef *hlcd, char *str )
{
    for(uint8_t i = 0; str[i] != '\0'; i++){
        HAL_GPIO_WritePin(hlcd->CsPort, hlcd->CsPin, RESET);
        HAL_GPIO_WritePin(hlcd->RsPort, hlcd->RsPin, SET);
        HAL_SPI_Transmit(hlcd->SpiHandler,(uint8_t *) &str[i], 1, 1000);
        HAL_GPIO_WritePin(hlcd->CsPort, hlcd->CsPin, SET);
    }
}

/**
  * @brief This function sets the Display data RAM address in which the LCD Cursor points.
  * @param hlcd pointer to a LCD_HandleTypeDef structure that contains the configuration information 
  *              for the LCD.
  * @param row integer value for the row position.
  * @param col integer value for the column position.
  * @retval None
  */
void MOD_LCD_SetCursor( LCD_HandleTypeDef *hlcd, uint8_t row, uint8_t col )
{
    uint8_t a[2][16] = {{0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F}, 
                        {0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF}};
    if((row >= 0u) && (row <= 1u)){
        if((col >= 0u) && (col <= 15u)){
            MOD_LCD_Command(hlcd, a[row][col]);
        }
    }
}


