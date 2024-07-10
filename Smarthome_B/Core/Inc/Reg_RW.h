#ifndef __REG_RW_H
#define __REG_RW_H


////开启时钟
//#define LD3320_RCC_APB2Periph_GPIO		RCC_APB2Periph_GPIOB

//LD3320引脚
#define LD3320_GPIO						GPIOB
#define LD3320_SCK						GPIO_PIN_3
#define LD3320_MI						GPIO_PIN_4
#define LD3320_MO						GPIO_PIN_5
#define LD3320_CS						GPIO_PIN_6
#define LD3320_RST						GPIO_PIN_2
#define LD3320_IRQ						GPIO_PIN_10

////AFIO引脚选择
//#define LD3320_GPIO_PortSourceGPIO		GPIO_PortSourceGPIOB
//#define LD3320_GPIO_PinSource			GPIO_PinSource8

////外部中断
//#define LD3320_EXTI_Line				EXTI_Line8
//#define LD3320_EXTI_IRQn				EXTI9_5_IRQn

//GPIO操作宏定义
#define SCK(x)		HAL_GPIO_WritePin(LD3320_GPIO, LD3320_SCK, (x) ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define MI(x)		HAL_GPIO_WritePin(LD3320_GPIO, LD3320_MI, (x) ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define CS(x)		HAL_GPIO_WritePin(LD3320_GPIO, LD3320_CS, (x) ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define RST(x)		HAL_GPIO_WritePin(LD3320_GPIO, LD3320_RST, (x) ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define MO			HAL_GPIO_ReadPin(LD3320_GPIO,LD3320_MO)
#define IRQ			HAL_GPIO_ReadPin(LD3320_GPIO,LD3320_IRQ)

//定义函数
void LD3320_Init(void);
//void LD3320_EXTI_Init(void);
void LD3320_WriteReg(uint8_t address, uint8_t data);
uint8_t LD3320_ReadReg(uint8_t address);



#endif

