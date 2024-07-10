#ifndef __LD3320_H
#define __LD3320_H


//	��������״̬����������¼������������ASRʶ����������MP3����
#define LD3320_MODE_IDLE			0x00
#define LD3320_MODE_ASR_RUN			0x08
#define LD3320_MODE_MP3		 		0x40


//	�������״̬����������¼������������ASRʶ������е��ĸ�״̬
#define LD_ASR_NONE				0x00		//	��ʾû������ASRʶ��
#define LD_ASR_RUNING			0x01		//	��ʾLD3320������ASRʶ����
#define LD_ASR_FOUNDOK			0x10		//	��ʾһ��ʶ�����̽�������һ��ʶ����
#define LD_ASR_FOUNDZERO 		0x11		//	��ʾһ��ʶ�����̽�����û��ʶ����
#define LD_ASR_ERROR	 		0x31		//	��ʾһ��ʶ��������LD3320оƬ�ڲ����ֲ���ȷ��״̬


#define CLK_IN   				24			/* �û�ע���޸�����ľ���ʱ�Ӵ�С */
#define LD_PLL_11				(uint8_t)((CLK_IN/2.0)-1)
#define LD_PLL_MP3_19			0x0f
#define LD_PLL_MP3_1B			0x18
#define LD_PLL_MP3_1D   		(uint8_t)(((90.0*((LD_PLL_11)+1))/(CLK_IN))-1)

#define LD_PLL_ASR_19 			(uint8_t)(CLK_IN*32.0/(LD_PLL_11+1) - 0.51)
#define LD_PLL_ASR_1B 			0x48
#define LD_PLL_ASR_1D 			0x1f

#define MIC_VOL 				0x43	 	//��ͷ����


//ʶ����ͻ��޸Ĵ�
#define CODE_CMD   		0x00   		//��������0x00�û����ɽ����޸ġ�
#define CODE_DMCS	 	0x01		//�������
#define CODE_CSWB  		0x02	  	//�������

#define CODE_1KL1 		0x03
#define CODE_1KL2 		0x04
#define CODE_1KL3 		0x05
#define CODE_1KL4 		0x06
						
#define CODE_2KL1 		0x18
#define CODE_2KL2 		0x19
#define CODE_2KL3		0x1A
#define CODE_2KL4 		0x1B
						
#define CODE_3KL1 		0x1C
#define CODE_3KL2		0x1D
#define CODE_3KL3 		0x1E
#define CODE_3KL4 		0x1F
						
#define CODE_4KL1 		0x20
#define CODE_4KL2		0x21
#define CODE_4KL3 		0x22
#define CODE_4KL4 		0x23
		
#define CODE_5KL1 		0x24

#define CODE_Light_ON	0x1A
#define CODE_Light_CLOSE	0x0A

#define CODE_Door_ON	0x1B
#define CODE_Door_CLOSE	0x0B

#define CODE_Window_ON	0x1C
#define CODE_Window_CLOSE	0x0C

#define CODE_Fan_ON	0x1D
#define CODE_Fan_CLOSE	0x0D




//��������
void LD3320_Reset(void);
void LD3320_Init_Command(void);
void LD3320_Init_ASR(void);
void ProcessInt(void);
//uint8_t LD3320_Check_ASRBusyFlag_b2(void);
void LD3320_AsrStart(void);
uint8_t LD3320_AsrRun(void);
uint8_t LD3320_AsrAddFixed(void);
uint8_t RunASR(void);
uint8_t LD3320_GetResult(void);

#endif

