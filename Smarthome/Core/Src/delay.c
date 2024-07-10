#include "stm32f4xx.h"                  // Device header
#include "tim.h"


void Delay_us(uint16_t us)
{
//	SysTick->LOAD = 72 * us;				//���ö�ʱ����װֵ
//	SysTick->VAL = 0x00;					//��յ�ǰ����ֵ
//	SysTick->CTRL = 0x00000005;				//����ʱ��ԴΪHCLK��������ʱ��
//	while(!(SysTick->CTRL & 0x00010000));	//�ȴ�������0
//	SysTick->CTRL = 0x00000004;				//�رն�ʱ��
	volatile uint16_t differ = 0xffff-us-5;
	__HAL_TIM_SET_COUNTER(&htim3, differ);		//����TIM3��ʱ������ʼֵ
	HAL_TIM_Base_Start(&htim3);					//������ʱ��
	
	while(differ < 0xffff-5)					//�ж�
	{
		differ = __HAL_TIM_GET_COUNTER(&htim3);	//��ѯ�������ļ���ֵ
	}
	HAL_TIM_Base_Stop(&htim3);					//�رն�ʱ��
}

void Delay_ms(uint32_t ms)
{
	while(ms--)
		Delay_us(1000);
}



