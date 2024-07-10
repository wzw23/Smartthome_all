#include "stm32f4xx.h"                  // Device header
#include "tim.h"


void Delay_us(uint16_t us)
{

	volatile uint16_t differ = 0xffff-us-5;
	__HAL_TIM_SET_COUNTER(&htim2, differ);		//设置TIM3定时器的起始值
	HAL_TIM_Base_Start(&htim2);					//启动定时器
	
	while(differ < 0xffff-5)					//判断
	{
		differ = __HAL_TIM_GET_COUNTER(&htim2);	//查询计数器的计数值
	}
	HAL_TIM_Base_Stop(&htim2);					//关闭定时器
}

void Delay_ms(uint32_t ms)
{
	while(ms--)
		Delay_us(1000);
}



