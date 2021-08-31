#ifndef _LCD_H_
#define _LCD_H_

#include "app_bsp.h"

typedef struct
{
    SPI_HandleTypeDef  	*SpiHandler;
    GPIO_TypeDef		*RstPort;
    uint32_t			RstPin;
    GPIO_TypeDef		*RsPort;
    uint32_t			RsPin;
    GPIO_TypeDef		*CsPort;
    uint32_t			CsPin;
//agregar más elementos si se requieren
}LCD_HandleTypeDef;

void MOD_LCD_Init(LCD_HandleTypeDef *hlcd);
void MOD_LCD_MspInit(LCD_HandleTypeDef *hlcd);
void MOD_LCD_Command(LCD_HandleTypeDef *hlcd, uint8_t cmd );
void MOD_LCD_Data(LCD_HandleTypeDef *hlcd, uint8_t data );
void MOD_LCD_String(LCD_HandleTypeDef *hlcd, char *str );
void MOD_LCD_SetCursor( LCD_HandleTypeDef *hlcd, uint8_t row, uint8_t col );

#endif