/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : 智能门锁
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  * 定时器2 通道1控制舵机
  * 定时器3 延时
  * 定时器4 as608
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "stdio.h"
#include "OLED.h"
#include "rc522_config.h"
#include "rc522_function.h"
#include "keypad.h"
#include "delay.h"
#include "as608.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//sprintf是将发送的内容放进u_buf数组。并且返回发送内容的长度
#define printf(...) HAL_UART_Transmit(&huart1,\
									 (uint8_t *)u_buf,\
									 sprintf((char*)u_buf,__VA_ARGS__),\
									 0xffff)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
									 
uint8_t u_buf[256];											//printf重映像
									 
char Door[] = "Close";										//开门状态
uint8_t val = 0;											//按键数值
uint8_t key_nums = 0;
uint8_t code[6] = {1, 2, 3, 4, 5, 6};						//初始密码
const uint8_t Administrator_code[6] = {5, 2, 2, 0, 0, 8};	//管理员密码
uint8_t code1[6] = {0}, code2[6] = {0};						//重置密码输入两次
uint8_t In_code[6] = {0};									//输入密码
uint8_t N = 0;												//验证密码次数
uint8_t Administrator_mode = 0;								//0：正常模式，1：进入管理员模式
uint8_t ban = 0;											//0：正常模式，1：连续三次输入密码错误，禁止输入密码并报警

unsigned char g_ucTempbuf[20];								//NFC卡标识

uint8_t rx_data[15];										//串口接收

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void Reset_code(void);

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
	
	unsigned char temp,status;
	unsigned int i;
	
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
  MX_USART1_UART_Init();
  MX_TIM3_Init();
  MX_TIM2_Init();
  MX_USART6_UART_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  
	// 清除定时器初始化过程中的更新中断标志，避免定时器一启动就中断
	__HAL_TIM_CLEAR_IT(&htim4, TIM_IT_UPDATE);		
	// 使能定时器4更新中断并启动定时器
	HAL_TIM_Base_Start_IT(&htim4);
	OLED_Init();		//OLED初始化

	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,50);		//控制舵机关锁
	
	HAL_UART_Receive_IT(&huart1, rx_data, 1);
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

	OLED_Clear();
	

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	
	OLED_ShowImage(0, 0, 40, 64, Lock);
	OLED_Update();
	  
	OLED_ShowChinese(48, 0, "门状态");
	OLED_ShowString(96, 0, ":",OLED_8X16);
	OLED_ShowChinese(104, 0, "关");
	OLED_ShowChinese(48, 16, "按键");
	OLED_ShowChinese(48, 32, "指纹");
	OLED_ShowString(48, 48, "NFC",OLED_8X16);
//	OLED_ShowChinese(48, 0, "开锁方式");
//	OLED_ShowString(48, 16, "K1:",OLED_8X16);
//	OLED_ShowChinese(72, 16, "按键");
//	OLED_ShowString(48, 32, "K2:IC",OLED_8X16);
//	OLED_ShowChinese(88, 32, "卡");
//	OLED_ShowString(48, 48, "K3:",OLED_8X16);
//	OLED_ShowChinese(72, 48, "指纹");
	
	key_nums = Keypad_scan();
	/*按键开锁*/
	if(key_nums == 1)
	{
		OLED_Clear();
		while(1)
		{

			OLED_ShowString(32,0,"Key",OLED_8X16);
			OLED_ShowChinese(16,16,"请输入密码");
			OLED_ShowChinese(16,32,"门状态");
			OLED_ShowChar(64, 32, ':', OLED_8X16);
			OLED_ShowString(72, 32, Door, OLED_8X16);
	
			if(ban == 0)	//正常输入密码
			{
				/*输入密码*/
				val = Keypad_scan();
				if(anjian == 0)
				{
					uint8_t i;
					for(i = 0; i < 5; i++)
					{
						In_code[i] = In_code[i+1];
					}
					In_code[5] = val;
					anjian = 1;
				}
				else if(val == 15)		//验证密码
				{
					N ++;
					/*匹配密码，正确开锁*/
					if(In_code[0] == code[0] & In_code[1] == code[1] & In_code[2] == code[2] & In_code[3] == code[3] 
						& In_code[4] == code[4] & In_code[5] == code[5])
					{
						strcpy(Door, "Open");
						OLED_ShowString(72, 32, "        ", OLED_8X16);
						__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,250);		//控制舵机开锁
						printf("1");
						Reset_code();		//清空密码
						OLED_ShowChinese(16, 48, "密码输入正确");
						N = 0;
					}
					else
					{
						OLED_ShowChinese(16, 48, "密码输入错误");
					}
					if(N >= 3)
					{
						ban = 1;
					}
				}
				else if(val == 14)	//按键关锁
				{
					strcpy(Door, "Close");
					__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,50);		//控制舵机关锁
					Reset_code();
					OLED_ShowString(16, 48, "            ", OLED_8X16);
					OLED_ShowString(72, 32, Door, OLED_8X16);
					Delay_ms(800);
					OLED_Clear();
					break;
				}
				else if(val == 10)	//进入管理员模式进行修改密码
				{
					//感觉要用RTOS
					while(1)
					{
						OLED_ShowChinese(0, 16, "请输入管理员密码");
						OLED_ShowString(0, 32, "                ", OLED_8X16);
						/*输入管理员密码*/
						val = Keypad_scan();
						if(anjian == 0)
						{
							uint8_t i;
							for(i = 0; i < 5; i++)
							{
								In_code[i] = In_code[i+1];
							}
							In_code[5] = val;
							anjian = 1;
						}
						else if(val == 15)		//验证管理员密码
						{
							/*匹配管理员密码，正确进行修改密码操作*/
							if(In_code[0] == Administrator_code[0] & In_code[1] == Administrator_code[1] & In_code[2] == Administrator_code[2] & 
								In_code[3] == Administrator_code[3] & In_code[4] == Administrator_code[4] & In_code[5] == Administrator_code[5])
							{
								Reset_code();		//清空密码
								Administrator_mode = 1;	
								OLED_ShowString(0, 16, "                ",OLED_8X16);
								OLED_ShowChinese(0, 48, "管理员密码正确");
								Delay_ms(2000);
								OLED_ShowString(0, 48, "                ",OLED_8X16);
								break;
							}
							else
							{
								OLED_ShowString(0, 16, "                ",OLED_8X16);
								OLED_ShowChinese(0, 48, "管理员密码错误");
								Delay_ms(2000);
								OLED_ShowString(0, 48, "                ",OLED_8X16);
								break;
							}
						}
						else if(val == 11)		//退出管理员模式
						{
							OLED_ShowString(4, 1, "                ", OLED_8X16);
							Administrator_mode = 0;
							break;
						}
					}	
				}
				else if(val == 12)	//	退出按键模式
				{
					key_nums = 0;
					OLED_Clear();
					break;
				}
			
				/*在管理员模式下进行密码设置*/
				while(Administrator_mode)
				{
					OLED_ShowString(0, 16, "  ", OLED_8X16);
					OLED_ShowString(112, 16, "  ", OLED_8X16);
					OLED_ShowChinese(16, 16, "请输入新密码");
					/*输入新密码*/
					val = Keypad_scan();
					if(anjian == 0)
					{
						uint8_t i;
						for(i = 0; i < 5; i++)
						{
							code1[i] = code1[i+1];
						}
						code1[5] = val;
						anjian = 1;
					}
					else if(val == 15)
					{
						while(Administrator_mode)
						{
							OLED_ShowString(0, 16, "  ", OLED_8X16);
							OLED_ShowString(96, 16, "    ", OLED_8X16);
							OLED_ShowChinese(16, 16, "请再次输入");
							/*再次输入新密码*/
							val = Keypad_scan();
							if(anjian == 0)
							{
								uint8_t i;
								for(i = 0; i < 5; i++)
								{
									code2[i] = code2[i+1];
								}
								code2[5] = val;
								anjian = 1;
							}
							if(val == 15)
							{
								if(code1[0] == code2[0] & code1[1] == code2[1] & code1[2] == code2[2] & code1[3] == code2[3] & 
									 code1[4] == code2[4] & code1[5] == code2[5])
								{
									OLED_ShowChinese(0, 48, "新密码设置成功");
									Delay_ms(3000);
									OLED_ShowString(0, 48, "                ", OLED_8X16);
									for(uint8_t i = 0; i < 6; i ++)
									{
										code[i] = code1[i];
									}
									Administrator_mode = 0;
									N = 0;
								}
								else
								{
									OLED_ShowChinese(0, 32, "两次输入不同");
									OLED_ShowChinese(0, 48, "新密码设置失败");
									Delay_ms(3000);
									OLED_ShowString(0, 32, "                ", OLED_8X16);
									OLED_ShowString(0, 48, "                ", OLED_8X16);
									Administrator_mode = 0;
								}
							}
						}
					}
					else if(val == 11)		//退出管理员模式
					{
						OLED_ShowString(4, 1, "                ", OLED_8X16);
						Administrator_mode = 0;
						break;
					}
				}
			}
			else if(ban == 1)	//触发防盗
			{
				while(1)
				{
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);		/*Buzzer_ON();*/
					OLED_Clear();
					OLED_ShowString(0, 0, "****************", OLED_8X16);
					val = Keypad_scan();
					if(val == 13)	//手动取消
					{
						N = 0;
						ban = 0;
						HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);		/*Buzzer_OFF();*/
						OLED_Clear();
						break;
					}
				}
			}
		}
	}
	/*IC卡开锁*/
	else if(key_nums == 2)
	{
		OLED_Clear();
		OLED_ShowChinese(16,32,"门状态");
		OLED_ShowChar(64, 32, ':', OLED_8X16);
		OLED_ShowString(72, 32, Door, OLED_8X16);
		while(1)
		{
			OLED_ShowString(32,0,"IC",OLED_8X16);
			val=Keypad_scan();
			if(val == 12)
			{
				key_nums = 0;
				OLED_Clear();
				break;
			}
			else if(val == 14)	//按键关锁
			{
				__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,50);		//控制舵机关锁
				Delay_ms(800);
				strcpy(Door, "Close");
				OLED_ShowChinese(16,32,"门状态");
				OLED_ShowChar(64, 32, ':', OLED_8X16);
				OLED_ShowString(72, 32, Door, OLED_8X16);
				OLED_ShowString(16, 48, "            ", OLED_8X16);
				Delay_ms(800);
				OLED_Clear();
				break;
			}
			status = PcdRequest(PICC_REQALL, g_ucTempbuf);//寻卡
			if (status != MI_OK)
			{    
				PcdReset();
				PcdAntennaOff(); 
				PcdAntennaOn(); 
				continue;
			}
			status = PcdAnticoll(g_ucTempbuf);//防冲撞
			if (status != MI_OK)
			{    
				continue;    
			}
			for(i=0;i<1;i++)		//for(i=0;i<4;i++)
			{
				temp=g_ucTempbuf[i];
				if(temp == 0xB3)
				{	
					OLED_ShowChinese(16, 16, "验证成功");
					Delay_ms(800);
					OLED_ShowString(16, 16, "  ", OLED_8X16);
					OLED_ShowString(64, 16,"  ", OLED_8X16);
					OLED_ShowChinese(32, 16, "开锁");
					Delay_ms(800);
					__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,250);		//控制舵机开锁
					printf("1");
					Delay_ms(800);
					strcpy(Door, "Open");
					OLED_ShowString(72, 32, "        ", OLED_8X16);
					OLED_ShowChinese(16,32,"门状态");
					OLED_ShowChar(64, 32, ':', OLED_8X16);
					OLED_ShowString(72, 32, Door, OLED_8X16);
					OLED_ShowString(104, 32, "   ", OLED_8X16);
					OLED_ShowString(32, 16,"    ", OLED_8X16);
				}
				else
				{
					OLED_ShowChinese(16, 16, "验证失败");
					Delay_ms(1000);
					OLED_ShowString(16, 16, "            ", OLED_8X16);
				}
			}
			
		}
	}
	/*指纹开锁*/
	else if(key_nums == 3)
	{
		OLED_Clear();
		while(1)
		{
			OLED_ShowChinese(32, 0, "指纹开锁");
			OLED_ShowString(16, 16, "K1",OLED_8X16);
			OLED_ShowChinese(32, 16, "键添加指纹");
			OLED_ShowString(16, 32, "K3",OLED_8X16);
			OLED_ShowChinese(32, 32, "键删除指纹");
			OLED_ShowString(16, 48, "K5",OLED_8X16);
			OLED_ShowChinese(32, 48, "键验证指纹");
			val=Keypad_scan();
			if(val==1)
			{
				val=0;
				OLED_Clear();
				Add_FR();
			}
			else if(val==3)
			{
				val=0;
				OLED_Clear();
				Del_FR();
			}
			else if(val==5)
			{
				val=0;
				OLED_Clear();
				OLED_ShowChinese(32,16,"请按手指");
				if(press_FR())
				{
					__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,250);		//控制舵机开锁
					printf("1");
					OLED_Clear();
					OLED_ShowChinese(32, 0, "指纹开锁");
					OLED_ShowString(16,16,"K1",OLED_8X16);
					OLED_ShowChinese(32,16,"键添加指纹");
					OLED_ShowString(16,32,"K3",OLED_8X16);
					OLED_ShowChinese(32,32,"键删除指纹");
					OLED_ShowString(16,48,"K5",OLED_8X16);
					OLED_ShowChinese(32,48,"键验证指纹");
				}
				else
				{
					OLED_Clear();
					OLED_ShowChinese(32, 0, "指纹开锁");
					OLED_ShowString(16,16,"K1",OLED_8X16);
					OLED_ShowChinese(32,16,"键添加指纹");
					OLED_ShowString(16,32,"K3",OLED_8X16);
					OLED_ShowChinese(32,32,"键删除指纹");
					OLED_ShowString(16,48,"K5",OLED_8X16);
					OLED_ShowChinese(32,48,"键验证指纹");
				}

			}
			else if(val == 12)
			{
				key_nums = 0;
				OLED_Clear();
				break;
			}
			else if(val == 14)	//按键关锁
			{
//				strcpy(Door, "Close");
//				OLED_ShowChinese(16,32,"门状态");
//				OLED_ShowChar(64, 32, ':', OLED_8X16);
//				OLED_ShowString(72, 32, Door, OLED_8X16);
				__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,50);		//控制舵机关锁
				OLED_Clear();
				break;
//				Reset_code();
//				OLED_ShowString(16, 48, "            ", OLED_8X16);
			}
		}
	}
//	else if(key_nums == 14)	//按键关锁
//	{
//		strcpy(Door, "Close");
//		__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,50);		//控制舵机关锁
////		Reset_code();
////		OLED_ShowString(16, 48, "            ", OLED_8X16);
////		OLED_ShowString(72, 32, Door, OLED_8X16);
////		Delay_ms(800);
////		OLED_Clear();
////		break;
//	}
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 144;
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
}

/* USER CODE BEGIN 4 */
/*清零输入的密码栈*/
void Reset_code(void)
{
	for(uint16_t i = 0; i < 6; i ++)
	{
		In_code[i] = 0;
	}
}

//串口中断
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		if (rx_data[0] == '2')
		{
			__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,250);		//控制舵机开锁
			rx_data[0] = 0;
		}
		else if (rx_data[0] == '3')
		{
			
			rx_data[0] = 0;
		}
		HAL_UART_Receive_IT(&huart1, (uint8_t*)rx_data, sizeof(rx_data));
	}
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
