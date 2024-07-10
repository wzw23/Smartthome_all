#include "stm32f4xx.h"                  // Device header
#include "tim.h"


void Delay_us(uint16_t us)
{

	volatile uint16_t differ = 0xffff-us-5;
	__HAL_TIM_SET_COUNTER(&htim2, differ);		//����TIM3��ʱ������ʼֵ
	HAL_TIM_Base_Start(&htim2);					//������ʱ��
	
	while(differ < 0xffff-5)					//�ж�
	{
		differ = __HAL_TIM_GET_COUNTER(&htim2);	//��ѯ�������ļ���ֵ
	}
	HAL_TIM_Base_Stop(&htim2);					//�رն�ʱ��
}

void Delay_ms(uint32_t ms)
{
	while(ms--)
		Delay_us(1000);
}



