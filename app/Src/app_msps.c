#include "stm32f0xx.h"
#include <stdint.h>
#include "app_bsp.h"

void HAL_MspInit( void )
{
    
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    (void)huart;
    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_USART2_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    HAL_NVIC_SetPriority(USART2_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
}

void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
    (void)hrtc;
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_PeriphCLKInitTypeDef RCC_RTCPeriClkInit;
    //Encendemos el LSE
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);
    //Seleccionamos el LSE como la fuente del RTCCLK
    RCC_RTCPeriClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    RCC_RTCPeriClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    HAL_RCCEx_PeriphCLKConfig(&RCC_RTCPeriClkInit);
    //Habilitamos el reloj RTC
    __HAL_RCC_RTC_ENABLE();
    //Habilitamos el IRQ en el NVIC
    HAL_NVIC_SetPriority(RTC_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(RTC_IRQn);
}

void HAL_WWDG_MspInit(WWDG_HandleTypeDef *hwwdg)
{
    (void)hwwdg;
  /* WWDG Peripheral clock enable */
  __HAL_RCC_WWDG_CLK_ENABLE();
}

void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    (void)hspi;
    /*pines B3, B4 y B5 en funcion alterna spi1 */
    GPIO_InitTypeDef GPIO_InitStruct;
    
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_SPI1_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF0_SPI1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void MOD_LCD_MspInit(LCD_HandleTypeDef *hlcd)
{
    (void)hlcd;
    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Pin = RST_PIN | RS_PIN | CS_PIN;
    HAL_GPIO_Init(LCD_PORT, &GPIO_InitStruct);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    (void)htim;
    //Enable the clock
    __HAL_RCC_TIM6_CLK_ENABLE();

    //Enable IRQ of TIMER6
    HAL_NVIC_EnableIRQ(TIM6_IRQn);
    HAL_NVIC_SetPriority(TIM6_IRQn, 3, 0);
}

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
    (void)hi2c;
    GPIO_InitTypeDef GPIO_InitStruct;
    
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_I2C2_CLK_ENABLE();
    
    GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_I2C2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void MOD_TEMP_MspInit( TEMP_HandleTypeDef *htemp )
{
    (void)htemp;
    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Pin = TEMP_ALERT_PIN; 
    HAL_GPIO_Init(TEMP_ALERT_PORT, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI4_15_IRQn, 4, 0);
    HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
}