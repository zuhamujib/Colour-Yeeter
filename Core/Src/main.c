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
#include <string.h>
#include <stdio.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
#include "config.h"
#include "ov7670.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
enum {
    COLOUR_UNKNOWN = 1, 
		COLOUR_RED, 
		COLOUR_BLUE,
		COLOUR_GREEN
};
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define CENTER_SIZE 60
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint16_t snapshot_buff[IMG_ROWS * IMG_COLS];
char msg[100];
uint8_t dma_flag = 0;
uint16_t detected_colour = COLOUR_UNKNOWN;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */
void get_center_color(void);
void move_to_colour(uint16_t duration);

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
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	ov7670_init();
	HAL_Delay(100);

	// we are using timer 2 channel 3 for setting the 
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); //PB10 TIM2 CH3
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2); //PA7 TIM3 CH2
	
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1); //PD12 TIM4 CH1
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2); //PD13 TIM4 CH2
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3); //PD14 TIM4 CH3
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	  ov7670_capture(snapshot_buff);
		
		
  while (1)
  {
		
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
    __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3, 2000);
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2, 2000);
		
		//HAL_Delay(200);
		//__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3, 1000);
		//__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2, 1000);
		
		// stopping motors when object detected
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 0);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 0);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 0);
		HAL_Delay(500);
			
		if (dma_flag) {
			HAL_DCMI_Suspend(&hdcmi);
			get_center_color();
			
			dma_flag = 0;
			HAL_DCMI_Resume(&hdcmi);
		}
		
		if (detected_colour != COLOUR_UNKNOWN) {
			// move to sorting zone depending on color
			switch (detected_colour) {
				case COLOUR_RED:
					sprintf(msg, "red");
					HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
					HAL_Delay(200);
					HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
					move_to_colour(1000); // move motors for longest time
					__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_1, 2500);
					HAL_Delay(1105);
					__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_1, 1500);
					HAL_Delay(200);
					__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_1, 500);
					HAL_Delay(1100);
					__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_1, 1500);
					break;
				case COLOUR_GREEN:
					sprintf(msg, "green");
					HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
					HAL_Delay(200);
					HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
					move_to_colour(400); // move motors for shortest time
					__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_2, 2500);
					HAL_Delay(1000);
					__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_2, 1500);
					HAL_Delay(200);
					__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_2, 500);
					HAL_Delay(1000);
					__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_2, 1500);
					break;
				case COLOUR_BLUE:
					sprintf(msg, "blue");
					HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
					HAL_Delay(200);
					HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
					move_to_colour(700); // move motors for shortest time
					__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_3, 2500);
					HAL_Delay(1000);
					__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_3, 1500);
					HAL_Delay(200);
					__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_3, 500);
					HAL_Delay(1000);
					__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_3, 1500);
					break;
				default:
					// idk, skip??
					move_to_colour(1200); // move motors for shortest time
					break;
			}
		}
    /* USER CODE END WHILE */
		HAL_Delay(1000);
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}


void move_to_colour(uint16_t duration) {
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
		
		HAL_Delay(duration);
		//__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3, 1000);
		//__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2, 1000);
		
		// stopping motors when object detected
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 0);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 0);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 0);
		// HAL_Delay(1000);
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
	detected_colour = COLOUR_RED;
  }
  else if (green_count > red_count+5 && blue_count+5 < green_count) {
  // GREEN, 2
  //colour = 2;
	detected_colour = COLOUR_GREEN;
  }
  else if (blue_count > green_count+5 && red_count+5 < blue_count) {
    // BLUE, 3
  //colour = 3;
	detected_colour = COLOUR_BLUE;
  }
  else {
		sprintf(msg, "colour unknown");
		detected_colour = COLOUR_UNKNOWN;
	}

  HAL_UART_Transmit(&huart3, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
  sprintf(msg, " - R:%d avg, %d pixels, G:%d avg, %d pixels, B:%d avg, %d pixels\r\n", red_avg, red_count, green_avg, green_count, blue_avg, blue_count);
  HAL_UART_Transmit(&huart3, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
  //sprintf(msg, " R: %d, G: %d, B: %d\r\n", red_avg, green_avg, blue_avg); // give average rgb values
  //sprintf(msg, " R: %d, G: %d, B: %d\r\n", red_count, green_count, blue_count); // give number of rgb pixels
}