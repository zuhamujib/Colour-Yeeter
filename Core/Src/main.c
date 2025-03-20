/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include <string.h>
#include <stdio.h>
#include "config.h"
#include "ov7670.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define CENTER_SIZE 60
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
uint16_t snapshot_buff[IMG_ROWS * IMG_COLS];
char msg[100];
uint8_t dma_flag = 0;
/*
DCMI_HandleTypeDef hdcmi;
DMA_HandleTypeDef hdma_dcmi;

I2C_HandleTypeDef hi2c2;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_tx;

PCD_HandleTypeDef hpcd_USB_OTG_FS;
*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void get_center_color(void);
/*
void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_DMA_Init(void);
void MX_DCMI_Init(void);
void MX_USART3_UART_Init(void);
void MX_USB_OTG_FS_PCD_Init(void);
void MX_I2C2_Init(void);
void MX_TIM1_Init(void);
void MX_TIM6_Init(void);
void MX_TIM2_Init(void);
void MX_TIM3_Init(void);
*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_DCMI_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_I2C2_Init();
  MX_TIM1_Init();
  MX_TIM6_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	ov7670_init();
	HAL_Delay(100);

	// we are using timer 2 channel 3 for setting the 
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); //PB10 TIM2 CH3
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2); //PA7 TIM2 CH3
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	  ov7670_capture(snapshot_buff);
  while (1)
  {
		if (dma_flag) {
			HAL_DCMI_Suspend(&hdcmi);
			get_center_color();	
			dma_flag = 0;
			HAL_DCMI_Resume(&hdcmi);
		}
		
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); //PB10 TIM2 CH3
		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2); //PB10 TIM2 CH3
		/* USER CODE END 2 */

    // Set the rotation direction forwards
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 1);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0);
	
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 0);
		
    
		// we set the timer max period to 2000 (max high speed)
		
		// to calculate speed and adjust to low, medium, high we can use the formula:
		
		// (% speed * MAX_PWM) / 100
		// Setting the speed to just 2000 for now
    __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3, 1500);
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2, 1500);
    /*
		HAL_Delay(2000);
		
		
    // Stoping the motor
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 0);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0); 
		
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 0);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 0);
    HAL_Delay(2000);
		*/

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

void get_center_color() {
  //int center = IMG_ROWS*CENTER_SIZE / 2;
  //int offset = CENTER_SIZE*CENTER_SIZE / 2;
  uint16_t *buffer = snapshot_buff;
  int start_index = (IMG_ROWS/2 - CENTER_SIZE/2) * IMG_COLS + (IMG_COLS/2 - CENTER_SIZE/2);
  //uint8_t colour;
  uint32_t red_sum = 0;
  uint32_t green_sum = 0;
  uint32_t blue_sum = 0;
  uint16_t count = 0;
  uint16_t red_count = 0;
  uint16_t green_count = 0;
  uint16_t blue_count = 0;
  uint16_t unknown_count = 0;

  //for (int i = center - offset; i < center + offset; i++) {
  //for (int i = IMG_ROWS*IMG_COLS/3 - 1; i < IMG_ROWS*IMG_COLS*2/3; i++) {
  for (int row = 0; row < CENTER_SIZE; row++) {
    int index = start_index + (row * IMG_COLS);
    for (int col = 0; col < CENTER_SIZE; col++) {
    uint16_t pixel = buffer[index+col];
    uint8_t r = (pixel >> 11) & 0x1F;  // 5 bits red
    uint8_t g = ((pixel >> 5) & 0x3F)/2;   // 6 bits green
    uint8_t b = pixel & 0x1F;          // 5 bits blue

    // Classify pixel based on color dominance
    if (r > 10 || g > 10 || b > 10) { // ignore skewed white / noise
      red_sum+=r;
      green_sum+=g;
      blue_sum+=b;
      count++;
      if (r > g+5 && b+5 < r)
        red_count++;
      else if (g > r+5 && b+5 < g)
        green_count++;
      else if (b > r+5 && g+5 < b)
        blue_count++;
      else
        unknown_count++;
    }
    else unknown_count++;
    }
  }
  // Average the RGB values
  uint8_t red_avg = red_sum / count;
  uint8_t green_avg = green_sum / count;
  uint8_t blue_avg = blue_sum / count;
  // Determine dominant color
  if (red_count > green_count+5 && blue_count+5 < red_count ) {
    // RED, 1
  //colour = 1;
  sprintf(msg, "red");
  HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
  HAL_Delay(200);
  HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
  }
  else if (green_count > red_count+5 && blue_count+5 < green_count) {
  // GREEN, 2
  //colour = 2;
  sprintf(msg, "green");
  HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
  HAL_Delay(200);
  HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
  }
  else if (blue_count > green_count+5 && red_count+5 < blue_count) {
    // BLUE, 3
  //colour = 3;
  sprintf(msg, "blue");
  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
  HAL_Delay(200);
  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
  }
  else sprintf(msg, "colour unknown");

  HAL_UART_Transmit(&huart3, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
  sprintf(msg, " - R:%d avg, %d pixels, G:%d avg, %d pixels, B:%d avg, %d pixels\r\n", red_avg, red_count, green_avg, green_count, blue_avg, blue_count);
  HAL_UART_Transmit(&huart3, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
  //sprintf(msg, " R: %d, G: %d, B: %d\r\n", red_avg, green_avg, blue_avg); // give average rgb values
  //sprintf(msg, " R: %d, G: %d, B: %d\r\n", red_count, green_count, blue_count); // give number of rgb pixels
}