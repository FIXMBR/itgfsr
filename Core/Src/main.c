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
//#include "usbd_def.h"
#include "usb_device.h"
#include "usbd_hid_keyboard.h"
#include "usbd_cdc_acm.h"
#include "usb_hid_keys.h"
//#include "usbd_composite.h"
//#include <eeprom.h>
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
extern USBD_HandleTypeDef hUsbDevice;

typedef struct
{
	uint8_t MODIFIER;
	uint8_t RESERVED;
	uint8_t KEYCODE1;
	uint8_t KEYCODE2;
	uint8_t KEYCODE3;
	uint8_t KEYCODE4;
	uint8_t KEYCODE5;
	uint8_t KEYCODE6;
	//	uint8_t KEYCODE7;
	//	uint8_t KEYCODE8;
	//	uint8_t KEYCODE9;
} subKeyBoard;

// subKeyBoard keyBoardHIDsub = {0,0,0,0,0,0,0,0};//,0,0,0};
uint8_t volatile keyBoardHIDsub[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00};//, 0, 0 }; //,0,0,0};
// subKeyBoard keyBoardHIDsub = {0,0,0,0,0,0,0,0,0,0,0};

uint16_t  adc_results_1[10];
uint16_t  adc_results_2[10];
uint16_t  adc_results_3[10];
uint16_t  adc_results_4[10];
uint8_t volatile ground_id=1;

uint16_t sensor_treshholds[40]={
		3000,2750,2750,2750,2750,2750,2750,2750,2750,2750,
		2750,2750,2750,2750,2750,2750,2750,2750,2750,2750,
		2750,2750,2750,2750,2750,2750,2750,2750,2750,2750,
		2750,2750,2750,2750,2750,2750,2750,2750,2750,2750
};

uint16_t raw;
char msg[10];
uint16_t volatile debug_var=0;

//uint8_t sensors_groups[40]={
//		0,1,2,3,4,5,6,7,8,9,
//		0,1,2,3,4,5,6,7,8,9,
//		0,1,2,3,4,5,6,7,8,9,
//		0,1,2,3,4,5,6,7,8,9
//};

uint8_t sensors_states[10]={0,0,0,0,0,0,0,0,0,0};

uint8_t sensors_keymap[10]={
		KEY_Q,KEY_W,KEY_E,
		KEY_A,KEY_S,KEY_D,
		KEY_Z,KEY_X,KEY_C,
		KEY_1};


static GPIO_InitTypeDef Output_1_in;
static GPIO_InitTypeDef Output_1_out;
static GPIO_InitTypeDef Output_2_in ;
static GPIO_InitTypeDef Output_2_out;
static GPIO_InitTypeDef Output_3_in;
static GPIO_InitTypeDef Output_3_out;
static GPIO_InitTypeDef Output_4_in ;
static GPIO_InitTypeDef Output_4_out;


//static GPIO_InitTypeDef Output_1_out={
//		.Pin = Output_1_Pin,
//		.Mode = GPIO_MODE_OUTPUT_PP,
//		.Pull = GPIO_NOPULL,
//};
//static GPIO_InitTypeDef Output_2_in={
//		.Pin = Output_2_Pin,
//		.Mode = GPIO_MODE_INPUT,
//		.Pull = GPIO_NOPULL,
//};
//
//static GPIO_InitTypeDef Output_2_out={
//		.Pin = Output_2_Pin,
//		.Mode = GPIO_MODE_OUTPUT_PP,
//		.Pull = GPIO_NOPULL,
//};
//static GPIO_InitTypeDef Output_3_in={
//		.Pin = Output_3_Pin,
//		.Mode = GPIO_MODE_INPUT,
//		.Pull = GPIO_NOPULL,
//};
//
//static GPIO_InitTypeDef Output_3_out={
//		.Pin = Output_3_Pin,
//		.Mode = GPIO_MODE_OUTPUT_PP,
//		.Pull = GPIO_NOPULL,
//};
//static GPIO_InitTypeDef Output_4_in={
//		.Pin = Output_4_Pin,
//		.Mode = GPIO_MODE_INPUT,
//		.Pull = GPIO_NOPULL,
//};
//
//static GPIO_InitTypeDef Output_4_out={
//		.Pin = Output_4_Pin,
//		.Mode = GPIO_MODE_OUTPUT_PP,
//		.Pull = GPIO_NOPULL,
//};

//static GPIO_InitTypeDef Output_1_in;
//Output_1_in.Pin;
//Output_1_in.Pull = GPIO_NOPULL;
//Output_1_in.Mode = GPIO_MODE_INPUT;


//Output_1_Pin



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	//	CDC_Transmit(0,"gowno", sizeof("gowno"));
	//	ground_id=69;
	debug_var++;
	HAL_ADC_Stop_DMA(&hadc1);
	switch (ground_id) {
	case 1:
		HAL_GPIO_Init(Output_1_GPIO_Port, &Output_1_in);
		HAL_GPIO_Init(Output_2_GPIO_Port, &Output_2_out);
		ground_id=2;
		HAL_ADC_Start_DMA(&hadc1, &adc_results_2, 10);
		break;
	case 2:
		HAL_GPIO_Init(Output_2_GPIO_Port, &Output_2_in);
		HAL_GPIO_Init(Output_3_GPIO_Port, &Output_3_out);
		ground_id=3;
		HAL_ADC_Start_DMA(&hadc1, &adc_results_3, 10);
		break;
	case 3:
		HAL_GPIO_Init(Output_3_GPIO_Port, &Output_3_in);
		HAL_GPIO_Init(Output_4_GPIO_Port, &Output_4_out);
		ground_id=4;
		HAL_ADC_Start_DMA(&hadc1, &adc_results_4, 10);
		break;
	case 4:
		HAL_GPIO_Init(Output_4_GPIO_Port, &Output_4_in);
		HAL_GPIO_Init(Output_1_GPIO_Port, &Output_1_out);
		ground_id=1;
		HAL_ADC_Start_DMA(&hadc1, &adc_results_1, 10);
		break;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if (htim->Instance == TIM2){
		for (int i = 0; i < 9; ++i) {
			sensors_states[i]=0;
			for (int j = 0; j < 4; ++j) {
				switch (j) {
				case 0:
					//					sprintf(msg, "%hu ", adc_results_1[j]);
					//					CDC_Transmit(0,msg, sizeof(msg));
					if(sensor_treshholds[i+j*10]>adc_results_1[i]){
						sensors_states[i]=1;
						//						sprintf(msg, "%hu ", adc_results_1[j]);
						//											CDC_Transmit(0,msg, sizeof(msg));
					}
					break;
				case 1:
					//					sprintf(msg, "%hu ", adc_results_2[j]);
					//					CDC_Transmit(0,msg, sizeof(msg));
					if(sensor_treshholds[i+j*10]>adc_results_2[i]){
						sensors_states[i]=1;
						//						sprintf(msg, "%hu ", adc_results_2[j]);
						//											CDC_Transmit(0,msg, sizeof(msg));
					}

					break;
				case 2:
					//					sprintf(msg, "%hu ", adc_results_3[j]);
					//					CDC_Transmit(0,msg, sizeof(msg));
					if(sensor_treshholds[i+j*10]>adc_results_3[i]){
						sensors_states[i]=1;
						//											sprintf(msg, "%hu ", adc_results_3[j]);
						//											CDC_Transmit(0,msg, sizeof(msg));
					}
					break;
				case 3:
					//					sprintf(msg, "%hu ", adc_results_4[j]);
					//					CDC_Transmit(0,msg, sizeof(msg));
					if(sensor_treshholds[i+j*10]>adc_results_4[i]){
						sensors_states[i]=1;
						//						sprintf(msg, "%hu ", adc_results_4[j]);
						//											CDC_Transmit(0,msg, sizeof(msg));
					}
					break;
				}

			}
			//			CDC_Transmit(0,"kupka", sizeof("kupka"));
		}

		//		for (int i = 0; i < 9; ++i) {
		//			keyBoardHIDsub[i]=0x00;
		//			if(sensors_states[i]==1){
		//				for (int j = 2; j < 8; ++j) {
		//					if(keyBoardHIDsub[j]==0x00){
		//						keyBoardHIDsub[j]=sensors_keymap[i];
		//					}
		//				}
		//			}
		//		}

		if(sensors_states[0]==1){
//			keyBoardHIDsub[2]=sensors_keymap[0];
			keyBoardHIDsub[2]=0x05;
		}else{
			keyBoardHIDsub[2]=0x00;
		}


		USBD_HID_Keybaord_SendReport(&hUsbDevice, &keyBoardHIDsub, sizeof(keyBoardHIDsub));
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

	int volatile jajco = 1;
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
	Output_1_in.Pin=Output_1_Pin;
	Output_1_in.Mode=GPIO_MODE_INPUT;
	Output_1_in.Pull=GPIO_NOPULL;
	Output_1_in.Speed = GPIO_SPEED_FREQ_LOW;
	Output_1_out.Pin=Output_1_Pin;
	Output_1_out.Mode=GPIO_MODE_OUTPUT_PP;
	Output_1_out.Pull=GPIO_NOPULL;
	Output_1_out.Speed = GPIO_SPEED_FREQ_LOW;
	Output_2_in.Pin=Output_2_Pin;
	Output_2_in.Mode=GPIO_MODE_INPUT;
	Output_2_in.Pull=GPIO_NOPULL;
	Output_2_in.Speed = GPIO_SPEED_FREQ_LOW;
	Output_2_out.Pin=Output_2_Pin;
	Output_2_out.Mode=GPIO_MODE_OUTPUT_PP;
	Output_2_out.Pull=GPIO_NOPULL;
	Output_2_out.Speed = GPIO_SPEED_FREQ_LOW;
	Output_3_in.Pin=Output_3_Pin;
	Output_3_in.Mode=GPIO_MODE_INPUT;
	Output_3_in.Pull=GPIO_NOPULL;
	Output_3_in.Speed = GPIO_SPEED_FREQ_LOW;
	Output_3_out.Pin=Output_3_Pin;
	Output_3_out.Mode=GPIO_MODE_OUTPUT_PP;
	Output_3_out.Pull=GPIO_NOPULL;
	Output_3_out.Speed = GPIO_SPEED_FREQ_LOW;
	Output_4_in.Pin=Output_4_Pin;
	Output_4_in.Mode=GPIO_MODE_INPUT;
	Output_4_in.Pull=GPIO_NOPULL;
	Output_4_in.Speed = GPIO_SPEED_FREQ_LOW;
	Output_4_out.Pin=Output_4_Pin;
	Output_4_out.Mode=GPIO_MODE_OUTPUT_PP;
	Output_4_out.Pull=GPIO_NOPULL;
	Output_4_out.Speed = GPIO_SPEED_FREQ_LOW;


	MX_USB_DEVICE_Init();
	HAL_GPIO_Init(GPIOB, &Output_1_out);
	HAL_GPIO_Init(GPIOB, &Output_2_out);
	HAL_GPIO_Init(GPIOB, &Output_3_out);
	HAL_GPIO_Init(GPIOB, &Output_4_out);
	HAL_ADC_Start_DMA(&hadc1, &adc_results_1, 10);
	HAL_TIM_Base_Start_IT(&htim2);
	//	HAL_ADCEx_Calibration_Start(&hadc1);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		//		HAL_ADC_Start(&hadc1);
		//		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		//		raw = HAL_ADC_GetValue(&hadc1);

		//		// Convert to string and print
		//		sprintf(msg, "%hu\r\n", raw);
		//		//    		 CDC_Transmit(0, msg, sizeof(msg));
		//		//    	     HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
		//		CDC_Transmit(0,"gowno", sizeof("gowno"));
		//		sprintf(msg, "%d\r\n", ground_id);
		//		sprintf(msg, "%d\r\n", debug_var);
		//		CDC_Transmit(0,msg, sizeof(msg));
		//		//    	     HAL_Delay(1);
		//		//	     HAL_Delay(1);
		//		if (raw < 2750)// && jajco == 1)
		//		{
		//			//    	jajco = 0;
		//			//	    	 CDC_Transmit(0, "dupa\r\n", sizeof("dupa"));
		//			//	    	 keyBoardHIDsub.MODIFIER=0x02;  // To press shift key<br>keyBoardHIDsub.KEYCODE1=0x04;  // Press A key
		//			//  keyBoardHIDsub[1]
		//			//        keyBoardHIDsub[0] = 0x0B; // Press B key
		//			//      keyBoardHIDsub[1] = 0x04; // Press B key
		//			keyBoardHIDsub[2] = 0x05; // Press C key
		//			//      keyBoardHIDsub[3] = 0x06; // To release shift key
		//			//      keyBoardHIDsub[4] = 0x07; // Release A key
		//			//      keyBoardHIDsub[5] = 0x08; // Release B key
		//			//      keyBoardHIDsub[6] = 0x09; // Release C key
		//			//      keyBoardHIDsub[7] = 0x0A; // Press B key
		//			//      keyBoardHIDsub[8] = 0x0B; // Press B key
		//			//      keyBoardHIDsub[9] = 0x0C; // Press B key
		//			//	    	 keyBoardHIDsub.KEYCODE7=0x0A;  // Release C key
		//			//	    	 keyBoardHIDsub.KEYCODE8=0x0B;  // Release C key
		//			//	    	 keyBoardHIDsub.KEYCODE9=0x0C;  // Release C key
		//			//	    	 USBD_HID_Keybaord_SendReport(&hUsbDevice,&keyBoardHIDsub,sizeof(keyBoardHIDsub));
		//			//	    	 	     sprintf(msg, "%hu\r\n", raw);
		//			//	    	 		 CDC_Transmit(0, msg, sizeof(msg));
		//		}
		//		else
		//		{
		//			//    	jajco = 1;
		//			//        keyBoardHIDsub[0] = 0x00; // Press B key
		//			//      keyBoardHIDsub[1] = 0x00; // Press B key
		//			keyBoardHIDsub[2] = 0x00; // Press C key
		//			//      keyBoardHIDsub[3] = 0x00; // To release shift key
		//			//      keyBoardHIDsub[4] = 0x00; // Release A key
		//			//      keyBoardHIDsub[5] = 0x00; // Release B key
		//			//      keyBoardHIDsub[6] = 0x00; // Release C key
		//			//      keyBoardHIDsub[7] = 0x00; // Press B keykey
		//			//      keyBoardHIDsub[8] = 0x00; // Press B key
		//			//      keyBoardHIDsub[9] = 0x00; // Press B key
		//			//			 keyBoardHIDsub.KEYCODE7=0x00;  // Release C key
		//			//			 keyBoardHIDsub.KEYCODE8=0x00;  // Release C key
		//			//			 keyBoardHIDsub.KEYCODE9=0x00;  // Release C key
		//		}
		//		// Pretend we have to do something else for a while





		for (int i = 0; i < 10; ++i) {
			sensors_states[i]=0;
			for (int j = 0; j < 4; ++j) {
				switch (j) {
				case 0:
					sprintf(msg, "%hu ", adc_results_1[i]);
					CDC_Transmit(0,msg, sizeof(msg));
					break;
				case 1:
					sprintf(msg, "%hu ", adc_results_2[i]);
					CDC_Transmit(0,msg, sizeof(msg));
					break;
				case 2:
					sprintf(msg, "%hu ", adc_results_3[i]);
					CDC_Transmit(0,msg, sizeof(msg));
					break;
				case 3:
					sprintf(msg, "%hu ", adc_results_4[i]);
					CDC_Transmit(0,msg, sizeof(msg));
					break;
				}
				HAL_Delay(1);
			}

			CDC_Transmit(0,"|\n\r", sizeof("|\n\r"));
			HAL_Delay(1);
		}

		CDC_Transmit(0,"===\n\r", sizeof("===\n\r"));
		HAL_Delay(1);
		sprintf(msg, "%hu ", debug_var);
		CDC_Transmit(0,msg, sizeof(msg));
		debug_var=0;
		HAL_Delay(1);
		CDC_Transmit(0,"\n\r===\n\r", sizeof("\n\r===\n\r"));
		HAL_Delay(1);
		sprintf(msg, "%d ====", sensors_states[1]);
		CDC_Transmit(0,msg, sizeof(msg));
		HAL_Delay(1);
		CDC_Transmit(0,"\n\r===\n\r", sizeof("\n\r===\n\r"));
		HAL_Delay(500);
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
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	{
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC|RCC_PERIPHCLK_USB;
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
