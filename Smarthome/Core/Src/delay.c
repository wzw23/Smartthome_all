#include "stm32f4xx.h"                  // Device header
#include "tim.h"


void Delay_us(uint16_t us)
{
//	SysTick->LOAD = 72 * us;				//设置定时器重装值
//	SysTick->VAL = 0x00;					//清空当前计数值
//	SysTick->CTRL = 0x00000005;				//设置时钟源为HCLK，启动定时器
//	while(!(SysTick->CTRL & 0x00010000));	//等待计数到0
//	SysTick->CTRL = 0x00000004;				//关闭定时器
	volatile uint16_t differ = 0xffff-us-5;
	__HAL_TIM_SET_COUNTER(&htim3, differ);		//设置TIM3定时器的起始值
	HAL_TIM_Base_Start(&htim3);					//启动定时器
	
	while(differ < 0xffff-5)					//判断
	{
		differ = __HAL_TIM_GET_COUNTER(&htim3);	//查询计数器的计数值
	}
	HAL_TIM_Base_Stop(&htim3);					//关闭定时器
}

void Delay_ms(uint32_t ms)
{
	while(ms--)
		Delay_us(1000);
}



