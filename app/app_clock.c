#include "app_clock.h"
#include "app_bsp.h"
#include "LCD.h"
#include "temp.h"

/*------ Constants -------*/
#define CLK_IDLE 0
#define CLK_START_ALARM 1
#define CLK_SET_CONFIG 2
#define CLK_SHOW_ALARM 3
#define CLK_SHOW_TIME 4
#define I2C_ADDRESS 0x30F
#define I2C_TIMING 0x00E51842
#define TEMPERATURE_ADDRESS 0x3E  

/*------ Prototype Functions -------*/
void RTC_TimeConfig(void);
void RTC_DateConfig(void);
void RTC_AlarmConfig(void);
void TEMP_LOWER_UPPER_CONFIG(void);
int8_t getTemperature(void);
void startAlarm(void);
void showAlarmDetails(void);
void showDateAndTime(void);
void setRtcConfiguration(void);
void spi_init(void);
void i2c_init(void);
void lcd_init(void);
void temp_init(void);
void intToString(char *str, uint8_t num);
void joinStrings(char *str1, char *str2);

/*------ Variables -------*/
RTC_HandleTypeDef RtcHandle;
RTC_TimeTypeDef RTC_TimeInit;
RTC_DateTypeDef RTC_DateInit;
RTC_AlarmTypeDef RTC_AlarmInit;
RTC_TimeTypeDef RTC_TimeRead;
RTC_DateTypeDef RTC_DateRead;
RTC_AlarmTypeDef RTC_AlarmRead;
SPI_HandleTypeDef SpiHandle;
I2C_HandleTypeDef I2cHandle;
LCD_HandleTypeDef LcdHandle;
TEMP_HandleTypeDef TmpHandle;
SERIAL_MsgTypeDef MsgToRead;
uint8_t temperatureFlag = 0;
int8_t lowerValue = 0;
int8_t upperValue = 0;
__IO ITStatus alarmStat = RESET;
__IO ITStatus tempAlarmStat = RESET;

/**
  * @brief This function initializes everything necessary for the clock task.
  * @param void 
  * @retval None
  */
void clock_init(void)
{
    spi_init();
    lcd_init();
    i2c_init();
    temp_init();

    //RTC initialization
    RtcHandle.Instance = RTC;
    RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
    RtcHandle.Init.AsynchPrediv = 0x7F;
    RtcHandle.Init.SynchPrediv = 0xFF;
    RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
    RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_LOW;
    RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_PUSHPULL;
    HAL_RTC_Init(&RtcHandle);

    //GPIO User Button initialization
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitTypeDef UserButton;
    UserButton.Mode = GPIO_MODE_INPUT;
    UserButton.Pull = GPIO_NOPULL;
    UserButton.Speed = GPIO_SPEED_FREQ_LOW;
    UserButton.Pin = GPIO_PIN_13;
    HAL_GPIO_Init(GPIOC, &UserButton);

    //RTC initial config
    RTC_TimeInit.Hours = 0;
    RTC_TimeInit.Minutes = 0;
    RTC_TimeInit.Seconds = 0;
    HAL_RTC_SetTime(&RtcHandle, &RTC_TimeInit, RTC_FORMAT_BIN);

    RTC_DateInit.Date = 1;
    RTC_DateInit.Month = RTC_MONTH_JANUARY;
    RTC_DateInit.Year = 0;
    RTC_DateInit.WeekDay = RTC_WEEKDAY_SATURDAY;
    HAL_RTC_SetDate(&RtcHandle, &RTC_DateInit, RTC_FORMAT_BIN);
    HAL_RTC_DeactivateAlarm(&RtcHandle, RTC_ALARM_A);
}

/**
  * @brief This function initializes the SPI with the settings added in the SPI_HandleTypeDef structure.
  * @param void
  * @retval None
  */
void spi_init(void)
{
    /*Configuramos al spi en modo maestro, comunicacion full-duplex, polaridad
    del reloj en alto y fase en flanco de bajada */
    SpiHandle.Instance = SPI1;
    SpiHandle.Init.Mode = SPI_MODE_MASTER;
    SpiHandle.Init.Direction = SPI_DIRECTION_2LINES;
    SpiHandle.Init.DataSize = SPI_DATASIZE_8BIT;
    SpiHandle.Init.CLKPolarity = SPI_POLARITY_HIGH;
    SpiHandle.Init.CLKPhase = SPI_PHASE_2EDGE;
    SpiHandle.Init.NSS = SPI_NSS_SOFT;
    SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
    SpiHandle.Init.FirstBit = SPI_FIRSTBIT_MSB;
    SpiHandle.Init.TIMode = SPI_TIMODE_DISABLE;
    SpiHandle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    SpiHandle.Init.CRCPolynomial = 7;

    /* aplicamos la configuracion al spi 1 pero antes nos aseguramos que el
    esclavo este deshabilitado pin C2 en alto*/
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, SET);
    HAL_SPI_Init(&SpiHandle);
}

/**
  * @brief This function initializes the I2C protocol with the settings added in the I2C_HandleTypeDef structure.
  * @param void
  * @retval None
  */
void i2c_init(void)
{
    I2cHandle.Instance             = I2C2;
    I2cHandle.Init.Timing          = I2C_TIMING;
    I2cHandle.Init.OwnAddress1     = I2C_ADDRESS;
    I2cHandle.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
    I2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    I2cHandle.Init.OwnAddress2     = 0x00;
    I2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    I2cHandle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;
    HAL_I2C_Init(&I2cHandle);

    //HAL_I2CEx_ConfigAnalogFilter(&I2cHandle,I2C_ANALOGFILTER_ENABLE);
}

/**
  * @brief This function initializes the LCD with the settings added in the LCD_HandleTypeDef structure.
  * @param void
  * @retval None
  */
void lcd_init(void)
{
    LcdHandle.SpiHandler = &SpiHandle;
    LcdHandle.RstPort = LCD_PORT;
    LcdHandle.RstPin = RST_PIN;
    LcdHandle.RsPort = LCD_PORT;
    LcdHandle.RsPin = RS_PIN;
    LcdHandle.CsPort = LCD_PORT;
    LcdHandle.CsPin = CS_PIN;
    MOD_LCD_Init(&LcdHandle);
}

/**
  * @brief This function initializes the temperature sensor with the settings in the TEMP_HandleTypeDef structure.
  * @param void
  * @retval None
  */
void temp_init(void)
{
    TmpHandle.I2cHandler = &I2cHandle;
    TmpHandle.AlertPort = TEMP_ALERT_PORT;
    TmpHandle.AlertPin = TEMP_ALERT_PIN; 
    TmpHandle.SensorAddress = TEMPERATURE_ADDRESS;
    MOD_TEMP_Init(&TmpHandle);
}

/**
  * @brief This function sets the parameters for the RTC in the hour, minute and second values.
  * @param void
  * @retval None
  */
void RTC_TimeConfig(void)
{
    RTC_TimeInit.Hours = MsgToRead.param1;
    RTC_TimeInit.Minutes = MsgToRead.param2;
    RTC_TimeInit.Seconds = MsgToRead.param3;
    HAL_RTC_SetTime(&RtcHandle, &RTC_TimeInit, RTC_FORMAT_BIN);
}

/**
  * @brief This function sets the parameters for the RTC in the hour, minute and second values.
  * @param void
  * @retval None
  */
void RTC_DateConfig(void)
{
    RTC_DateInit.Date = MsgToRead.param1;
    RTC_DateInit.Month = MsgToRead.param2;
    RTC_DateInit.Year = MsgToRead.param3;
    RTC_DateInit.WeekDay = MsgToRead.param4;
    HAL_RTC_SetDate(&RtcHandle, &RTC_DateInit, RTC_FORMAT_BIN);
}

/**
  * @brief This function sets the parameters for the RTC Alarm in the hour and minute values.
  * @param void
  * @retval None
  */
void RTC_AlarmConfig(void)
{
    memset(&RTC_AlarmInit, 0, sizeof(RTC_AlarmInit));

    HAL_RTC_DeactivateAlarm(&RtcHandle, RTC_ALARM_A);

    RTC_AlarmInit.Alarm = RTC_ALARM_A;
    RTC_AlarmInit.AlarmTime.Hours = MsgToRead.param1;
    RTC_AlarmInit.AlarmTime.Minutes = MsgToRead.param2;
    RTC_AlarmInit.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY | RTC_ALARMMASK_SECONDS;
    RTC_AlarmInit.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
    HAL_RTC_SetAlarm_IT(&RtcHandle, &RTC_AlarmInit, RTC_FORMAT_BIN);
}

/**
  * @brief This function sets the lower and upper configuration alarm of the temperature sensor.
  * @param void
  * @retval None
  */
void TEMP_LOWER_UPPER_CONFIG(void)
{
    uint16_t lower, upper;

    if(MsgToRead.param1 < 0)
    {
        lower = ((((MsgToRead.param1 * -1) - 1) ^ 255) << 4) | 0x800;
    }
    else
    {
        lower = (MsgToRead.param1 << 4);
    }

    if(MsgToRead.param2 < 0)
    {
        upper = ((((MsgToRead.param2 * -1) - 1) ^ 255) << 4) | 0x800;
    }
    else
    {
        upper = (MsgToRead.param2 << 4);
    }

    MOD_TEMP_SetAlarms(&TmpHandle, lower, upper);
}

/**
  * @brief This function gets the temperature register from the sensor and returns the data in decimal celsius.
  * @param void
  * @retval An integer that represents the temperature value.
  */
int8_t getTemperature(void)
{
    uint16_t temperatureRegister;
    uint8_t upperRegister, lowerRegister;
    int8_t temperature;

    temperatureRegister = MOD_TEMP_Read(&TmpHandle);
    
    upperRegister = temperatureRegister >> 8;
    lowerRegister = temperatureRegister;

    upperRegister = upperRegister & 0x1F;

    if ((upperRegister & 0x10) == 0x10) //Si la temperatura es negativa
    { 
        upperRegister = upperRegister & 0x0F; // se limpia el bit del signo
        temperature = 256 - ((upperRegister << 4) + (lowerRegister >> 4));
    }
    else
    { 
        temperature = ((upperRegister << 4) + (lowerRegister >> 4));
    }

    return temperature;
}

/**
  * @brief This is the callback for the alarm interrupt, this function sets the alarm flag.
  * @param void
  * @retval None
  */
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
    alarmStat = SET;
    HAL_RTC_DeactivateAlarm(&RtcHandle, RTC_ALARM_A);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    tempAlarmStat = SET; //bandera de alarma activa
    MOD_TEMP_DisableAlarm(&TmpHandle);
    temperatureFlag = 0; //bandera de alarma configurada
}

/**
  * @brief This function switches between the configuration to be set, it could be time, alarm, date, temperature alarm.
  * @param void
  * @retval None
  */
void setRtcConfiguration(void)
{
    switch (MsgToRead.msg)
    {
    case TIME:
        RTC_TimeConfig();
        break;
    case DATE:
        RTC_DateConfig();
        break;
    case ALARM:
        RTC_AlarmConfig();
        break;
    case TEMP:
        TEMP_LOWER_UPPER_CONFIG();
        temperatureFlag = 1; 
        lowerValue = MsgToRead.param1;
        upperValue = MsgToRead.param2;
        break;
    default:
        MsgToRead.msg = NONE;
        break;
    }
    MsgToRead.msg = NONE;
}

/**
  * @brief This function prints the date, time and temperature on the LCD.
  * @param void
  * @retval None
  */
void showDateAndTime(void)
{
    char bufferLine1[25] = {0};
    char bufferLine2[25] = {0};
    char convertionBuffer[25] = {0};
    char *months[] = {"ENE,", "FEB,", "MAR,", "ABR,", "MAY,", "JUN,", "JUL,", "AGO,", "SEP,", "OCT,", "NOV,", "DIC,"};
    char *weekDays[] = {"Lu", "Ma", "Mi", "Ju", "Vi", "Sa", "Do"};
    int8_t temperature;

    temperature = getTemperature();

    joinStrings(bufferLine1, " ");
    joinStrings(bufferLine1, months[RTC_DateRead.Month - 1]);
    intToString(convertionBuffer, RTC_DateRead.Date);
    joinStrings(bufferLine1, convertionBuffer);
    joinStrings(bufferLine1, " 20");
    intToString(convertionBuffer, RTC_DateRead.Year);
    joinStrings(bufferLine1, convertionBuffer);
    joinStrings(bufferLine1, " ");
    joinStrings(bufferLine1, weekDays[RTC_DateRead.WeekDay - 1]);
   
    intToString(convertionBuffer, RTC_TimeRead.Hours);
    joinStrings(bufferLine2, convertionBuffer);
    joinStrings(bufferLine2, ":");
    intToString(convertionBuffer, RTC_TimeRead.Minutes);
    joinStrings(bufferLine2, convertionBuffer);
    joinStrings(bufferLine2, ":");
    intToString(convertionBuffer, RTC_TimeRead.Seconds);
    joinStrings(bufferLine2, convertionBuffer);
    joinStrings(bufferLine2, " ");
    intToString(convertionBuffer, temperature);
    joinStrings(bufferLine2, convertionBuffer);

    if((__HAL_RTC_ALARM_GET_IT_SOURCE(&RtcHandle, RTC_ALARM_A)==1) && (temperatureFlag == 1))
    {
        joinStrings(bufferLine2, "C  AT");
    }
    else
    {
        if(temperatureFlag == 1)
        {
            joinStrings(bufferLine2, "C   T");
        }
        else if (__HAL_RTC_ALARM_GET_IT_SOURCE(&RtcHandle, RTC_ALARM_A))
        {
            joinStrings(bufferLine2, "C  A ");
        }
        else
        {
            joinStrings(bufferLine2, "C    ");
        }
    }

    MOD_LCD_SetCursor(&LcdHandle, 0, 0);
    MOD_LCD_String(&LcdHandle, bufferLine1);
    MOD_LCD_SetCursor(&LcdHandle, 1, 0);
    MOD_LCD_String(&LcdHandle, bufferLine2);
}

/**
  * @brief This function does the alarm process printing the date and the alarm animation sequence.
  * @param void
  * @retval None
  */
void startAlarm(void)
{
    static uint8_t time = 0;

    char bufferLine2[25] = {0};
    char convertionBuffer[25] = {0};
    int8_t temperature;

    temperature = getTemperature();

    if (time % 2 == 0)
    {
        joinStrings(bufferLine2, "    ");
    }
    else
    {
        joinStrings(bufferLine2, "*** ");
    }
    intToString(convertionBuffer, RTC_TimeRead.Hours);
    joinStrings(bufferLine2, convertionBuffer);
    joinStrings(bufferLine2, ":");
    intToString(convertionBuffer, RTC_TimeRead.Minutes);
    joinStrings(bufferLine2, convertionBuffer);
    joinStrings(bufferLine2, " ");
    intToString(convertionBuffer, temperature);
    joinStrings(bufferLine2, convertionBuffer);
    if (time % 2 == 0)
    {
        joinStrings(bufferLine2, "C   ");
    }
    else
    {
        joinStrings(bufferLine2, "C***");
    }

    MOD_LCD_SetCursor(&LcdHandle, 1, 0);
    MOD_LCD_String(&LcdHandle, bufferLine2);

    time++;

    if (time > 59)
    {
        alarmStat = RESET;
        tempAlarmStat = RESET; //Bandera de alarma activa, se desactiva
        lowerValue = 0;
        upperValue = 0;
        time = 0;
    }
}

/**
  * @brief This function prints the alarm details if an alarm is on, 
  *        or prints "No alarms config" if the alarms are off.
  * @param void
  * @retval None
  */
void showAlarmDetails(void)
{
    char bufferLine2[25] = {0};
    char convertionBuffer[25] = {0};

    HAL_RTC_GetAlarm(&RtcHandle, &RTC_AlarmRead, RTC_ALARM_A, RTC_FORMAT_BIN);

    if((__HAL_RTC_ALARM_GET_IT_SOURCE(&RtcHandle, RTC_ALARM_A)==1) && (temperatureFlag == 1))
    {
        joinStrings(bufferLine2, "AT ");
        intToString(convertionBuffer, RTC_AlarmRead.AlarmTime.Hours);
        joinStrings(bufferLine2, convertionBuffer);
        joinStrings(bufferLine2, ":");
        intToString(convertionBuffer, RTC_AlarmRead.AlarmTime.Minutes);
        joinStrings(bufferLine2, convertionBuffer);
        joinStrings(bufferLine2, " ");
        intToString(convertionBuffer, lowerValue);
        joinStrings(bufferLine2, convertionBuffer);
        joinStrings(bufferLine2, "-");
        intToString(convertionBuffer, upperValue);
        joinStrings(bufferLine2, convertionBuffer);
        joinStrings(bufferLine2, "C ");
    }
    else
    {
        if(temperatureFlag == 1)
        {
            joinStrings(bufferLine2, " T       ");
            intToString(convertionBuffer, lowerValue);
            joinStrings(bufferLine2, convertionBuffer);
            joinStrings(bufferLine2, "-");
            intToString(convertionBuffer, upperValue);
            joinStrings(bufferLine2, convertionBuffer);
            joinStrings(bufferLine2, "C ");
        }
        else if (__HAL_RTC_ALARM_GET_IT_SOURCE(&RtcHandle, RTC_ALARM_A))
        {
            joinStrings(bufferLine2, "A  ");
            intToString(convertionBuffer, RTC_AlarmRead.AlarmTime.Hours);
            joinStrings(bufferLine2, convertionBuffer);
            joinStrings(bufferLine2, ":");
            intToString(convertionBuffer, RTC_AlarmRead.AlarmTime.Minutes);
            joinStrings(bufferLine2, convertionBuffer);
            joinStrings(bufferLine2, "        "); 
        }
        else
        {
            joinStrings(bufferLine2, "NO ALARMS CONFIG");
        }
    }
    

    MOD_LCD_SetCursor(&LcdHandle, 1, 0);
    MOD_LCD_String(&LcdHandle, bufferLine2);
}

/**
  * @brief This is the main function of the app_clock, it contains the state machine to be executed during the process.
  * @param void
  * @retval None
  */
void clock_task(void)
{
    static uint8_t clockState = CLK_IDLE;
    static int8_t lastSec1, lastSec2, lastSec3;

    HAL_RTC_GetTime(&RtcHandle, &RTC_TimeRead, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&RtcHandle, &RTC_DateRead, RTC_FORMAT_BIN);
    switch (clockState)
    {
    case CLK_IDLE:
        if ((RTC_TimeRead.Seconds - lastSec1) != 0)
        {
            clockState = CLK_SHOW_TIME;
            HAL_RTC_GetTime(&RtcHandle, &RTC_TimeRead, RTC_FORMAT_BIN);
            HAL_RTC_GetDate(&RtcHandle, &RTC_DateRead, RTC_FORMAT_BIN);
            lastSec1 = RTC_TimeRead.Seconds;
        }
        if (alarmStat == SET || tempAlarmStat == SET)
        {
            if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == 0)
            {
                alarmStat = RESET;
                tempAlarmStat = RESET; 
                lowerValue = 0;
                upperValue = 0;
            }
            if ((RTC_TimeRead.Seconds - lastSec2) != 0)
            {
                clockState = CLK_START_ALARM;
                HAL_RTC_GetTime(&RtcHandle, &RTC_TimeRead, RTC_FORMAT_BIN);
                HAL_RTC_GetDate(&RtcHandle, &RTC_DateRead, RTC_FORMAT_BIN);
                lastSec2 = RTC_TimeRead.Seconds;
            }
        }
        if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == 0)
        {
            if ((RTC_TimeRead.Seconds - lastSec3) != 0)
            {
                clockState = CLK_SHOW_ALARM;
                HAL_RTC_GetTime(&RtcHandle, &RTC_TimeRead, RTC_FORMAT_BIN);
                HAL_RTC_GetDate(&RtcHandle, &RTC_DateRead, RTC_FORMAT_BIN);
                lastSec3 = RTC_TimeRead.Seconds;
            }
        }
        if (HIL_QUEUE_Read(&MsgQueue, &MsgToRead) == 1u)
        {
            clockState = CLK_SET_CONFIG;
        }
        break;
    case CLK_START_ALARM:
        startAlarm();
        clockState = CLK_IDLE;
        break;
    case CLK_SET_CONFIG:
        setRtcConfiguration();
        clockState = CLK_IDLE;
        break;
    case CLK_SHOW_ALARM:
        showAlarmDetails();
        clockState = CLK_IDLE;
        break;
    case CLK_SHOW_TIME:
        showDateAndTime();
        clockState = CLK_IDLE;
        break;

    default:
        clockState = CLK_IDLE;
        break;
    }
}

/**
  * @brief This function transforms an integer value in a character string.
  * @param str pointer to a string to store the ascii values
  * @param num integer number to be convert 
  * @retval None
  */
void intToString(char *str, uint8_t num)
{
    uint8_t copiaNum, r = 0, i;

    if (num < 0)
    {
        copiaNum = num * -1;
        i = 1;
        str[0] = '-';
    }
    else
    {
        if (num < 10)
        {
            copiaNum = num;
            i = 1;
            str[0] = '0';
        }
        else
        {
            if (num % 10 == 0)
            {
                str[1] = '0';
            }
            copiaNum = num;
            i = 0;
        }
    }
    while (copiaNum != 0)
    {
        r = r * 10;
        r = r + copiaNum % 10;
        copiaNum = copiaNum / 10;
    }
    if (r == 0)
    {
        str[i] = '0';
    }
    else
    {
        for (; r > 0; i++)
        {
            str[i] = (char)((r % 10) + 48);
            r /= 10;
        }
    }
}

/**
  * @brief This function joins two different character strings into one.  
  * @param str1 pointer to a string, it will be modified by adding the other string here. 
  * @param str2 pointer to a string 
  * @retval None
  */
void joinStrings(char *str1, char *str2)
{
    uint8_t i;

    for (i = 0; str1[i] != '\0'; i++)
    {
        str1[i] = str1[i];
    }
    for (uint8_t j = 0; str2[j] != '\0'; j++, i++)
    {
        str1[i] = str2[j];
    }
    str1[i + 1] = 0;
}