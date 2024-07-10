/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "string.h"
#include "stdio.h"
#include "LD3320.h"
#include "Reg_RW.h"
#include "delay.h"
#include "DHT11.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//sprintf�ǽ����͵����ݷŽ�u_buf���顣���ҷ��ط������ݵĳ���
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
int fputc(int ch, FILE *f)
{
	//printf????1????
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
  return ch;
}

int fgetc(FILE *f)
{
  uint8_t ch = 0;
  HAL_UART_Receive(&huart2, &ch, 1, 0xffff);
  return ch;
}									 
									 
									 
uint8_t u_buf[256];											//printf��ӳ��
uint16_t AdcBuf[3] = {0};									//ADC����
									 
uint8_t nAsrStatus=0;										//LD3320����ʶ��
uint8_t nAsrRes=0;
uint8_t flag=0;
									 
uint8_t TH = 0x00, TL = 0x00, HH = 0x00, HL = 0x00;			//��ʪ�ȴ�����

uint8_t rx_data[10];										//���ڽ���
//��������Ļ���ݲ���
char buffer1[50];
char buffer2[50];
char buffer3[50];
char buffer4[50];
char buffer5[50];
//����2
uint8_t buf_case2[1024];
uint8_t Buffer2[1024];
int rev_flag_2;//
int send_flag_2;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void Motor_SetSpeed(int8_t Speed);
void User_Modification(uint8_t data);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//��Ļָ��ͺ���
void HMISends(char *buf1) { // ?????????
    while (*buf1) {
        HAL_UART_Transmit(&huart2, (uint8_t *)buf1, 1, HAL_MAX_DELAY); // ???????????
        buf1++;
    }
}
void HMISendb(uint8_t k) { // ??????????
    uint8_t i;
    
    for (i = 0; i < 3; i++) {
        if (k != 0) {
            HAL_UART_Transmit(&huart2, &k, 1, HAL_MAX_DELAY); // ???????????
        } else {
            return;
        }
    }
}

void ScreenShow_Init(uint8_t TH,uint8_t TL,uint8_t HH, uint8_t HL,uint16_t AdcBuf[3]){
		//��Ļ��ʾ�¶�
		sprintf(buffer1, "page2.t4.txt=\"%d.%d\"", TH,TL);
		HMISends(buffer1);
		HMISendb(0xFF);
		//��ʾʪ��
		sprintf(buffer5, "page2.t9.txt=\"%d.%d\"",HH,HL);
		HMISends(buffer5);
		HMISendb(0xFF);
	  //��Ļ��ʾ����������
		sprintf(buffer2, "page2.t5.txt=\"%d\"", AdcBuf[0]);
		HMISends(buffer2);
		HMISendb(0xFF);
		//��Ļ��ʾ����������
		sprintf(buffer3, "page2.t6.txt=\"%d\"", AdcBuf[1]);
		HMISends(buffer3);
		HMISendb(0xFF);
		//��Ļ��ʾ��������
		sprintf(buffer4, "page2.t7.txt=\"%d\"", AdcBuf[2]);
		HMISends(buffer4);
		HMISendb(0xFF);
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

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)AdcBuf, 3);
	

	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);//***��ʱ����ʼ��
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);//***��ʱ����ʼ��

	
	HAL_UART_Receive_IT(&huart1, rx_data, 1);
	HAL_UART_Receive_IT(&huart2,Buffer2,1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	LD3320_Init();
	LD3320_Reset();
	nAsrStatus = LD_ASR_NONE;		//	��ʼ״̬��û������ASR
	CS(0);	
	//�ϵ��ʼ����Ļ����
	sprintf(buffer1, "page2.t4.txt=\"%d.%d\"", TH,TL);
	HMISends(buffer1);
	HMISendb(0xFF);
	ScreenShow_Init(TH,TL,HH,HL,AdcBuf);
	printf("���г���\r\n");
	
//	Motor_SetSpeed(1);							//������
	Motor_SetSpeed(0);				//�ط���
	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,50);		//���ƶ���ش�
 
  while (1)
  {
	  //test

//	  HAL_UART_Transmit(&huart1, (uint8_t *)"345", 3, 0xff);
	  if(HH > 100)
		  __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,50);		//ʪ�ȸߣ����ƶ���ش�
	  if((AdcBuf[0] < 800) && AdcBuf[0] != 0)
	  {
		  HAL_GPIO_WritePin(jidianqi_GPIO_Port, jidianqi_Pin, GPIO_PIN_SET);	//ͨ���̵����ϵ磬�����ŷ����źš�1�������Ų�������
		  printf("2");
		  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
	  }
	  //��Ļ��ʾ����������
		sprintf(buffer2, "page2.t5.txt=\"%d\"", AdcBuf[0]);
		HMISends(buffer2);
		HMISendb(0xFF);
		//��Ļ��ʾ����������
		sprintf(buffer3, "page2.t6.txt=\"%d\"", AdcBuf[1]);
		HMISends(buffer3);
		HMISendb(0xFF);
		//��Ļ��ʾ��������
		sprintf(buffer4, "page2.t7.txt=\"%d\"", AdcBuf[2]);
		HMISends(buffer4);
		HMISendb(0xFF);

		
//	  printf("%d\r\n", AdcBuf[0]);			//��ӡ������������ֵ������1600���ң���������ֵ����
//	  printf("%d\r\n", AdcBuf[1]);			//��ӡ������������ֵ������1900���ң��ȣ���ֵ���ͣ�������������1700����
//	  printf("%d\r\n", AdcBuf[2]);			//��ӡ����������ֵ������1700���ң�
//	  Delay_ms(1000);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  
	//��ʪ�ȴ�����
	DHT11_Start();
	if(DHT11_Receive_Bit() == 1)
	{
//		uint8_t flag;
//		flag = DHT11_Receive_Data(&TH, &TL, &HH, &HL);
		DHT11_Receive_Data(&TH, &TL, &HH, &HL);
		//��Ļ��ʾ�¶�
		sprintf(buffer1, "page2.t4.txt=\"%d.%d\"", TH,TL);
		HMISends(buffer1);
		HMISendb(0xFF);
		//��ʾʪ��
		sprintf(buffer5, "page2.t9.txt=\"%d.%d\"",HH,HL);
		HMISends(buffer5);
		HMISendb(0xFF);
//		printf("�¶ȣ�%d",TH);											//��ӡ�¶�
//		printf(".%d\r\n",TL);
//		printf("ʪ�ȣ�%d",HH);											//��ӡʪ��
//		printf(".%d\r\n",HL);
	}
  
  //����ʶ��
	switch(nAsrStatus)
	{
		case LD_ASR_RUNING:
		case LD_ASR_ERROR:	
				 break;
		case LD_ASR_NONE:
		{
			nAsrStatus=LD_ASR_RUNING;
			if (RunASR()==0)	/*	����һ��ASRʶ�����̣�ASR��ʼ����ASR��ӹؼ��������ASR����*/
			{
//				printf("RunASRʧ��");
				nAsrStatus = LD_ASR_ERROR;
			}
			break;
		}

		case LD_ASR_FOUNDOK: /*	һ��ASRʶ�����̽�����ȥȡASRʶ����*/
		{
			nAsrRes = LD3320_GetResult();		/*��ȡ���*/												
			User_Modification(nAsrRes);
			nAsrStatus = LD_ASR_NONE;
//			printf("ʶ����\r\n");
			break;
		}
		case LD_ASR_FOUNDZERO:
		default:
		{
			nAsrStatus = LD_ASR_NONE;
//			printf("û�н��\r\n");
			break;
		}
	} 
	//��Ļ����
	if (rev_flag_2) {
			//printf("Uart2 Received data: ");

			switch (buf_case2[0]) {
					case 0x1a:
						printf("open lights\n\r");
						HAL_GPIO_WritePin(GPIOB, light_Pin, GPIO_PIN_SET);//����
						break;
					case 0x0a:
						printf("close lights\n\r");
						HAL_GPIO_WritePin(GPIOB, light_Pin, GPIO_PIN_RESET);//�ص�
						break;
					case 0x1b:
						printf("open door\n\r");
						printf("2");
						break;
					case 0x0b:
						printf("close door\n\r");
						break;
					case 0x1c:
						printf("open window\n\r");
						__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,250);//����
						break;
					case 0x0c:
						printf("close window\n\r");
						__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,50);//�ش�
						break;
					case 0x1d:
						printf("open fan\n\r");
						Motor_SetSpeed(1);//������
						break;
					case 0x0d:
						printf("close fan\n\r");
						Motor_SetSpeed(0);				//�ط���
						break;
					default:
						//printf("%x",buf_case2[0]);
						break;
			}

			rev_flag_2 = 0; 
		}
	
	
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

/***********************************************************
* ��    �ƣ��û�ִ�к��� 
* ��    �ܣ�ʶ��ɹ���ִ�ж������ڴ˽����޸� 
* ��ڲ����� �� 
* ���ڲ�������
* ˵    ���� 					 
**********************************************************/
void User_Modification(uint8_t data)
{
	if(data ==0)
	{
		flag=1;
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
//		printf("�յ�\r\n");															//С��ͬѧ
	}
	else if(flag)
	{
		flag=0;
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
		switch(nAsrRes)		   /*�Խ��ִ����ز���,�ͻ��޸�*/
		{
			case CODE_DMCS:			/*���������ԡ�*/
					printf("\"�������\"ʶ��ɹ�\r\n"); /*text.....*/
												break;
			case CODE_CSWB:			/*���������ϡ�*/
					printf("\"�������\"ʶ��ɹ�\r\n"); /*text.....*/
												break;
			
			case CODE_1KL1:	 /*���������*/
//					printf("\"����\"ʶ��ɹ�\r\n"); /*text.....*/					//����
					HAL_GPIO_WritePin(GPIOB, light_Pin, GPIO_PIN_SET);
												break;
			case CODE_1KL2:		/*����Ϻ���*/
		
//					printf("\"�ص�\"ʶ��ɹ�\r\n"); /*text.....*/					//�ص�
					HAL_GPIO_WritePin(GPIOB, light_Pin, GPIO_PIN_RESET);
												break;
			case CODE_1KL3:	 /*������ơ�*/
//					printf("\"�򿪷���\"ʶ��ɹ�\r\n"); /*text.....*/				//�򿪷���
					Motor_SetSpeed(1);
												break;
			case CODE_1KL4:		/*����صơ�*/				
//					printf("\"�رշ���\"ʶ��ɹ�\r\n"); /*text.....*/				//�رշ���
					Motor_SetSpeed(0);
												break;
			
			case CODE_2KL1:	 /*���....��*/
//					printf("\"����\"ʶ��ɹ�\r\n"); /*text.....*/
					__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,250);				//���ƶ������
												break;
			case CODE_2KL2:	 /*���....��*/
//					printf("\"�ش�\"ʶ��ɹ�\r\n"); /*text.....*/
					__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,50);					//���ƶ���ش�
												break;
			case CODE_2KL3:	 /*���....��*/
//					printf("\"����\"ʶ��ɹ�\r\n"); /*text.....*/					//����
					printf("2");													//���Ϳ���ָ��
												break;
			case CODE_2KL4:	 /*���....��*/
					printf("\"����ת\"ʶ��ɹ�\r\n"); /*text.....*/
															break;
						
			case CODE_3KL1:	 /*���....��*/
					printf("\"�򿪿յ�\"ʶ��ɹ�\r\n"); /*text.....*/
												break;
			case CODE_3KL2:	 /*���....��*/
					printf("\"�رտյ�\"ʶ��ɹ�\r\n"); /*text.....*/
												break;
			case CODE_5KL1:	 /*���....��*/
					printf("\"����\"ʶ��ɹ�"); /*text.....*/
												break;
			
			default:break;
		}
	}
	else 	
	{
//		printf("��˵��һ������\r\n"); /*text.....*/	
	}
	
}

//���
void Motor_SetSpeed(int8_t Speed)
{
	if(Speed < 0)
	{
		HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_RESET);
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,Speed);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOA, AIN1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, AIN2_Pin, GPIO_PIN_SET);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, -Speed);
	}
}


//�ж�
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == LD3320_IRQ)
    {
        ProcessInt();
//        printf("�����ж�\r\n");
    }
	if(GPIO_Pin == S5_Pin)
    {
		//��ȡ��ʱ��
//		uint32_t now = HAL_GetTick();
//		
//		//��������С��10ms�������
//		if(now - keyTime < 10)
//			return;
//		//���°���ʱ��
//		keyTime = now;
//		
//		//���Ӽ�����
//		keyCnt++;
//		
//		//������������ڵ���3�����ʾ�����Ѿ��ȶ�
//		if(keyCnt >= 3)
//		{
//			printf("2");
//			keyCnt = 0;
//			keyTime = 0;
//		}
		
//		while(HAL_GPIO_ReadPin(S5_GPIO_Port, S5_Pin) == RESET);
		printf("2");
    }
	if(GPIO_Pin == S6_Pin)
    {
//        ProcessInt();
		printf("3");
		NVIC_SystemReset();
    }
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1) // ???????????? USART1
	{
		if (rx_data[0] == '0') // ??????????? 0
		{

		}
		if (rx_data[0] == '1') // ??????????? 0
		{
			HAL_GPIO_WritePin(light_GPIO_Port, light_Pin, GPIO_PIN_SET);
		}
		HAL_UART_Receive_IT(&huart1, (uint8_t*)rx_data, sizeof(rx_data)); // ???? UART ????
	}
	if(huart->Instance == USART2)
    {

        buf_case2[0] = Buffer2[0];
				rev_flag_2 = 1; 
				//����2���յ������ݴ�������1 
        //HAL_UART_Transmit(&huart1, Buffer2, 1, 100);
				//����ʹ�ܴ���2�����ж�
        HAL_UART_Receive_IT(&huart2, Buffer2, 1);

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
