#include "stm32f0xx.h"
#include <stdint.h>
#include "app_bsp.h"

extern UART_HandleTypeDef UartHandle;
extern RTC_HandleTypeDef RtcHandle;
extern TIM_HandleTypeDef TIM_Handle;

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/
void NMI_Handler( void )
{

}

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/
void HardFault_Handler( void )
{
    assert_param( 0u );
}

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/
void SVC_Handler( void )
{

}

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/
void PendSV_Handler( void )
{

}

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/
void SysTick_Handler( void )
{
    HAL_IncTick( );
}

void USART2_IRQHandler( void )
{
    HAL_UART_IRQHandler(&UartHandle);
}

void EXTI4_15_IRQHandler( void )
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12); 
}

void RTC_IRQHandler(void)
{
    HAL_RTC_AlarmIRQHandler(&RtcHandle);
}

void TIM6_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM_Handle);
}
