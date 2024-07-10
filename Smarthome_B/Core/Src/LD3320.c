#include "stm32f4xx.h"                  // Device header

#include "LD3320.h"
#include "delay.h"
#include "Reg_RW.h"
#include "usart.h"

extern uint8_t nAsrStatus;
uint8_t  nLD3320_Mode = LD3320_MODE_IDLE;
uint8_t  ucRegVal;

//sprintf是将发送的内容放进u_buf数组。并且返回发送内容的长度
#define printf(...) HAL_UART_Transmit(&huart1,\
									 (uint8_t *)u_buf,\
									 sprintf((char*)u_buf,__VA_ARGS__),\
									 0xffff)

extern uint8_t u_buf[256];											//printf重映像
/************************************************************************
功能描述： 	 复位LD3320模块
入口参数：	 none
返 回 值： 	 none
其他说明：	 none
**************************************************************************/
void LD3320_Reset(void)
{
	RST(1);
	Delay_ms(5);
	RST(0);
	Delay_ms(5);
	RST(1);
	
	Delay_ms(5);
	CS(0);
	Delay_ms(5);
	CS(1);
	Delay_ms(5);
}

/************************************************************************
功能描述： LD3320模块命令初始化
入口参数： none
返 回 值： none
其他说明： 该函数为出厂配置，一般不需要修改；
					 有兴趣的客户可对照开发手册根据需要自行修改。
**************************************************************************/
void LD3320_Init_Command(void)
{
	LD3320_ReadReg(0x06);				//经测试，可以去掉
	LD3320_WriteReg(0x17, 0x35);
	
	Delay_ms(5);
	LD3320_ReadReg(0x06);  

	LD3320_WriteReg(0x89, 0x03); 
	Delay_ms(5);
	LD3320_WriteReg(0xCF, 0x43);
	Delay_ms(5);
	LD3320_WriteReg(0xCB, 0x02);
	
	/*PLL setting*/
	LD3320_WriteReg(0x11, LD_PLL_11);
	if (nLD3320_Mode == LD3320_MODE_MP3)
	{
		LD3320_WriteReg(0x1E,0x00);
		LD3320_WriteReg(0x19, LD_PLL_MP3_19); 
		LD3320_WriteReg(0x1B, LD_PLL_MP3_1B);		
		LD3320_WriteReg(0x1D, LD_PLL_MP3_1D);
	}
	else
	{
		LD3320_WriteReg(0x1E,0x00);
		LD3320_WriteReg(0x19, LD_PLL_ASR_19); 
		LD3320_WriteReg(0x1B, LD_PLL_ASR_1B);
		LD3320_WriteReg(0x1D, LD_PLL_ASR_1D);
	}	
	Delay_ms(5);
	
	LD3320_WriteReg(0xCD, 0x04);
	LD3320_WriteReg(0x17, 0x4c);
	Delay_ms(5);
	LD3320_WriteReg(0xB9, 0x00);
	LD3320_WriteReg(0xCF, 0x4F);
	LD3320_WriteReg(0x6F, 0xFF);
}

/************************************************************************
功能描述： 	 LD3320模块 ASR功能初始化
入口参数：	 none
返 回 值： 	 none
其他说明：	 该函数为出厂配置，一般不需要修改；
					 有兴趣的客户可对照开发手册根据需要自行修改。
**************************************************************************/

void LD3320_Init_ASR(void)
{
	nLD3320_Mode=LD3320_MODE_ASR_RUN;
	LD3320_Init_Command();

	LD3320_WriteReg(0xBD, 0x00);
	LD3320_WriteReg(0x17, 0x48);
	Delay_ms(5);

	LD3320_WriteReg(0x3C, 0x80);
	LD3320_WriteReg(0x3E, 0x07);
	LD3320_WriteReg(0x38, 0xff); 
	LD3320_WriteReg(0x3A, 0x07);
	
	LD3320_WriteReg(0x40, 0);
	LD3320_WriteReg(0x42, 8);
	LD3320_WriteReg(0x44, 0); 
	LD3320_WriteReg(0x46, 8);
	Delay_ms(5);
}

/************************************************************************
功能描述： 	中断处理函数
入口参数：	 none
返 回 值： 	 none
其他说明：	当LD模块接收到音频信号时，将进入该函数，
						判断识别是否有结果，如果没有从新配置寄
            存器准备下一次的识别。
**************************************************************************/
void ProcessInt(void)
{
	uint8_t nAsrResCount=0;

	ucRegVal = LD3320_ReadReg(0x2B);
	LD3320_WriteReg(0x29,0) ;
	LD3320_WriteReg(0x02,0) ;
	if((ucRegVal & 0x10)&&LD3320_ReadReg(0xb2)==0x21&&LD3320_ReadReg(0xbf)==0x35)			/*识别成功*/
	{	
		nAsrResCount = LD3320_ReadReg(0xba);
		if(nAsrResCount>0 && nAsrResCount<=4) 
		{
			nAsrStatus=LD_ASR_FOUNDOK;
		}
		else
	  {
			nAsrStatus=LD_ASR_FOUNDZERO;
		}	
	}															 /*没有识别结果*/
	else
	{	 
		nAsrStatus=LD_ASR_FOUNDZERO;
	}
		
	LD3320_WriteReg(0x2b,0);
	LD3320_WriteReg(0x1C,0);									/*写0:ADC不可用*/
	LD3320_WriteReg(0x29,0);
	LD3320_WriteReg(0x02,0);
	LD3320_WriteReg(0x2B,0);
	LD3320_WriteReg(0xBA,0);	
	LD3320_WriteReg(0xBC,0);	
	LD3320_WriteReg(0x08,1);									/*清除FIFO_DATA*/
	LD3320_WriteReg(0x08,0);									/*清除FIFO_DATA后 再次写0*/
}

/************************************************************************
功能描述：  检测LD模块是否空闲
入口参数：	none
返 回 值： 	flag：1-> 空闲
其他说明：	none
**************************************************************************/
uint8_t LD3320_Check_ASRBusyFlag_b2(void)
{
	uint8_t j,i;
	uint8_t flag = 0;
	for (j=0; j<5; j++)
	{
		i=LD3320_ReadReg(0xb2);
		if ( i== 0x21)
		{
			flag = 1;						
			break;
		}
		Delay_ms(20);
	}
	return flag;
}

/************************************************************************
功能描述： 	启动ASR
入口参数：	none
返 回 值： 	none
其他说明：	none
**************************************************************************/
void LD3320_AsrStart(void)
{
	LD3320_Init_ASR();
}

/************************************************************************
功能描述： 	运行ASR
入口参数：	none
返 回 值： 	1：启动成功
其他说明：	none
**************************************************************************/
uint8_t LD3320_AsrRun(void)
{
	LD3320_WriteReg(0x35, MIC_VOL);
	LD3320_WriteReg(0x1C, 0x09);
	LD3320_WriteReg(0xBD, 0x20);
	LD3320_WriteReg(0x08, 0x01);
	Delay_ms(5);
	LD3320_WriteReg(0x08, 0x00);
	Delay_ms(5);

	if(LD3320_Check_ASRBusyFlag_b2() == 0)
	{
		return 0;
	}

	LD3320_WriteReg(0xB2, 0xff);
	LD3320_WriteReg(0x37, 0x06);
	Delay_ms(5);
	LD3320_WriteReg(0x37, 0x06);
	Delay_ms(5);
	LD3320_WriteReg(0x1C, 0x0b);
	LD3320_WriteReg(0x29, 0x10);	
	LD3320_WriteReg(0xBD, 0x00);
	return 1;
}

/************************************************************************
功能描述： 向LD3320模块添加关键词
入口参数： none
返 回 值： flag：1->添加成功
其他说明： 用户修改.
					 1、根据如下格式添加拼音关键词，同时注意修改sRecog 和pCode 数组的长度
					 和对应变量k的循环值。拼音串和识别码是一一对应的。
					 2、开发者可以学习"语音识别芯片LD3320高阶秘籍.pdf"中
           关于垃圾词语吸收错误的用法，来提供识别效果。
**************************************************************************/
uint8_t LD3320_AsrAddFixed(void)
{
	uint8_t k, flag;
	uint8_t nAsrAddLength;
	
	#define DATE_A 	14   	//数组二维数值
	#define DATE_B 	20		//数组一维数值
	
	
	uint8_t sRecog[DATE_A][DATE_B] = {
										"xiao ai tong xue",\
										"dai ma ce shi",\
										"ce shi wan bi",\
		
										"kai deng",\
										"guan deng",\
										"da kai feng shan",\
										"guan bi feng shan",\
		
										"kai chuang",\
										"guan chuang",\
										"kai suo",\
										"xiang you zhuan",\
		
										"da kai kong tiao",\
										"guan bi kong tiao",\
										"hou tui",\
										};	/*添加关键词，用户修改*/
	uint8_t pCode[DATE_A] = {
								CODE_CMD,\
								CODE_DMCS,\
								CODE_CSWB,\

								CODE_1KL1,\
								CODE_1KL2,\
								CODE_1KL3,\
								CODE_1KL4,\

								CODE_2KL1,\
								CODE_2KL2,\
								CODE_2KL3,\
								CODE_2KL4,\

								CODE_3KL1,\
								CODE_3KL2,\
								CODE_5KL1,
							};	/*添加识别码，用户修改*/	
	flag = 1;
	for (k=0; k<DATE_A; k++)
	{
			
		if(LD3320_Check_ASRBusyFlag_b2() == 0)
		{
			flag = 0;
			break;
		}
		
		LD3320_WriteReg(0xc1, pCode[k] );
		LD3320_WriteReg(0xc3, 0 );
		LD3320_WriteReg(0x08, 0x04);
		Delay_ms(1);
		LD3320_WriteReg(0x08, 0x00);
		Delay_ms(1);

		for (nAsrAddLength=0; nAsrAddLength<DATE_B; nAsrAddLength++)
		{
			if (sRecog[k][nAsrAddLength] == 0)
				break;
			LD3320_WriteReg(0x5, sRecog[k][nAsrAddLength]);
		}
		LD3320_WriteReg(0xb9, nAsrAddLength);
		LD3320_WriteReg(0xb2, 0xff);
		LD3320_WriteReg(0x37, 0x04);
	}
    return flag;
}

/************************************************************************
功能描述： 	运行ASR识别流程
入口参数：	none
返 回 值：  asrflag：1->启动成功， 0—>启动失败
其他说明：	识别顺序如下:
						1、RunASR()函数实现了一次完整的ASR语音识别流程
						2、LD_AsrStart() 函数实现了ASR初始化
						3、LD_AsrAddFixed() 函数实现了添加关键词语到LD3320芯片中
						4、LD_AsrRun()	函数启动了一次ASR语音识别流程					
						任何一次ASR识别流程，都需要按照这个顺序，从初始化开始
**************************************************************************/
uint8_t RunASR(void)
{
	uint8_t i=0;
	uint8_t asrflag=0;
	for (i=0; i<5; i++)			//	防止由于硬件原因导致LD3320芯片工作不正常，所以一共尝试5次启动ASR识别流程
	{
		LD3320_AsrStart();
		Delay_ms(5);
		if (LD3320_AsrAddFixed()==0)
		{
			LD3320_Reset();			//	LD3320芯片内部出现不正常，立即重启LD3320芯片
			Delay_ms(5);			//	并从初始化开始重新ASR识别流程
			continue;
		}
		Delay_ms(5);
		if (LD3320_AsrRun() == 0)
		{
			LD3320_Reset();			//	LD3320芯片内部出现不正常，立即重启LD3320芯片
			Delay_ms(5);			//	并从初始化开始重新ASR识别流程
			continue;
		}	
		asrflag=1;
		break;					//	ASR流程启动成功，退出当前for循环。开始等待LD3320送出的中断信号
	}
	return asrflag;
}

/************************************************************************
功能描述： 	获取识别结果
入口参数：	none
返 回 值： 	LD3320_ReadReg(0xc5 )；  读取内部寄存器返回识别码。
其他说明：	none
**************************************************************************/
uint8_t LD3320_GetResult(void)
{	
	return LD3320_ReadReg(0xc5 );
}




