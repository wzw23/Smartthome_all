#include "stm32f4xx.h"                  // Device header
#include "gpio.h"

uint8_t anjian = 1;

void Keypad_Init(void)
{

	MX_GPIO_Init();
	
}

uint16_t Keypad_scan(void)
{
	uint16_t key_val = 0;
	
	//第一行输出为低电平，扫描四列
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);

	if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_12) == 0)
	{
		while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_12) == 0);
		key_val = 10;
	}
	else if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13) == 0)
	{
		while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13) == 0);
		key_val = 11;
	}
	else if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14) == 0)
	{
		while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14) == 0);
		key_val = 12;
	}
	else if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15) == 0)
	{
		while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15) == 0);
		key_val = 13;
	}
	
	//第二行输出为低电平，扫描四列
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
	
	if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_12) == 0)
	{
		while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_12) == 0);
		key_val = 7;
		anjian = 0;	
	}
	else if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13) == 0)
	{
		while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13) == 0);
		key_val = 8;
		anjian = 0;
	}
	else if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14) == 0)
	{
		while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14) == 0);
		key_val = 9;
		anjian = 0;
	}
	else if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15) == 0)
	{
		while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15) == 0);
		key_val = 0;
		anjian = 0;
	}
	
	//第三行输出为低电平，扫描四列
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
	
	if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_12) == 0)
	{
		while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_12) == 0);
		key_val = 4;
		anjian = 0;	
	}
	else if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13) == 0)
	{
		while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13) == 0);
		key_val = 5;
		anjian = 0;
	}
	else if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14) == 0)
	{
		while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14) == 0);
		key_val = 6;
		anjian = 0;
	}
	else if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15) == 0)
	{
		while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15) == 0);
		key_val = 14;
	}
	
	//第四行输出为低电平，扫描四列
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);

	if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_12) == 0)
	{
		while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_12) == 0);
		key_val = 1;
		anjian = 0;	
	}
	else if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13) == 0)
	{
		while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13) == 0);
		key_val = 2;
		anjian = 0;
	}
	else if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14) == 0)
	{
		while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14) == 0);
		key_val = 3;
		anjian = 0;
	}
	else if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15) == 0)
	{
		while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15) == 0);
		key_val = 15;
	}
	
	return key_val;
}

