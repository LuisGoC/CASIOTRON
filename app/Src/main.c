#include "app_bsp.h"
#include "app_serial.h"
#include "app_clock.h"
#include "LCD.h"

/*------ Prototype Functions -------*/
void heart_init(void);
void heart_beat(void);
void dog_init(void);
void pet_the_dog(void);
void tim_init(void);
void SystemClock_Config(void);

/*------ variables -------*/
WWDG_HandleTypeDef WwdgHandle;
TIM_HandleTypeDef TIM_Handle;
uint16_t heartBeatTime;
uint32_t timerTick = 0;
__IO uint8_t timerFlag = RESET;

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    serial_init();
    clock_init();
    heart_init();
    tim_init();
    dog_init();

    for (;;)
    {
        if(timerFlag == SET)
        {
            timerFlag = RESET; 
            if((timerTick % 1UL) == 0UL)
            {
                serial_task();
            }
            if((timerTick % 5UL) == 0UL)
            {
                clock_task();
                heart_beat(); 
            }
            if((timerTick % 3UL) == 0UL)
            {
                pet_the_dog();
            }
            // Micro can sleep here
        }
    }
    return 0u;
}

/**
  * @brief This function initialize the GPIO Pin of the user led in the board for the heart beat.
  * @param void 
  * @retval None
  */
void heart_init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef UserLed;
    UserLed.Mode = GPIO_MODE_OUTPUT_PP;
    UserLed.Pull = GPIO_NOPULL;
    UserLed.Speed = GPIO_SPEED_FREQ_HIGH;
    UserLed.Pin = GPIO_PIN_5 | GPIO_PIN_6;
    HAL_GPIO_Init(GPIOA, &UserLed);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5 | GPIO_PIN_6, RESET);

    heartBeatTime = 300;
}

/**
  * @brief This function toggles the state of the user led each heart beat time.
  * @param void 
  * @retval None
  */
void heart_beat(void)
{
    if(HIL_QUEUE_IsEmpty(&HeartQueue) != 1U)
    {
        (void)HIL_QUEUE_Read(&HeartQueue, &heartBeatTime);
    }
    if((timerTick % (heartBeatTime/10UL)) == 0UL)
    {
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    }
}

/**
  * @brief This function initialize the WWDG accordingly with the configurations added in the structure WwdgHandle.
  * @param void
  * @retval None
  */
void dog_init(void)
{
    /* tClkWwdg = 1/((48000000/4096)/8) = 0.000682 = 682 us */

    if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) != RESET)
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, SET);
        HAL_Delay(4000);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, RESET);
    }
     __HAL_RCC_CLEAR_RESET_FLAGS();

    WwdgHandle.Instance = WWDG;
    WwdgHandle.Init.Prescaler = WWDG_PRESCALER_8;
    WwdgHandle.Init.Window = 100;  // tNoPet = 682us * (127 - 100) = 18.41ms
    WwdgHandle.Init.Counter = 127; // tWWDG = 682us * (127 - 63) = 43.64ms  //tWindow = 43.64 - 18.41 = 25.23 ms
    WwdgHandle.Init.EWIMode = WWDG_EWI_DISABLE;
    HAL_WWDG_Init(&WwdgHandle);
}

/**
  * @brief This function refresh the watchdog.
  * @param void
  * @retval None
  */
void pet_the_dog(void)
{
    HAL_WWDG_Refresh(&WwdgHandle);
}

/**
  * @brief This function initialize the Timer 6 of the microcontroller to have an event each 10 ms.
  * @param void 
  * @retval None
  */
void tim_init(void)
{
    //tim freq = (SystemCoreClock / (prescaler + 1)) = 48000000 Hz / 600 = 80 KHz
    // Time period = (1 / tim freq) = 1 / 80 KHz = 12.5 us
    // Period value = (time base / time period) - 1 = 10 ms / 12.5us = 800; 
    TIM_Handle.Instance = TIM6;
    TIM_Handle.Init.Prescaler = 599;  
    TIM_Handle.Init.Period = 799; 
    HAL_TIM_Base_Init(&TIM_Handle);

    HAL_TIM_Base_Start_IT(&TIM_Handle);
}

/**
  * @brief This function is the callback event for the timer 6.
  * @param htim pointer to a TIM_HandleTypeDef structure that contains the configuration information 
  *             for the specified TIMER peripheral. 
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    (void)htim;
    timerFlag = SET;
    timerTick++;
}

/**
  * @brief This function sets the system clock configuration.
  * @param void 
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);
}
