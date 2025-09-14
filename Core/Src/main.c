/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usb.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
// #include "usbd_def.h"
#include "usb_device.h"
#include "usbd_hid_keyboard.h"
#include "usbd_cdc_acm_if.h"
#include "usb_hid_keys.h"
// #include "usbd_composite.h"
#include <eeprom.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
#define DEBUG_SENSORS 0

uint16_t VirtAddVarTab[NB_OF_VAR];
uint16_t VarIndex, VarDataTmp = 0;

extern USBD_HandleTypeDef hUsbDevice;

uint8_t volatile keyBoardHIDsub[11] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

uint16_t adc_results_1[4];
uint16_t adc_results_2[4];
uint16_t adc_results_3[4];
uint16_t adc_results_4[4];

uint16_t adc_results_1_buf1[4];
uint16_t adc_results_2_buf1[4];
uint16_t adc_results_3_buf1[4];
uint16_t adc_results_4_buf1[4];

uint16_t adc_results_1_buf2[4];
uint16_t adc_results_2_buf2[4];
uint16_t adc_results_3_buf2[4];
uint16_t adc_results_4_buf2[4];

uint16_t adc_results_1_buf3[4];
uint16_t adc_results_2_buf3[4];
uint16_t adc_results_3_buf3[4];
uint16_t adc_results_4_buf3[4];

uint8_t volatile ground_id = 1;

uint16_t sensor_treshholds[16] = {
	750, 750, 750, 750,
	750, 750, 750, 750,
	750, 750, 750, 750,
	750, 750, 750, 750};

uint16_t sensor_offsets[16] = {
	4000, 4000, 4000, 4000,
	4000, 4000, 4000, 4000,
	4000, 4000, 4000, 4000,
	4000, 4000, 4000, 4000};

// uint8_t sensors_binding[40]={
//		0,0,0,0,1,1,1,1,2,2,
//		2,2,3,3,3,3,4,4,4,4,
//		5,5,5,5,6,6,6,6,7,7,
//		7,7,8,8,8,8,3,3,5,5
// };

//P1


//uint8_t key_map[9] = {
//	KEY_Q, KEY_W, KEY_E,
//	KEY_A, KEY_S, KEY_D,
//	KEY_Z, KEY_X, KEY_C};
//
//uint8_t sensors_binding[40] = {
//	5,5, 7, 4, 8, 1, 2, 6, 0, 3,
//	5,5, 7, 4, 8, 1, 2, 6, 0, 3,
//	3,5, 7, 4, 8, 1, 2, 6, 0, 3,
//	3,5, 7, 4, 8, 1, 2, 6, 0, 3};

//P2

uint8_t key_map[9] = {
	KEY_R, KEY_T, KEY_Y,
	KEY_F, KEY_G, KEY_H,
	KEY_V, KEY_B, KEY_N};


uint8_t sensors_binding[16] = {
	5,5, 7, 4,
	5,5, 7, 4,
	3,5, 7, 4,
	3,5, 7, 4};


uint16_t raw;
char msg[10];
char msg2[255];
uint16_t s;
char *token;
uint16_t num1, num2,sensor_id;

uint8_t rx_buff[255];
uint8_t volatile rx_buff_flag = 0;
uint16_t volatile debug_var = 0;

uint8_t key_states[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};




static GPIO_InitTypeDef Output_1_in;
static GPIO_InitTypeDef Output_1_out;
static GPIO_InitTypeDef Output_2_in;
static GPIO_InitTypeDef Output_2_out;
static GPIO_InitTypeDef Output_3_in;
static GPIO_InitTypeDef Output_3_out;
static GPIO_InitTypeDef Output_4_in;
static GPIO_InitTypeDef Output_4_out;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	//	CDC_Transmit(0,"gowno", sizeof("gowno"));
	//	ground_id=69;
	//	debug_var++;
	HAL_ADC_Stop_DMA(&hadc1);
	switch (ground_id)
	{
	case 1:
		HAL_GPIO_Init(Output_1_GPIO_Port, &Output_1_in);
		HAL_GPIO_Init(Output_2_GPIO_Port, &Output_2_out);
		ground_id = 2;
		for (int i = 0; i < 10; ++i)
		{
			adc_results_1_buf3[i] = adc_results_1_buf2[i];
			adc_results_1_buf2[i] = adc_results_1_buf1[i];
			adc_results_1_buf1[i] = adc_results_1[i];
		}
		HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&adc_results_2, 10);
		break;
	case 2:
		HAL_GPIO_Init(Output_2_GPIO_Port, &Output_2_in);
		HAL_GPIO_Init(Output_3_GPIO_Port, &Output_3_out);
		ground_id = 3;
		for (int i = 0; i < 10; ++i)
		{
			adc_results_2_buf3[i] = adc_results_2_buf2[i];
			adc_results_2_buf2[i] = adc_results_2_buf1[i];
			adc_results_2_buf1[i] = adc_results_2[i];
		}
		HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&adc_results_3, 10);
		break;
	case 3:
		HAL_GPIO_Init(Output_3_GPIO_Port, &Output_3_in);
		HAL_GPIO_Init(Output_4_GPIO_Port, &Output_4_out);
		ground_id = 4;
		for (int i = 0; i < 10; ++i)
		{
			adc_results_3_buf3[i] = adc_results_3_buf2[i];
			adc_results_3_buf2[i] = adc_results_3_buf1[i];
			adc_results_3_buf1[i] = adc_results_3[i];
		}
		HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&adc_results_4, 10);
		break;
	case 4:
		HAL_GPIO_Init(Output_4_GPIO_Port, &Output_4_in);
		HAL_GPIO_Init(Output_1_GPIO_Port, &Output_1_out);
		ground_id = 1;
		for (int i = 0; i < 10; ++i)
		{
			adc_results_4_buf3[i] = adc_results_4_buf2[i];
			adc_results_4_buf2[i] = adc_results_4_buf1[i];
			adc_results_4_buf1[i] = adc_results_4[i];
		}
		HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&adc_results_1, 10);
		break;
	}
}

uint16_t get_sensor_avg_1(uint8_t i){
return (adc_results_1_buf2[i] + adc_results_1_buf1[i]+adc_results_1_buf3[i])/3;
}
uint16_t get_sensor_avg_2(uint8_t i){
return (adc_results_2_buf2[i] + adc_results_2_buf1[i]+adc_results_2_buf3[i])/3;
}
uint16_t get_sensor_avg_3(uint8_t i){
return (adc_results_3_buf2[i] + adc_results_3_buf1[i]+adc_results_3_buf3[i])/3;
}
uint16_t get_sensor_avg_4(uint8_t i){
return (adc_results_4_buf2[i] + adc_results_4_buf1[i]+adc_results_4_buf3[i])/3;
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	//	if(DEBUG_SENSORS==1){
	//		s = 0;
	//		s = sprintf(msg2, " ");
	//
	//		for (int i = 0; i < 10; ++i)
	//		{
	//			for (int j = 0; j < 4; ++j)
	//			{
	//				switch (j)
	//				{
	//				case 0:
	//					s += sprintf(msg2 + s, " %d",  adc_results_1[i]);
	//					break;
	//				case 1:
	//					s += sprintf(msg2 + s, " %d", adc_results_2[i]);
	//					break;
	//				case 2:
	//					s += sprintf(msg2 + s, " %d", adc_results_3[i]);
	//					break;
	//				case 3:
	//					s += sprintf(msg2 + s, " %d", adc_results_4[i]);
	//					break;
	//				}
	//			}
	//			s += sprintf(msg2 + s, "\n");
	//		}
	//
	//		s += sprintf(msg2 + s, "\n");
	//
	//		CDC_Transmit(0, msg2, s);
	//	}
	if (htim->Instance == TIM2)
	{
		for (int i = 0; i < 9; i++)
		{
			key_states[i] = 0;
		}

		for (int i = 0; i < 10; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				switch (j)
				{
				case 0:
					if (sensor_treshholds[i + j * 10] < (((sensor_offsets[i + j * 10] * 1024) / get_sensor_avg_1(i)) - 1024))
					{
						key_states[sensors_binding[i + j * 10]] = 1;
					}
					break;
				case 1:
					if (sensor_treshholds[i + j * 10] < (((sensor_offsets[i + j * 10] * 1024) / get_sensor_avg_2(i)) - 1024))
					{
						key_states[sensors_binding[i + j * 10]] = 1;
					}
					break;
				case 2:
					if (sensor_treshholds[i + j * 10] < (((sensor_offsets[i + j * 10] * 1024) / get_sensor_avg_3(i)) - 1024))
					{
						key_states[sensors_binding[i + j * 10]] = 1;
					}
					break;
				case 3:
					if (sensor_treshholds[i + j * 10] < (((sensor_offsets[i + j * 10] * 1024) / get_sensor_avg_4(i)) - 1024))
					{
						key_states[sensors_binding[i + j * 10]] = 1;
					}
					break;
				}
			}
		}

		//		for (int i = 0; i < 40; ++i)
		//		{
		//			key_states[sensors_binding[i]] = 0;
		//			switch (i % 4)
		//			{
		//			case 0:
		//				if (sensor_treshholds[i] > ((sensor_offsets[i] * 1024) / adc_results_1[i % 4]) - 1024)
		//				{
		//					key_states[sensors_binding[i]] = 1;
		//				}
		//				break;
		//			case 1:
		//				if (sensor_treshholds[i] > ((sensor_offsets[i] * 1024) / adc_results_2[i % 4]) - 1024)
		//				{
		//					key_states[sensors_binding[i]] = 1;
		//				}
		//				break;
		//			case 2:
		//				if (sensor_treshholds[i] > ((sensor_offsets[i] * 1024) / adc_results_3[i % 4]) - 1024)
		//				{
		//					key_states[sensors_binding[i]] = 1;
		//				}
		//				break;
		//			case 3:
		//				if (sensor_treshholds[i] > ((sensor_offsets[i] * 1024) / adc_results_4[i % 4]) - 1024)
		//				{
		//					key_states[sensors_binding[i]] = 1;
		//				}
		//				break;
		//			}
		//		}

		for (int i = 0; i < 9; ++i)
		{
			if (key_states[i] == 1)
			{
				keyBoardHIDsub[i + 2] = key_map[i];
			}
			else
			{
				keyBoardHIDsub[i + 2] = 0x00;
			}
		}
		//		keyBoardHIDsub[3] = key_map[2];
		//		s = 0;
		//		s = sprintf(msg2, "t");
		//
		//		for (int i = 0; i < 9; ++i)
		//		{
		//
		//				s += sprintf(msg2 + s, " %d", key_states[i]);
		//
		//		}
		//		s += sprintf(msg2 + s, "\n");
		//
		//		CDC_Transmit(0, msg2, s);

		USBD_HID_Keybaord_SendReport(&hUsbDevice, (uint8_t *)&keyBoardHIDsub, sizeof(keyBoardHIDsub));
	}
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* USER CODE BEGIN 1 */

	//	int volatile jajco = 1;
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
	MX_USB_PCD_Init();
	MX_TIM2_Init();
	/* USER CODE BEGIN 2 */


	Output_1_in.Pin = Output_1_Pin;
	Output_1_in.Mode = GPIO_MODE_INPUT;
	Output_1_in.Pull = GPIO_NOPULL;
	Output_1_in.Speed = GPIO_SPEED_FREQ_LOW;
	Output_1_out.Pin = Output_1_Pin;
	Output_1_out.Mode = GPIO_MODE_OUTPUT_PP;
	Output_1_out.Pull = GPIO_NOPULL;
	Output_1_out.Speed = GPIO_SPEED_FREQ_LOW;
	Output_2_in.Pin = Output_2_Pin;
	Output_2_in.Mode = GPIO_MODE_INPUT;
	Output_2_in.Pull = GPIO_NOPULL;
	Output_2_in.Speed = GPIO_SPEED_FREQ_LOW;
	Output_2_out.Pin = Output_2_Pin;
	Output_2_out.Mode = GPIO_MODE_OUTPUT_PP;
	Output_2_out.Pull = GPIO_NOPULL;
	Output_2_out.Speed = GPIO_SPEED_FREQ_LOW;
	Output_3_in.Pin = Output_3_Pin;
	Output_3_in.Mode = GPIO_MODE_INPUT;
	Output_3_in.Pull = GPIO_NOPULL;
	Output_3_in.Speed = GPIO_SPEED_FREQ_LOW;
	Output_3_out.Pin = Output_3_Pin;
	Output_3_out.Mode = GPIO_MODE_OUTPUT_PP;
	Output_3_out.Pull = GPIO_NOPULL;
	Output_3_out.Speed = GPIO_SPEED_FREQ_LOW;
	Output_4_in.Pin = Output_4_Pin;
	Output_4_in.Mode = GPIO_MODE_INPUT;
	Output_4_in.Pull = GPIO_NOPULL;
	Output_4_in.Speed = GPIO_SPEED_FREQ_LOW;
	Output_4_out.Pin = Output_4_Pin;
	Output_4_out.Mode = GPIO_MODE_OUTPUT_PP;
	Output_4_out.Pull = GPIO_NOPULL;
	Output_4_out.Speed = GPIO_SPEED_FREQ_LOW;

	MX_USB_DEVICE_Init();
	HAL_GPIO_Init(GPIOB, &Output_1_out);
	HAL_GPIO_Init(GPIOB, &Output_2_out);
	HAL_GPIO_Init(GPIOB, &Output_3_out);
	HAL_GPIO_Init(GPIOB, &Output_4_out);
	//	HAL_Delay(1000);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&adc_results_1, 10);
	HAL_TIM_Base_Start_IT(&htim2);
	//	HAL_ADCEx_Calibration_Start(&hadc1);

	HAL_FLASH_Unlock();

	/* EEPROM Init */
	if (EE_Init() != HAL_OK)
	{
		Error_Handler();
	}

	for (VarIndex = 1; VarIndex <= NB_OF_VAR; VarIndex++)
	{
		VirtAddVarTab[VarIndex - 1] = VarIndex;
	}

	for (VarIndex = 0; VarIndex < NB_OF_VAR; VarIndex++)
	{
		if ((EE_ReadVariable(VirtAddVarTab[VarIndex], &sensor_treshholds[VarIndex])) == 1)
		{

			if ((EE_WriteVariable(VirtAddVarTab[VarIndex], sensor_treshholds[VarIndex])) != HAL_OK)
			{
				Error_Handler();
			}
		}
	}
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{

//		s = 0;
//		s += sprintf(msg2 + s, " %d", rx_buff_flag);
//		CDC_Transmit(0, (uint8_t *)msg2, s);

		if (rx_buff_flag == 1)
		{
			rx_buff_flag = 0;
			//		CDC_Receive(0, &rx_buff, 255);

			switch (rx_buff[0])
			{
			case 'o':
			case 'O':
				for (int i = 0; i < 10; ++i)
				{
					for (int j = 0; j < 4; ++j)
					{
						switch (j)
						{
						case 0:
							sensor_offsets[i + j * 10] = adc_results_1[i];
							break;
						case 1:
							sensor_offsets[i + j * 10] = adc_results_2[i];
							break;
						case 2:
							sensor_offsets[i + j * 10] = adc_results_3[i];
							break;
						case 3:
							sensor_offsets[i + j * 10] = adc_results_4[i];
							break;
						}
					}
				}
				break;
			case 'v':
			case 'V':
				s = 0;
				s = sprintf(msg2, "v");

				for (int i = 0; i < 10; ++i)
				{
					for (int j = 0; j < 4; ++j)
					{
						switch (j)
						{
						case 0:
							s += sprintf(msg2 + s, " %d", ((sensor_offsets[i + j * 10] * 1024) / get_sensor_avg_1(i)) - 1024);
							break;
						case 1:
							s += sprintf(msg2 + s, " %d", ((sensor_offsets[i + j * 10] * 1024) / get_sensor_avg_2(i)) - 1024);
							break;
						case 2:
							s += sprintf(msg2 + s, " %d", ((sensor_offsets[i + j * 10] * 1024) / get_sensor_avg_3(i)) - 1024);
							break;
						case 3:
							s += sprintf(msg2 + s, " %d", ((sensor_offsets[i + j * 10] * 1024) / get_sensor_avg_4(i)) - 1024);
							break;
						}
					}
				}

				s += sprintf(msg2 + s, "\n");

				CDC_Transmit(0, (uint8_t *)msg2, s);
				break;
			case 't':
			case 'T':
				s = 0;
				s = sprintf(msg2, "t");

				for (int i = 0; i < 10; ++i)
				{
					for (int j = 0; j < 4; ++j)
					{
						s += sprintf(msg2 + s, " %d", sensor_treshholds[i + j * 10]);
					}
				}
				s += sprintf(msg2 + s, "\n");

				CDC_Transmit(0, (uint8_t *)msg2, s);
				break;

			case '0' ... '9': // Case ranges are non-standard but work in gcc

				sscanf((char *)rx_buff, "%hu %hu", &num1, &num2);
				sprintf(msg, "%d", num1);
				CDC_Transmit(0, (uint8_t *)msg, sizeof(msg));

				if (num1 < 40 && num2 > 0 && num2 < 1023)
				{
					//				sensor_treshholds[(num1*10)%40 + num1/4] = num2;
					sensor_id = num1 % 4 * 10 + num1 / 4;
					sensor_treshholds[sensor_id] = num2;
					s = 0;
					s = sprintf(msg2, "t");
//					s += sprintf(msg2 + s, " %d", sensor_id);

					for (int i = 0; i < 10; ++i)
					{
						for (int j = 0; j < 4; ++j)
						{
							s += sprintf(msg2 + s, " %d", sensor_treshholds[i + j * 10]);
						}
					}
					s += sprintf(msg2 + s, "\n");


					if ((EE_WriteVariable(VirtAddVarTab[sensor_id], sensor_treshholds[sensor_id])) != HAL_OK)
					{
						Error_Handler();
					}

					CDC_Transmit(0, (uint8_t *)msg2, s);
				}

				break;
			default:
				break;
			}
		}
//		HAL_Delay(1);


		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
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
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	{
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC | RCC_PERIPHCLK_USB;
	PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV4;
	PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

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

#ifdef USE_FULL_ASSERT
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
