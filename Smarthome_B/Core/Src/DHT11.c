#include "stm32f4xx.h"                  // Device header
#include "gpio.h"
#include "Delay.h"

void DHT11_Init(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.Pin = DHT11_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(GPIOA, DHT11_Pin, GPIO_PIN_SET);
//	MX_GPIO_Init();
}

void DHT11_Out(void)
{
	//�������
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = DHT11_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}

void DHT11_In(void)
{	
	//��������
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = DHT11_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}

void DHT11_Start(void)
{
	DHT11_Init();
	Delay_us(10);
	HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_RESET);
	Delay_ms(20);
	HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_SET);
	Delay_us(30);
	
	DHT11_In();
}

//��Ӧ�źţ�1��Ӧ��0����Ӧ
uint8_t DHT11_Receive_Bit(void)
{
	uint8_t data;
	Delay_us(40);
	if(HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == 0)
	{
		data = 1;
		while(!HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin));
		while(HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin));
	}
	else
		data = 0;
	return data;
}

uint8_t DHT11_Receive_Byte(void)
{
	uint8_t data = 0x00;
	for(uint8_t i = 0; i < 8; i++)
	{
		while(HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin));
		while(!HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin));	//ֱ�����ǵ͵�ƽ
		Delay_us(40);
		if(HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == 1)
		{
			data |= (0x80 >> i);
		}	
		while(HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin));	//ֱ�����Ǹߵ�ƽ
	}
	return data;
}
//���ϣ�������һ����0.��͵�ƽ��������ֽڡ������һ����1�����Ҳ�ǵ͵�ƽ����


uint8_t DHT11_Receive_Data(uint8_t *TH, uint8_t *TL, uint8_t *HH, uint8_t *HL)
{
	uint8_t Check;
	uint8_t flag;
	*HH = DHT11_Receive_Byte();
	*HL = DHT11_Receive_Byte();
	*TH = DHT11_Receive_Byte();
	*TL = DHT11_Receive_Byte();
	Check = DHT11_Receive_Byte();
	
	while(!HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin));
	
	if(*TH + *TL + *HH + *HL == Check)
		flag = 1;
	else
		flag = 0;
	return flag;
}
