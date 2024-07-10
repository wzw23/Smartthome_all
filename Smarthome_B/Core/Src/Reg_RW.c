#include "stm32f4xx.h"                  // Device header


#include "delay.h"
#include "Reg_RW.h"
#include "usart.h"
#include "LD3320.h"
#include "GPIO.h"

//寄存器读写
//软件模拟SPI方式读写
#define Delay_Nop 	Delay_us(1)


void LD3320_Init(void)
{
//	RCC_APB2PeriphClockCmd(LD3320_RCC_APB2Periph_GPIO | RCC_APB2Periph_AFIO, ENABLE);
//	
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//要先开时钟，再重映射；关闭jtag，保留swd。
//	
//	GPIO_InitTypeDef GPIO_InitStructure;
//	
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_Pin = LD3320_SCK | LD3320_MI | LD3320_CS | LD3320_RST;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(LD3320_GPIO, &GPIO_InitStructure);					//初始化SCK、MI、CS、RST
//	
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//	GPIO_InitStructure.GPIO_Pin = LD3320_MO;
//	GPIO_Init(LD3320_GPIO, &GPIO_InitStructure);					//初始化MO
//	
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_InitStructure.GPIO_Pin = LD3320_IRQ;
//	GPIO_Init(LD3320_GPIO, &GPIO_InitStructure);					//初始化IRQ
	
	MX_GPIO_Init();
}

//void LD3320_EXTI_Init(void)
//{
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
//	
//	/*AFIO选择中断引脚*/
//	GPIO_EXTILineConfig(LD3320_GPIO_PortSourceGPIO, LD3320_GPIO_PinSource);
//	
//	EXTI_InitTypeDef EXTI_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//	
//	EXTI_InitStructure.EXTI_Line = LD3320_EXTI_Line;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//	EXTI_Init(&EXTI_InitStructure);
//	
//	NVIC_InitStructure.NVIC_IRQChannel = LD3320_EXTI_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
//	NVIC_Init(&NVIC_InitStructure);
//	
//	
//	/*NVIC中断分组*/
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);				//配置NVIC为分组2
//	
//}


void LD3320_WriteReg(uint8_t address, uint8_t data)
{
	uint8_t i = 0;
	uint8_t command = 0x04;			//写指令
	CS(0);
	Delay_Nop;
	
	//write command
	for(i = 0; i < 8; i++)
	{
		if(command & 0x80)
		{
			MI(1);
		}
		else
		{
			MI(0);
		}
		
		Delay_Nop;
		SCK(0);
		command = (command << 1);
		Delay_Nop;
		SCK(1);
	}
	//write address
	for(i = 0; i < 8; i++)
	{
		if(address & 0x80)
		{
			MI(1);
		}
		else
		{
			MI(0);
		}
		
		Delay_Nop;
		SCK(0);
		address = (address << 1);
		Delay_Nop;
		SCK(1);
	}
	//write data
	for(i = 0; i < 8; i++)
	{
		if(data & 0x80)
		{
			MI(1);
		}
		else
		{
			MI(0);
		}
		
		Delay_Nop;
		SCK(0);
		data = (data << 1);
		Delay_Nop;
		SCK(1);
	}
	Delay_Nop;
	CS(1);
}

uint8_t LD3320_ReadReg(uint8_t address)
{
	uint8_t i = 0;
	uint8_t datain = 0;
	uint8_t temp = 0;
	uint8_t command = 0x05;			//读指令
	CS(0);
	Delay_Nop;
	
	//write command
	for(i = 0; i < 8; i++)
	{
		if(command & 0x80)
		{
			MI(1);
		}
		else
		{
			MI(0);
		}
		Delay_Nop;
		SCK(0);
		command = (command << 1);
		Delay_Nop;
		SCK(1);
	}
	
	//write address
	for(i = 0; i < 8; i++)
	{
		if(address & 0x80)
		{
			MI(1);
		}
		else
		{
			MI(0);
		}
		Delay_Nop;
		SCK(0);
		address = (address << 1);
		Delay_Nop;
		SCK(1);
	}
	//Read
	Delay_Nop;
	for(i = 0 ;i < 8; i ++)
	{
		datain = (datain << 1);
		temp = MO;
		Delay_Nop;
		SCK(0);
		if(temp == 1)
			datain |= 0x01;
		Delay_Nop;
		SCK(1);
	}
	
	Delay_Nop;
	CS(1);
	return datain;
}

//void EXTI9_5_IRQHandler(void)
//{
//	if(EXTI_GetITStatus(LD3320_EXTI_Line) != RESET)
//	{
//		ProcessInt(); 
// 		printf("进入中断\r\n");
//		EXTI_ClearFlag(LD3320_EXTI_Line);
//		EXTI_ClearITPendingBit(LD3320_EXTI_Line);		//清除LINE上的中断标志位
//	}
//}




