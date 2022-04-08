/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.cIGULI
  * @brief          : Main program bodyadsada
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "adc_conversion_handler.h"
#include "lcd16x2_i2c.h"
#include "i2c-lcd.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DHT11_PORT GPIOB
#define DHT11_PIN GPIO_PIN_5
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim1;

/* USER CODE BEGIN PV */

/* ADC Variables */
volatile uint16_t adc_result_dma[7];
const int adc_channel_lenght = sizeof(adc_result_dma) / sizeof(adc_result_dma[0]);
int adc_result_percentage[7];
const int moist_per_line[] = {2,2};
const int adc_line_number = 2;
int adc_result_line_avg_percentage = 0;
int adc_line_avg[2];



/* Control Variable */

/* Menu Variable */
int menu_lcd_refresh = 0;
int menu_click = 0;
int menu_up = 0;
int menu_down = 0;
int next_page=0;
int current_page = 0;

/* Manual Mode Variables */
int mode_manual_start = 0;

/* Planner Mode Variables */
int planner_line_compare[2];
int planner_line_lenght = sizeof(planner_line_compare) /sizeof(planner_line_compare[0]);
int relay_set[2];
int relay_end_flag = 0;
int line_relays[] = {line0_relay_Pin, line1_relay_Pin};

/* Auto Mode Variables */
int auto_modes[2][3][9] = {{{1,20,0,0,0,1,0,0,0}, {2,30,0,1,0,0,1,0,0}, {2,60,0,1,0,0,1,0,0}},
							{{1,30,0,0,0,1,0,0,0}, {3,60,0,1,0,1,0,1,0}, {3,120,0,1,0,1,0,1,0}}};

// 0:Sapling | 1:Tree
int auto_irrigation_time[2];

/*	Index = 0:Sapling | 1:Tree
	Value = 0:Low | 1:Mid | 2:High */
int auto_mode_param[2];


/* Real Time Clock */
RTC_TimeTypeDef currTime = {0};
RTC_DateTypeDef currDate = {0};
char timeBuff[20];
char dateBuff[20];
int set_hours = 0; 		int set_minutes = 0; int set_seconds = 0;
int set_weekday = 0;  	int set_month = 0;   int set_date = 0;
int set_year = 0;


/* DHT Temp Sensor Variables */
uint8_t Rh_byte1, Rh_byte2, Temp_byte1, Temp_byte2;
uint16_t SUM, RH, TEMP;

float Temperature = 0;
float Humidity = 0;
uint8_t Presence = 0;

/* DEBUG Variables */
int yanked;
int x =0;
int a =5;
int b=8;
int testDummy_manual = 0;



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_RTC_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */

//MENU
void menu_func(int refresh, int reset);
void lcd_initial_message();

//MODES
void mode_manual(int start);
void mode_planner(int auto_mode_flag, int start);
void mode_auto();

//DELAY
void delay_us (uint16_t us);

//DHT PROTOTYPES
void DHT11_Start (void);
uint8_t DHT11_Check_Response (void);
uint8_t DHT11_Read (void);
void Set_Pin_Input (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void Set_Pin_Output (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void DHT11_get_value();

//RTC PROTOTYPES
void set_time (int set_hours, int set_minutes, int set_seconds, int set_weekday, int set_month, int set_date, int set_year);
void get_time();

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//lcd16x
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_RTC_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  //Initializes line average array


  for(int x = 0; x < 4; x ++){
		  adc_line_avg[x] = 0;
			}

  //HAL_RTCEx_SetSmoothCalib(&hrtc, RTC_SMOOTHCALIB_PERIOD_32SEC, RTC_SMOOTHCALIB_PLUSPULSES_RESET, -511);
  HAL_TIM_Base_Start(&htim1);
  lcd_initial_message();





  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */


	  DHT11_get_value();
	  //get_time();

	  	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_result_dma, adc_channel_lenght);
	  //mode_planner(0,1);
	  //mode_manual(mode_manual_start);
	  menu_func(menu_lcd_refresh, 0);
	  HAL_Delay(1000);


	  /*

      //lcd_clear ();
      lcd_send_string(timeBuff);
      lcd_put_cur(1, 0);
      lcd_send_string("");
      //lcd_clear ();
	*/




  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_TIM|RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  PeriphClkInitStruct.TIMPresSelection = RCC_TIMPRES_ACTIVATED;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 7;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = 3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = 4;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = 5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = 6;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = 7;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};
  RTC_AlarmTypeDef sAlarm = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 280;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_MARCH;
  sDate.Date = 0x4;
  sDate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enable the Alarm A
  */
  sAlarm.AlarmTime.Hours = 0x3;
  sAlarm.AlarmTime.Minutes = 0x45;
  sAlarm.AlarmTime.Seconds = 0x0;
  sAlarm.AlarmTime.SubSeconds = 0x0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 0x4;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 84-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 0xffff-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, tankmotor_relay_Pin|line1_relay_Pin|line0_relay_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(wellmotor_relay_GPIO_Port, wellmotor_relay_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : tankmotor_relay_Pin line1_relay_Pin line0_relay_Pin */
  GPIO_InitStruct.Pin = tankmotor_relay_Pin|line1_relay_Pin|line0_relay_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : light_exti_sens_Pin button_press_Pin */
  GPIO_InitStruct.Pin = light_exti_sens_Pin|button_press_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : button_up_Pin button_down_Pin ac_source_Pin */
  GPIO_InitStruct.Pin = button_up_Pin|button_down_Pin|ac_source_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : wellmotor_relay_Pin */
  GPIO_InitStruct.Pin = wellmotor_relay_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(wellmotor_relay_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : dht_in_Pin */
  GPIO_InitStruct.Pin = dht_in_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(dht_in_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

void delay_us (uint16_t us)
{
	__HAL_TIM_SET_COUNTER(&htim1,0);  // set the counter value a 0
	while (__HAL_TIM_GET_COUNTER(&htim1) < us);  // wait for the counter to reach the us input in the parameter
}


/*-----------------INTERRUPTS BEGIN-----------------------*/
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	if(hadc == &hadc1){

		for(int x =0; x<4; x++){
			adc_result_dma[x] = (adc_result_dma[x]> 2730) ? 2730:adc_result_dma[x];
			adc_result_dma[x] = (adc_result_dma[x]< 1260) ? 1260:adc_result_dma[x];
			adc_result_percentage[x] = 100-((adc_result_dma[x]-1260)*100/1470);
		}
			//Produces Line average : adc_line_avg
			int y =0;
			int per_line_offset = moist_per_line[0];
			for(int x = 0; x < adc_line_number; x++){
				adc_line_avg[x] = 0;
				for(; y < per_line_offset; y++){
					adc_line_avg[x] += adc_result_percentage[y];
				}
				adc_line_avg[x] /= moist_per_line[x];
			   per_line_offset += moist_per_line[x+1];
			}

	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	menu_lcd_refresh = 1;
    if(GPIO_Pin == button_up_Pin){
    	menu_up = 1;
    }else if(GPIO_Pin == button_down_Pin){
    	menu_down = 1;
    }else if(GPIO_Pin == button_press_Pin){
    	menu_click = 1;
    }
}

/*-----------------INTERRUPTS END-----------------------*/


/*-----------------DHT11 BEGIN--------------------------*/
void Set_Pin_Output (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void Set_Pin_Input (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void DHT11_Start (void)
{
	Set_Pin_Output (DHT11_PORT, DHT11_PIN);  // set the pin as output
	HAL_GPIO_WritePin (DHT11_PORT, DHT11_PIN, 0);   // pull the pin low
	delay_us (18000);   // wait for 18ms
    HAL_GPIO_WritePin (DHT11_PORT, DHT11_PIN, 1);   // pull the pin high
    delay_us (20);   // wait for 20us
	Set_Pin_Input(DHT11_PORT, DHT11_PIN);    // set as input
}

uint8_t DHT11_Check_Response (void)
{
	uint8_t Response = 0;
	delay_us (40);
	if (!(HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)))
	{
		delay_us (80);
		if ((HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN))) Response = 1;
		else Response = -1; // 255
	}
	while ((HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)));   // wait for the pin to go low

	return Response;
}

uint8_t DHT11_Read (void)
{
	uint8_t i,j;
	for (j=0;j<8;j++)
	{
		while (!(HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)));   // wait for the pin to go high
		delay_us (40);   // wait for 40 us
		if (!(HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)))   // if the pin is low
		{
			i&= ~(1<<(7-j));   // write 0
		}
		else i|= (1<<(7-j));  // if the pin is high, write 1
		while ((HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)));  // wait for the pin to go low
	}
	return i;
}

void DHT11_get_value(){
	DHT11_Start();
	Presence = DHT11_Check_Response();
	Rh_byte1 = DHT11_Read ();
	Rh_byte2 = DHT11_Read ();
	Temp_byte1 = DHT11_Read ();
	Temp_byte2 = DHT11_Read ();
	SUM = DHT11_Read();
	TEMP = Temp_byte1;
	RH = Rh_byte1;
	Temperature = (float) TEMP;
	Humidity = (float) RH;
}

/*-----------------DHT11 END----------------------------*/

/*-----------------MODE FUNCTIONS BEGIN-----------------*/
void mode_manual(int start){
	if(start){
		HAL_GPIO_WritePin(GPIOA, line0_relay_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, line1_relay_Pin, GPIO_PIN_SET);
		HAL_Delay(1000);
		HAL_GPIO_WritePin(GPIOA, tankmotor_relay_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, wellmotor_relay_Pin, GPIO_PIN_SET);
	}else{
		HAL_GPIO_WritePin(GPIOA, tankmotor_relay_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, wellmotor_relay_Pin, GPIO_PIN_RESET);
		HAL_Delay(1000);
		HAL_GPIO_WritePin(GPIOA, line0_relay_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, line1_relay_Pin, GPIO_PIN_RESET);
	}
}

void mode_planner(int auto_mode_flag, int start){
	if(auto_mode_flag == 0 && start == 1){
		HAL_GPIO_WritePin(GPIOA, tankmotor_relay_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, wellmotor_relay_Pin, GPIO_PIN_SET);
		for(int a = 0; a < adc_line_number; a++){
			for(int x = 0; x < moist_per_line[a]; x++){
				if(adc_line_avg[x] >= planner_line_compare[x]){
					relay_set[x] = 1;
					relay_end_flag = (relay_end_flag == 2) ? 2:relay_end_flag+1;
					HAL_GPIO_WritePin(GPIOC, line_relays[x], GPIO_PIN_SET);
				}else{
					relay_set[x] = 0;
					relay_end_flag = (relay_end_flag) ? relay_end_flag-1:0;
					HAL_GPIO_WritePin(GPIOC, line_relays[x], GPIO_PIN_RESET);
				}
			}
		}
		if(relay_end_flag == 2){
			relay_end_flag = 0;
			HAL_GPIO_WritePin(GPIOA, tankmotor_relay_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, wellmotor_relay_Pin, GPIO_PIN_RESET);
		}
	}
}

void mode_auto(int start){
	//Saat geldi mi
		//yagisli mi ya da genel toprak nemi >80

			//evet: her modu ertesi gune aktar
				//bu hafta 2. kez yagisliysa sulama yapma bu hafta

			//hayir:
				//sapling
					//hafta ortalamalari hangi moda denk geliyor
						//o modun sulama gunu bugun mu
							//evet: moda gore sula
							///hayir: skip
				//agac
					//hafta ortalamalari hangi moda denk geliyor
						//o modun sulama gunu bugun mu
							//evet: moda gore sula
							//hayir: skip

}

/*-----------------MODE FUNCTIONS END-----------------*/

void set_time (int set_hours, int set_minutes, int set_seconds, int set_weekday, int set_month, int set_date, int set_year){
  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;
  sTime.Hours = set_hours; // set hours
  sTime.Minutes = set_minutes; // set minutes
  sTime.Seconds = set_seconds; // set seconds
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK){}
  sDate.WeekDay = set_weekday;
  sDate.Month = set_month;
  sDate.Date = set_date;
  sDate.Year = set_year;
  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK){}
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x32F2); // backup register
}

void get_time(){
	HAL_RTC_GetTime(&hrtc, &currTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &currDate, RTC_FORMAT_BIN);
	sprintf(timeBuff,"%d,%d.%d", currTime.Hours, currTime.Minutes, currTime.Seconds);
	sprintf(dateBuff,"%02d-%02d-%2d",currDate.Date, currDate.Month, 2000 + currDate.Year);
}

/*-----------------MENU BEGIN-------------------------*/
void lcd_initial_message(){
	lcd_init ();
	lcd_send_string ("ECODROP");
	HAL_Delay(500);
	lcd_put_cur(1, 0);
	lcd_send_string("Irrigation");
	HAL_Delay(1000);
	lcd_send_cmd (0x01);
	HAL_Delay(100);
	lcd_init ();
	lcd_send_string ("> Manual Mode");
	lcd_put_cur(1, 0);
	lcd_send_string("  Planner Mode  ");

}

void menu_func(int refresh, int reset){
	if(refresh){
		current_page = (reset) ? 0:current_page;
		lcd_clear();
		switch(current_page){
			case 0:
				if(menu_click){
					current_page = 4;
					menu_click = 0;
					menu_func(1, 0);
					break;
				  }else if(menu_down){
					 current_page = 1;
					 menu_down=0;
					 menu_func(1, 0);
					 break;
				  }else if(menu_up){
					 current_page = 3;
					 menu_up =0;
					 menu_func(1, 0);
					 break;
				  }
				lcd_init ();
				lcd_send_string ("> Manual Mode");
				lcd_put_cur(1, 0);
				lcd_send_string("  Planner Mode  ");
				break;

			case 1:
				if(menu_click){
					current_page = 7;
					menu_click = 0;
					menu_func(1, 0);
					break;
				}else if(menu_down){
					current_page = 2;
					menu_down =0;
					menu_func(1, 0);
					break;
				}else if(menu_up){
					current_page = 0;
					menu_up =0;
					menu_func(1, 0);
					break;
				}
				lcd_init ();
				lcd_send_string ("> Planner Mode  ");
				lcd_put_cur(1, 0);
				lcd_send_string("  Auto Mode");
				break;

			case 2:
				if(menu_click){
					current_page = 14;
					menu_click = 0;
					menu_func(1, 0);
					break;
				}else if(menu_down){
					current_page = 3;
					menu_down =0;
					menu_func(1, 0);
					break;
				}else if(menu_up){
					current_page = 1;
					menu_up =0;
					menu_func(1, 0);
					break;
				}
				lcd_init ();
				lcd_send_string ("> Auto Mode");
				lcd_put_cur(1, 0);
				lcd_send_string("  Settings");
				break;

			case 3:
				if(menu_click){
					//Manual Sub Menu
					current_page = 15;
					menu_click = 0;
					menu_func(1, 0);
					break;
				}else if(menu_down){
					current_page = 0;
					menu_down = 0;
					menu_func(1, 0);
					break;
				}else if(menu_up){
					current_page = 2;
					menu_up = 0;
					menu_func(1, 0);
					break;
				}
				lcd_init ();
				lcd_send_string ("> Settings");
				lcd_put_cur(1, 0);
				lcd_send_string("  ");
				break;

			case 4:
				if(menu_click){
					current_page = 6;
					menu_click = 0;
					menu_func(1, 0);
					break;
				}else if(menu_down){
					current_page = 5;
					menu_down = 0;
					menu_func(1, 0);
					break;
				}else if(menu_up){
					current_page = 5;
					menu_up = 0;
					menu_func(1, 0);
					break;
				}
				lcd_init ();
				lcd_send_string ("> Press Start");
				lcd_put_cur(1, 0);
				lcd_send_string(" Back");
				break;

			case 5:
				if(menu_click){
					current_page = 0;
					menu_click = 0;
					menu_func(1, 0);
					break;
				}else if(menu_down){
					current_page = 4;
					menu_down = 0;
					menu_func(1, 0);
					break;
				}else if(menu_up){
					current_page = 4;
					menu_up = 0;
					menu_func(1, 0);
					break;
				}
				lcd_init ();
				lcd_send_string ("> Back");
				lcd_put_cur(1, 0);
				lcd_send_string("");
				break;

			case 6:
				if(menu_click){
					current_page = 4;
					menu_click = 0;
					menu_func(1, 0);
				}else{
					current_page = 6;
					menu_up = 0;
					menu_down = 0;
					lcd_init ();
					lcd_send_string ("Watering");
					lcd_put_cur(1, 0);
					lcd_send_string("Now");
				}
				break;

			case 7:
				if(menu_click){
					current_page = 10;
					menu_click = 0;
					menu_func(1, 0);
					break;
				}else if(menu_down){
					current_page = 8;
					menu_down = 0;
					menu_func(1, 0);
					break;
				}else if(menu_up){
					current_page = 13;
					menu_up = 0;
					menu_func(1, 0);
					break;
				}
				lcd_init ();
				lcd_send_string ("> Line 1");
				lcd_put_cur(1, 0);
				lcd_send_string("  Line 2");
				break;

			case 8:
				if(menu_click){
					current_page = 10;
					menu_click = 0;
					menu_func(1, 0);
					break;
				}else if(menu_down){
					current_page = 9;
					menu_down = 0;
					menu_func(1, 0);
					break;
				}else if(menu_up){
					current_page = 7;
					menu_up = 0;
					menu_func(1, 0);
					break;
				}
				lcd_init ();
				lcd_send_string ("> Line 2");
				lcd_put_cur(1, 0);
				lcd_send_string("  Time");
				break;

			case 9:
				if(menu_click){
					current_page = 11;
					menu_click = 0;
					menu_func(1, 0);
					break;
				}else if(menu_down){
					current_page = 12;
					menu_down = 0;
					menu_func(1, 0);
					break;
				}else if(menu_up){
					current_page = 8;
					menu_up = 0;
					menu_func(1, 0);
					break;
				}
				lcd_init ();
				lcd_send_string ("> Time");
				lcd_put_cur(1, 0);
				lcd_send_string("  Start");
				break;

			case 10:
				if(menu_click){
					current_page = 7;
					menu_click = 0;
					menu_func(1, 0);
					break;
				}else{
					current_page = 10;
					lcd_init ();
					lcd_send_string ("> 20");
					lcd_put_cur(1, 0);
					lcd_send_string("  ");
				}
				break;

			case 11:
				if(menu_click){
					current_page = 9;
					menu_click = 0;
					menu_func(1, 0);
					break;
				}else{
					current_page = 11;
					lcd_init ();
					lcd_send_string ("> Saat Loopu");
					lcd_put_cur(1, 0);
					lcd_send_string("  ");
				}
				break;

			case 12:
				if(menu_click){
					current_page = 1;
					menu_click = 0;
					menu_func(1, 0);
					break;
				}else if(menu_down){
					current_page = 13;
					menu_down = 0;
					menu_func(1, 0);
					break;
				}else if(menu_up){
					current_page = 9;
					menu_up = 0;
					menu_func(1, 0);
					break;
				}
				lcd_init ();
				lcd_send_string ("> Start");
				lcd_put_cur(1, 0);
				lcd_send_string("  Back");
				break;

			case 13:
				if(menu_click){
					current_page = 1;
					menu_click = 0;
					menu_func(1, 0);
					break;
				}else if(menu_down){
					current_page = 7;
					menu_down = 0;
					menu_func(1, 0);
					break;
				}else if(menu_up){
					current_page = 12;
					menu_up = 0;
					menu_func(1, 0);
					break;
				}
				lcd_init ();
				lcd_send_string ("> Back");
				lcd_put_cur(1, 0);
				lcd_send_string(" ");
				break;

			case 14:
				if(menu_click){
					current_page = 2;
					menu_click = 0;
					menu_func(1, 0);
					break;
				}else{
					current_page = 14;
					lcd_init ();
					lcd_send_string ("> INFO DISP");
					lcd_put_cur(1, 0);
					lcd_send_string("  ");
				}
				break;

			case 15:
				if(menu_click){
					current_page = 11;
					menu_click = 0;
					menu_func(1, 0);
					break;
				}else if(menu_down){
					current_page = 16;
					menu_down = 0;
					menu_func(1, 0);
					break;
				}else if(menu_up){
					current_page = 17;
					menu_up = 0;
					menu_func(1, 0);
					break;
				}
				lcd_init ();
				lcd_send_string ("> Time S.");
				lcd_put_cur(1, 0);
				lcd_send_string("  Bluetooth");
				break;

			case 16:
				if(menu_click){
					current_page = 11;
					menu_click = 0;
					menu_func(1, 0);
					break;
				}else if(menu_down){
					current_page = 17;
					menu_down = 0;
					menu_func(1, 0);
					break;
				}else if(menu_up){
					current_page = 15;
					menu_up = 0;
					menu_func(1, 0);
					break;
				}
				lcd_init ();
				lcd_send_string ("> Bluetooth");
				lcd_put_cur(1, 0);
				lcd_send_string("  Back");
				break;
			case 17:
				if(menu_click){
					current_page = 3;
					menu_click = 0;
					menu_func(1, 0);
					break;
				}else if(menu_down){
					current_page = 15;
					menu_down = 0;
					menu_func(1, 0);
					break;
				}else if(menu_up){
					current_page = 16;
					menu_up = 0;
					menu_func(1, 0);
					break;
				}
				lcd_init ();
				lcd_send_string ("> Back");
				lcd_put_cur(1, 0);
				lcd_send_string("  ");
				break;
		}
		menu_lcd_refresh = 0;
		menu_click= 0;
		menu_up = 0;
		menu_down = 0;
	}
}

/*-----------------MENU END--------------------------*/

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
