/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "user_delay.h"
#include "stdio.h"
#include "user_gpio.h"
#include	"user_main.h"
#include "user_flash.h"
#include "DAP_config.h"
#include "oled.h"
#include "user_swdp.h"
#include "TEA.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

static void UART_Init(uint32_t bound);
/* USER CODE BEGIN PTD */
u8 aRxBuffer[1];//HAL库使用的串口接收缓冲
UART_HandleTypeDef huart2; //UART句柄

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;

/* USER CODE BEGIN PV */
extern u16 USART_RX_STA;
extern u8 USART_RX_BUF[265];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int fputc(int ch, FILE *f)
{      
//	HAL_UART_Transmit(&UART_Handler,(uint8_t *)ch,1,0xFFFF);
	while((USART2->SR&0X40)==0); 
	USART2->DR = (u8) ch;      
	return ch;
}
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
	Delay_Init(72);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
	
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_TIM1_Init();  
	UART_Init(115200);
	SPI_Flash_Init();
	OLED_Init();
	Upgrade_Target_Flash();
  /* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start_IT(&htim1);
  /* USER CODE END 2 */

  /* Infinite loop */
	
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE BEGIN 3 */
		user_main();
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

  /** Initializes the CPU, AHB and APB busses clocks 
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
  /** Initializes the CPU, AHB and APB busses clocks 
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */

static void DeInit_SPI(void)
{
	SPI1->CR1	|=0<<6;
	RCC->APB2ENR	|=0<<12;
	RCC->APB2ENR	|=0;

	
	GPIOB->CRL	&=0xF0000FFF;
	GPIOB->CRL	|=0x03743000;
	
	GPIOB->BRR=GPIO_PIN_3;
	GPIOB->BSRR=GPIO_PIN_5;
}

static void SPI_ON(void)
{
	RCC->APB2ENR |=1<<12;
	RCC->APB2ENR |=1<<0;
	
	
	GPIOB->CRL	&=0xF0000FFF;//GPIO INIT
	GPIOB->CRL	|=0x03F4B000;//GPIO INIT
	
	SPI1->CR1	|=1<<6;//EN SPI
}

void SPI_Switch(uint8_t ONOFF)
{
	if(ONOFF==1) SPI_ON();
	else DeInit_SPI();
}

static void MX_SPI1_Init(void)
{
  /* USER CODE BEGIN SPI1_Init 0 */
	RCC->APB2ENR |=1<<12;//SPI CK EN
	RCC->APB2ENR |=1<<3;//GPIO CK EN

	GPIOB->CRL	&=0xF0000FFF;//GPIO INIT
	GPIOB->CRL	|=0x03F4B000;//GPIO INIT
	
	SPI1->CR1	|=0<<10;//DOUBULE
	SPI1->CR1	|=1<<9;
	SPI1->CR1	|=1<<8;
	SPI1->CR1	|=1<<7;//LSB
	SPI1->CR1	|=0<<11;//8BIT
	SPI1->CR1	|=1<<2;//HOST
	SPI1->CR1	|=0<<1;//CPOL LOW
	SPI1->CR1	|=0<<0;//CPHA 1EDGE
	SPI1->CR1	|=2<<3;//F/8
	
	SPI1->CR1	|=1<<6;//EN SPI
	
	
	/* USER CODE END SPI1_Init 0 */
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
  htim1.Init.Prescaler = 7199;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 99;
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
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

	
	__HAL_AFIO_REMAP_SPI1_ENABLE();
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);


  /*Configure GPIO pins : PA0 PA1 PA2 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB11 PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */

static void UART_Init(uint32_t bound)
{
	//UART 初始化设置
	huart2.Instance=USART2;					    //USART1
	huart2.Init.BaudRate=bound;				    //波特率
	huart2.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	huart2.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	huart2.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位
	huart2.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
	huart2.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&huart2);					    //HAL_UART_Init()会使能UART1
	

	HAL_UART_Receive_IT(&huart2, (u8 *)aRxBuffer, 1);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量
}
//-----Soft reset + Hard reset-------------------------------------------------
#define PIN_SWCLK_SET PIN_SWCLK_TCK_SET
#define PIN_SWCLK_CLR PIN_SWCLK_TCK_CLR

#define RST_CLOCK_CYCLE()                \
  PIN_SWCLK_CLR();                       \
  PIN_DELAY();                           \
  PIN_SWCLK_SET();                       \
  PIN_DELAY()

#define RST_WRITE_BIT(bit)               \
  PIN_SWDIO_OUT(bit);                    \
  PIN_SWCLK_CLR();                       \
  PIN_DELAY();                           \
  PIN_SWCLK_SET();                       \
  PIN_DELAY()

#define RST_READ_BIT(bit)                \
  PIN_SWCLK_CLR();                       \
  PIN_DELAY();                           \
  bit = PIN_SWDIO_IN();                  \
  PIN_SWCLK_SET();                       \
  PIN_DELAY()

//#define PIN_DELAY() PIN_DELAY_SLOW(DAP_Data.clock_delay)
#define PIN_DELAY() PIN_DELAY_FAST()

uint8_t RST_Transfer(uint32_t request, uint32_t data)
{
  uint32_t ack;                                                                 \
  uint32_t bit;                                                                 \
  uint32_t val;                                                                 \
  uint32_t parity;                                                              \
  uint32_t n;                                                                   \
  \
  /* Packet Request */                                                          \
  parity = 0U;                                                                  \
  RST_WRITE_BIT(1U);                     /* Start Bit */                        \
  bit = request >> 0;                                                           \
  RST_WRITE_BIT(bit);                    /* APnDP Bit */                        \
  parity += bit;                                                                \
  bit = request >> 1;                                                           \
  RST_WRITE_BIT(bit);                    /* RnW Bit */                          \
  parity += bit;                                                                \
  bit = request >> 2;                                                           \
  RST_WRITE_BIT(bit);                    /* A2 Bit */                           \
  parity += bit;                                                                \
  bit = request >> 3;                                                           \
  RST_WRITE_BIT(bit);                    /* A3 Bit */                           \
  parity += bit;                                                                \
  RST_WRITE_BIT(parity);                 /* Parity Bit */                       \
  RST_WRITE_BIT(0U);                     /* Stop Bit */                         \
  RST_WRITE_BIT(1U);                     /* Park Bit */                         \
  \
  /* Turnaround */                                                              \
  PIN_SWDIO_OUT_DISABLE();                                                      \
  for (n = DAP_Data.swd_conf.turnaround; n; n--) {                              \
    RST_CLOCK_CYCLE();                                                          \
  }                                                                             \
  \
  /* Acknowledge response */                                                    \
  RST_READ_BIT(bit);                                                            \
  ack  = bit << 0;                                                              \
  RST_READ_BIT(bit);                                                            \
  ack |= bit << 1;                                                              \
  RST_READ_BIT(bit);                                                            \
  ack |= bit << 2;                                                              \
  \
  /* Data transfer */                                                           \
  /* Turnaround */                                                              \
  for (n = DAP_Data.swd_conf.turnaround; n; n--) {                              \
    RST_CLOCK_CYCLE();                                                          \
  }                                                                             \
                                                         \
  /* Write data */                                                              \
  val = data;                                                                   \
  parity = 0U;                                                                  \
  for (n = 32U; n; n--) {                                                       \
    RST_WRITE_BIT(val);              /* Write WDATA[0:31] */                    \
    parity += val;                                                              \
    val >>= 1;                                                                  \
  }                                                                             \
  RST_WRITE_BIT(parity);             /* Write Parity Bit */                     \
                                                         \
  PIN_SWDIO_OUT(1U);                                                            \
  return ((uint8_t)ack);                                                        \
}

void vResetTarget(uint8_t bit)
{
  uint32_t i;
  //soft-reset for Cortex-M
  RST_Transfer(0x00000CC5, 0xE000ED0C); //set AIRCR address
  for (i=0; i<100; i++);
  RST_Transfer(0x00000CDD, 0x05FA0007); //set RESET data
  for (i=0; i<100; i++);
  RST_Transfer(0x00000CC5, 0xE000ED0C); //repeat
  for (i=0; i<100; i++);
  RST_Transfer(0x00000CDD, 0x05FA0007);
  
  if (bit & 1)  PIN_nRESET_HIGH();
  else          PIN_nRESET_LOW();
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
