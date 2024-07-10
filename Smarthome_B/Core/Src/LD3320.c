#include "stm32f4xx.h"                  // Device header

#include "LD3320.h"
#include "delay.h"
#include "Reg_RW.h"
#include "usart.h"

extern uint8_t nAsrStatus;
uint8_t  nLD3320_Mode = LD3320_MODE_IDLE;
uint8_t  ucRegVal;

//sprintf�ǽ����͵����ݷŽ�u_buf���顣���ҷ��ط������ݵĳ���
#define printf(...) HAL_UART_Transmit(&huart1,\
									 (uint8_t *)u_buf,\
									 sprintf((char*)u_buf,__VA_ARGS__),\
									 0xffff)

extern uint8_t u_buf[256];											//printf��ӳ��
/************************************************************************
���������� 	 ��λLD3320ģ��
��ڲ�����	 none
�� �� ֵ�� 	 none
����˵����	 none
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
���������� LD3320ģ�������ʼ��
��ڲ����� none
�� �� ֵ�� none
����˵���� �ú���Ϊ�������ã�һ�㲻��Ҫ�޸ģ�
					 ����Ȥ�Ŀͻ��ɶ��տ����ֲ������Ҫ�����޸ġ�
**************************************************************************/
void LD3320_Init_Command(void)
{
	LD3320_ReadReg(0x06);				//�����ԣ�����ȥ��
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
���������� 	 LD3320ģ�� ASR���ܳ�ʼ��
��ڲ�����	 none
�� �� ֵ�� 	 none
����˵����	 �ú���Ϊ�������ã�һ�㲻��Ҫ�޸ģ�
					 ����Ȥ�Ŀͻ��ɶ��տ����ֲ������Ҫ�����޸ġ�
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
���������� 	�жϴ�����
��ڲ�����	 none
�� �� ֵ�� 	 none
����˵����	��LDģ����յ���Ƶ�ź�ʱ��������ú�����
						�ж�ʶ���Ƿ��н�������û�д������ü�
            ����׼����һ�ε�ʶ��
**************************************************************************/
void ProcessInt(void)
{
	uint8_t nAsrResCount=0;

	ucRegVal = LD3320_ReadReg(0x2B);
	LD3320_WriteReg(0x29,0) ;
	LD3320_WriteReg(0x02,0) ;
	if((ucRegVal & 0x10)&&LD3320_ReadReg(0xb2)==0x21&&LD3320_ReadReg(0xbf)==0x35)			/*ʶ��ɹ�*/
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
	}															 /*û��ʶ����*/
	else
	{	 
		nAsrStatus=LD_ASR_FOUNDZERO;
	}
		
	LD3320_WriteReg(0x2b,0);
	LD3320_WriteReg(0x1C,0);									/*д0:ADC������*/
	LD3320_WriteReg(0x29,0);
	LD3320_WriteReg(0x02,0);
	LD3320_WriteReg(0x2B,0);
	LD3320_WriteReg(0xBA,0);	
	LD3320_WriteReg(0xBC,0);	
	LD3320_WriteReg(0x08,1);									/*���FIFO_DATA*/
	LD3320_WriteReg(0x08,0);									/*���FIFO_DATA�� �ٴ�д0*/
}

/************************************************************************
����������  ���LDģ���Ƿ����
��ڲ�����	none
�� �� ֵ�� 	flag��1-> ����
����˵����	none
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
���������� 	����ASR
��ڲ�����	none
�� �� ֵ�� 	none
����˵����	none
**************************************************************************/
void LD3320_AsrStart(void)
{
	LD3320_Init_ASR();
}

/************************************************************************
���������� 	����ASR
��ڲ�����	none
�� �� ֵ�� 	1�������ɹ�
����˵����	none
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
���������� ��LD3320ģ����ӹؼ���
��ڲ����� none
�� �� ֵ�� flag��1->��ӳɹ�
����˵���� �û��޸�.
					 1���������¸�ʽ���ƴ���ؼ��ʣ�ͬʱע���޸�sRecog ��pCode ����ĳ���
					 �Ͷ�Ӧ����k��ѭ��ֵ��ƴ������ʶ������һһ��Ӧ�ġ�
					 2�������߿���ѧϰ"����ʶ��оƬLD3320�߽��ؼ�.pdf"��
           ���������������մ�����÷������ṩʶ��Ч����
**************************************************************************/
uint8_t LD3320_AsrAddFixed(void)
{
	uint8_t k, flag;
	uint8_t nAsrAddLength;
	
	#define DATE_A 	14   	//�����ά��ֵ
	#define DATE_B 	20		//����һά��ֵ
	
	
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
										};	/*��ӹؼ��ʣ��û��޸�*/
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
							};	/*���ʶ���룬�û��޸�*/	
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
���������� 	����ASRʶ������
��ڲ�����	none
�� �� ֵ��  asrflag��1->�����ɹ��� 0��>����ʧ��
����˵����	ʶ��˳������:
						1��RunASR()����ʵ����һ��������ASR����ʶ������
						2��LD_AsrStart() ����ʵ����ASR��ʼ��
						3��LD_AsrAddFixed() ����ʵ������ӹؼ����ﵽLD3320оƬ��
						4��LD_AsrRun()	����������һ��ASR����ʶ������					
						�κ�һ��ASRʶ�����̣�����Ҫ�������˳�򣬴ӳ�ʼ����ʼ��
**************************************************************************/
uint8_t RunASR(void)
{
	uint8_t i=0;
	uint8_t asrflag=0;
	for (i=0; i<5; i++)			//	��ֹ����Ӳ��ԭ����LD3320оƬ����������������һ������5������ASRʶ������
	{
		LD3320_AsrStart();
		Delay_ms(5);
		if (LD3320_AsrAddFixed()==0)
		{
			LD3320_Reset();			//	LD3320оƬ�ڲ����ֲ���������������LD3320оƬ
			Delay_ms(5);			//	���ӳ�ʼ����ʼ����ASRʶ������
			continue;
		}
		Delay_ms(5);
		if (LD3320_AsrRun() == 0)
		{
			LD3320_Reset();			//	LD3320оƬ�ڲ����ֲ���������������LD3320оƬ
			Delay_ms(5);			//	���ӳ�ʼ����ʼ����ASRʶ������
			continue;
		}	
		asrflag=1;
		break;					//	ASR���������ɹ����˳���ǰforѭ������ʼ�ȴ�LD3320�ͳ����ж��ź�
	}
	return asrflag;
}

/************************************************************************
���������� 	��ȡʶ����
��ڲ�����	none
�� �� ֵ�� 	LD3320_ReadReg(0xc5 )��  ��ȡ�ڲ��Ĵ�������ʶ���롣
����˵����	none
**************************************************************************/
uint8_t LD3320_GetResult(void)
{	
	return LD3320_ReadReg(0xc5 );
}




