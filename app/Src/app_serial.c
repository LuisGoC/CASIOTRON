#include "app_serial.h"
#include "app_bsp.h"

/*------ Constants -------*/
#define SERIAL_IDLE 0
#define SERIAL_SPLIT 1
#define SERIAL_VALUES 2
#define SERIAL_MESSAGE 3
#define SERIAL_ERROR 4

/*------ Prototype Functions -------*/
void splitCommandWords(void);
uint8_t convertCommandsToValues(void);
uint8_t checkErrorsWriteMsg(void);
uint8_t ZellersCongruenceAlgorithm(uint8_t day, uint8_t month, uint8_t year);
int32_t myAtoi(char* str);
void clearBuffer(void);

/*------ Variables -------*/
uint8_t RxBuffer[20];
uint8_t temporalBuffer[20];
uint8_t RxByte;
uint8_t *AT = NULL;
uint8_t *commandType = NULL;
uint8_t *charDigit1 = NULL;
uint8_t *charDigit2 = NULL;
uint8_t *charDigit3 = NULL;
uint8_t serialBuffer[116];
uint16_t heartBuffer[40];

UART_HandleTypeDef UartHandle;
QUEUE_HandleTypeDef SerialQueue;
QUEUE_HandleTypeDef MsgQueue;
QUEUE_HandleTypeDef HeartQueue;
SERIAL_MsgTypeDef msgBuffer[8];
SERIAL_MsgTypeDef MsgToSend;

__IO ITStatus uartStat = RESET;

/**
  * @brief This is the main function of the app_serial, it contains the state machine to be executed during the process.
  * @param void
  * @retval None
  */
void serial_task(void)
{
    static uint8_t serialState = SERIAL_IDLE;
    uint8_t Counter = 0;
    uint8_t data;

    switch (serialState)
    {
    case SERIAL_IDLE:
        while (HIL_QUEUE_IsEmpty(&SerialQueue) == (uint8_t)NOT_OK)
        {
            HAL_NVIC_DisableIRQ(USART2_IRQn);
            HAL_NVIC_DisableIRQ(RTC_IRQn);
            HAL_NVIC_DisableIRQ(TIM6_IRQn);
            HAL_NVIC_DisableIRQ(EXTI4_15_IRQn);
            (void)HIL_QUEUE_Read(&SerialQueue, &data);
            HAL_NVIC_EnableIRQ(USART2_IRQn);
            HAL_NVIC_EnableIRQ(RTC_IRQn);
            HAL_NVIC_EnableIRQ(TIM6_IRQn);
            HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

            if (data == (uint8_t)'\r')
            {
                serialState = SERIAL_SPLIT;
                break;
            }
            else
            {
                RxBuffer[Counter] = data;
                Counter++;
            }
        }    
        break;
    case SERIAL_SPLIT:
        splitCommandWords();
        serialState = SERIAL_VALUES;
        break;
    case SERIAL_VALUES:
        if (convertCommandsToValues() == 1u)
        {
            serialState = SERIAL_ERROR;
        }
        else
        {
            serialState = SERIAL_MESSAGE;
        }
        break;
    case SERIAL_MESSAGE:
        if (checkErrorsWriteMsg() == 1u)
        {
            serialState = SERIAL_ERROR;
        }
        else
        {
            clearBuffer();
            serialState = SERIAL_IDLE;
            HAL_UART_Transmit(&UartHandle, (const uint8_t *)"\r\nOK\r\n", 6, 5000);
        }
        break;
    case SERIAL_ERROR:
        clearBuffer();
        HAL_UART_Transmit(&UartHandle, (const uint8_t *)"\r\nERROR\r\n", 9, 5000);
        serialState = SERIAL_IDLE;
        break;
    default:
        serialState = SERIAL_IDLE;
        break;
    }
}

/**
  * @brief This function initializes all the required for serial UART communication between 
  *        the microcontroller and the computer terminal.
  * @param void
  * @retval None
  */
void serial_init(void)
{
    UartHandle.Instance = USART2;
    UartHandle.Init.BaudRate = 115200;
    UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits = UART_STOPBITS_1;
    UartHandle.Init.Parity = UART_PARITY_NONE;
    UartHandle.Init.Mode = UART_MODE_TX_RX;
    UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&UartHandle);

    SerialQueue.Buffer = (void *)serialBuffer;
    SerialQueue.Elements = 116u;
    SerialQueue.Size = sizeof(uint8_t);
    HIL_QUEUE_Init(&SerialQueue);

    MsgQueue.Buffer = (void *)msgBuffer;
    MsgQueue.Elements = 8u;
    MsgQueue.Size = sizeof(SERIAL_MsgTypeDef);
    HIL_QUEUE_Init(&MsgQueue);

    HeartQueue.Buffer = (void *)heartBuffer;
    HeartQueue.Elements = 40u;
    HeartQueue.Size = sizeof(uint16_t);
    HIL_QUEUE_Init(&HeartQueue);

    HAL_UART_Receive_IT(&UartHandle, &RxByte, 1);
}

/**
  * @brief This is the callback function for the UART, this function stores each data byte received in a buffer 
  *        and sets a flag when a complete string is received. 
  * @param huart pointer to a UART_HandleTypeDef structure that contains the configuration information 
  *              for the specified USART peripheral.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    (void)huart;
    (void)HIL_QUEUE_Write(&SerialQueue, &RxByte);
    HAL_UART_Receive_IT(&UartHandle, &RxByte, 1);
}

/**
  * @brief This function create a copy of the string received and split the message 
  *        according with the AT command.
  * @param void
  * @retval None
  */
void splitCommandWords(void)
{
    (void)strcpy((char *)temporalBuffer, (char *)RxBuffer);
    AT = (uint8_t *)strtok((char *)temporalBuffer, "+");
    commandType = (uint8_t *)strtok(NULL, "=");
    if (strcmp((char *)commandType, "TIME") == 0)
    {
        charDigit1 = (uint8_t *)strtok(NULL, ",");
        charDigit2 = (uint8_t *)strtok(NULL, ",");
        charDigit3 = (uint8_t *)strtok(NULL, "\r");
    }
    else if (strcmp((char *)commandType, "DATE") == 0)
    {
        charDigit1 = (uint8_t *)strtok(NULL, ",");
        charDigit2 = (uint8_t *)strtok(NULL, ",");
        charDigit3 = (uint8_t *)strtok(NULL, "\r");
    }
    else if (strcmp((char *)commandType, "ALARM") == 0)
    {
        charDigit1 = (uint8_t *)strtok(NULL, ",");
        charDigit2 = (uint8_t *)strtok(NULL, "\r");
        charDigit3 = NULL;
    }
    else if (strcmp((char *)commandType, "HEARTBEAT") == 0)
    {
        charDigit3 = (uint8_t *)strtok(NULL, "\r");
        charDigit2 = NULL;
        charDigit1 = NULL;
    }
    else if (strcmp((char *)commandType, "TEMP") == 0)
    {
        charDigit1 = (uint8_t *)strtok(NULL, ",");
        charDigit2 = (uint8_t *)strtok(NULL, "\r");
        charDigit3 = NULL;
    }
    else
    {
        //do nothing
    }
}

/**
  * @brief This function checks if the characters received are numbers, 
  *        converts the character string commands in integer values 
  *        and stores each value in the SERIAL_MsgTypeDef structure.
  * @param void
  * @retval unsigned integer value if an error occurs.
  */
uint8_t convertCommandsToValues(void)
{
    uint8_t *cadena[] = {charDigit1, charDigit2, charDigit3};
    uint8_t err = 0;
    for (uint8_t i = 0; i < 3u; i++)
    {
        for (uint8_t j = 0; cadena[i][j] != (uint8_t)'\0'; j++)
        {
            if ( !((cadena[i][j] >= (uint8_t)'0') && (cadena[i][j] <= (uint8_t)'9'))) //Need to be modified for negative numbers
            {
                err = 1; //error
            }
        }
    }
    MsgToSend.param1 = myAtoi((char *)charDigit1);
    MsgToSend.param2 = myAtoi((char *)charDigit2);
    MsgToSend.param3 = myAtoi((char *)charDigit3);
    MsgToSend.param4 = ZellersCongruenceAlgorithm(MsgToSend.param1, MsgToSend.param2, (MsgToSend.param3 + 2000));
    return err;
}

/**
  * @brief This function checks for out of range value errors and assigns 
  *        the type of message to the SERIAL_MsgTypeDef structure.
  * @param void
  * @retval unsigned integer value if an error occurs.
  */
uint8_t checkErrorsWriteMsg(void)
{
    uint8_t err = 0;

    if (strcmp((char *)AT, "AT") == 0)
    {
        if (strcmp((char *)commandType, "TIME") == 0)
        {
            if ((MsgToSend.param1 < 0) || (MsgToSend.param1 > 23))
            {
                err = 1;
            }
            if ((MsgToSend.param2 < 0) || (MsgToSend.param2 > 59))
            {
                err = 1;
            }
            if ((MsgToSend.param3 < 0) || (MsgToSend.param3 > 59))
            {
                err = 1;
            }
            if (err == 0u)
            {
                MsgToSend.msg = TIME;
                if (HIL_QUEUE_Write(&MsgQueue, &MsgToSend) != (uint8_t)OK)
                {
                    err = 1;
                }
            }
        }
        else if (strcmp((char *)commandType, "DATE") == 0)
        {
            if ((MsgToSend.param1 < 1) || (MsgToSend.param1 > 31))
            {
                err = 1;
            }
            if ((MsgToSend.param2 < 1) || (MsgToSend.param2 > 12))
            {
                err = 1;
            }
            if ((MsgToSend.param3 < 0) || (MsgToSend.param3 > 99))
            {
                err = 1;
            }
            if ((MsgToSend.param4 < 1u) || (MsgToSend.param4 > 7u))
            {
                err = 1;
            }
            if (err == 0u)
            {
                MsgToSend.msg = DATE;
                if (HIL_QUEUE_Write(&MsgQueue, &MsgToSend) != (uint8_t)OK)
                {
                    err = 1;
                }
            }
        }
        else if (strcmp((char *)commandType, "ALARM") == 0)
        {
            if ((MsgToSend.param1 < 0) || (MsgToSend.param1 > 23))
            {
                err = 1;
            }
            if ((MsgToSend.param2 < 0) || (MsgToSend.param2 > 59))
            {
                err = 1;
            }
            if (err == 0u)
            {
                MsgToSend.msg = ALARM;
                if (HIL_QUEUE_Write(&MsgQueue, &MsgToSend) != (uint8_t)OK)
                {
                    err = 1;
                }
            }
        }
        else if (strcmp((char *)commandType, "HEARTBEAT") == 0)
        {
            if (((MsgToSend.param3 % 50) != 0) || (MsgToSend.param3 < 50) || (MsgToSend.param3 > 1000))
            {
                err = 1;
            }
            if (err == 0u)
            {
                if (HIL_QUEUE_Write(&HeartQueue, &MsgToSend.param3) != (uint8_t)OK)
                {
                    err = 1;
                }
            }
        }
        else if (strcmp((char *)commandType, "TEMP") == 0)
        {
            if (MsgToSend.param1 > MsgToSend.param2)
            {
                err = 1;
            }
            if ((MsgToSend.param1 < -20) || (MsgToSend.param1 > 100))
            {
                err = 1;
            }
            if ((MsgToSend.param2 < -20) || (MsgToSend.param2 > 100))
            {
                err = 1;
            }
            if (err == 0u)
            {
                MsgToSend.msg = TEMP;
                if (HIL_QUEUE_Write(&MsgQueue, &MsgToSend) != (uint8_t)OK)
                {
                    err = 1;
                }
            }
        }
        else
        {
            err = 1;
        }
    }
    else
    {
        err = 1;
    }
    return err;
}

void clearBuffer(void)
{
    (void)memset(RxBuffer, 0, sizeof(RxBuffer));
    (void)memset(temporalBuffer, 0, sizeof(temporalBuffer));
}

/**
  * @brief This function contains the Zeller's congruence algorithm 
  *        to calculate the day of the week for any Gregorian calendar date.
  * @param day integer value for the day in the range of 1 to 31.
  * @param month integer value for the month in the range of 1 to 12.
  * @param year integer value for the year in this century.
  * @retval unsigned integer value of the day of the week from 1 to 7.
  */
uint8_t ZellersCongruenceAlgorithm(uint8_t day, uint8_t month, uint8_t year)
{
    uint16_t h;
    uint32_t K;
    uint32_t J;
    uint8_t mCopy;
    uint8_t yCopy;
    uint8_t wday;

    mCopy = month;
    yCopy = year;

    if (mCopy <= 2u)
    {
        mCopy += 12u;
        yCopy = yCopy - 1u;
    }
    else
    {
        mCopy = mCopy - 2u;
    }

    K = yCopy % 100u;
    J = yCopy / 100u;

    h = ((700u + (((26u * mCopy) - 2u) / 10u) + day + K + (K / 4u) + ((J / 4u) + (5u * J))) % 7u) + 1u;
    if (h == 0u)
    {
        wday = 7u;
    }
    else
    {
        wday = h;
    }
    return wday;
}

/**
  * @brief This function converts an array to an integer number.
  * @param str pointer to a character type.
  * @retval an integer value of the number.
  */
int32_t myAtoi(char* str)
{
    int32_t res = 0;
    int32_t sign = 1;
    int32_t i = 0;
 
    // If number is negative,
    if (str[0] == '-') {
        sign = -1;
        // Also update index of first digit
        i++;
    }
 
    // Iterate through all digits
    // and update the result
    for(; str[i] != '\0'; ++i)
    {
        res = ((uint32_t)res * 10UL) + ((uint8_t)str[i] - (uint8_t)'0');
    }
    
    // Return result with sign
    return sign * res;
}