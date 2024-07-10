#include "stm32f4xx.h"                  // Device header
#include <string.h>
#include <stdio.h>
#include "delay.h"
#include "usart.h"
#include "as608.h"
#include "OLED.h"
#include "Keypad.h"

uint32_t AS608Addr = 0XFFFFFFFF; 	//Ĭ��
char str2[6] = {0};
uint8_t key_num=0;

//���ڷ���һ���ֽ�
static void MyUsart_SendByte(uint8_t Byte)
{
	HAL_UART_Transmit(&huart6, &Byte, 1, HAL_MAX_DELAY);
//	USART_SendData(USART2, Byte);		//���ֽ�����д�����ݼĴ�����д���USART�Զ�����ʱ����
//	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	/*�´�д�����ݼĴ������Զ����������ɱ�־λ���ʴ�ѭ�������������־λ*/
	
//	while((USART2->SR & 0X40) == 0);
//	USART2->DR = Byte;
}

//���Ͱ�ͷ
static void SendHead(void)
{
	MyUsart_SendByte(0xEF);
	MyUsart_SendByte(0x01);
}
//���͵�ַ
static void SendAddr(void)
{
	MyUsart_SendByte(AS608Addr >> 24);
	MyUsart_SendByte(AS608Addr >> 16);
	MyUsart_SendByte(AS608Addr >> 8);
	MyUsart_SendByte(AS608Addr);
}
//���Ͱ���ʶ
static void SendFlag(uint8_t flag)
{
	MyUsart_SendByte(flag);
}
//���Ͱ�����
static void SendLength(int length)
{
	MyUsart_SendByte(length >> 8);
	MyUsart_SendByte(length);
}
//����ָ����
static void Sendcmd(uint8_t cmd)
{
	MyUsart_SendByte(cmd);
}
//����У���
static void SendCheck(uint16_t check)
{
  MyUsart_SendByte(check >> 8);
  MyUsart_SendByte(check);
}
//�ж��жϽ��յ�������û��Ӧ���
//waittimeΪ�ȴ��жϽ������ݵ�ʱ�䣨��λ1ms��
//����ֵ�����ݰ��׵�ַ
static uint8_t *JudgeStr(uint16_t waittime)
{
  char *data;
  uint8_t str[8];
  str[0] = 0xEF;
  str[1] = 0x01;
  str[2] = AS608Addr >> 24;
  str[3] = AS608Addr >> 16;
  str[4] = AS608Addr >> 8;
  str[5] = AS608Addr;
  str[6] = 0x07;
  str[7] = '\0';
//  USART6_RX_STA = 0;
	
	//HAL���д��
	HAL_UART_Receive(&huart6,(uint8_t *)USART6_RX_BUF,USART6_MAX_RECV_LEN,waittime/4);//����6��������
	if(!memcmp(str,USART6_RX_BUF,7))//�ȶ�����
	{
		data = strstr((const char*)USART6_RX_BUF, (const char*)str);
		if(data)
			return (uint8_t*)data;
	}
//  while(--waittime)
//  {
//    Delay_ms(1);
//    if(USART6_RX_STA & 0X8000) //���յ�һ������
//    {
//      USART6_RX_STA = 0;
//      data = strstr((const char*)USART6_RX_BUF, (const char*)str);
//      if(data)
//        return (uint8_t*)data;
//    }
//  }
  return 0;
}
//¼��ͼ�� PS_GetImage
//����:̽����ָ��̽�⵽��¼��ָ��ͼ�����ImageBuffer��
//ģ�鷵��ȷ����
uint8_t PS_GetImage(void)
{
	uint16_t temp;
	uint8_t ensure;
	uint8_t *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x01);
	temp =  0x01 + 0x03 + 0x01;
	SendCheck(temp);
	data = JudgeStr(2000);
	if(data)
		ensure = data[9];
	else
		ensure = 0xff;
	return ensure;
}
//�������� PS_GenChar
//����:��ImageBuffer�е�ԭʼͼ������ָ�������ļ�����CharBuffer1��CharBuffer2
//����:BufferID --> charBuffer1:0x01	charBuffer1:0x02
//ģ�鷵��ȷ����
uint8_t PS_GenChar(uint8_t BufferID)
{
  uint16_t temp;
  uint8_t  ensure;
  uint8_t  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x04);
  Sendcmd(0x02);
  MyUsart_SendByte(BufferID);
  temp = 0x01 + 0x04 + 0x02 + BufferID;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}
//��ȷ�ȶ���öָ������ PS_Match
//����:��ȷ�ȶ�CharBuffer1 ��CharBuffer2 �е������ļ�
//ģ�鷵��ȷ����
uint8_t PS_Match(void)
{
  uint16_t temp;
  uint8_t  ensure;
  uint8_t  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x03);
  Sendcmd(0x03);
  temp = 0x01 + 0x03 + 0x03;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}
//����ָ�� PS_Search
//����:��CharBuffer1��CharBuffer2�е������ļ����������򲿷�ָ�ƿ�.�����������򷵻�ҳ�롣
//����:  BufferID @ref CharBuffer1	CharBuffer2
//˵��:  ģ�鷵��ȷ���֣�ҳ�루����ָ��ģ�壩
uint8_t PS_Search(uint8_t BufferID, uint16_t StartPage, uint16_t PageNum, SearchResult *p)
{
  uint16_t temp;
  uint8_t  ensure;
  uint8_t  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x08);
  Sendcmd(0x04);
  MyUsart_SendByte(BufferID);
  MyUsart_SendByte(StartPage >> 8);
  MyUsart_SendByte(StartPage);
  MyUsart_SendByte(PageNum >> 8);
  MyUsart_SendByte(PageNum);
  temp = 0x01 + 0x08 + 0x04 + BufferID
         + (StartPage >> 8) + (uint8_t)StartPage
         + (PageNum >> 8) + (uint8_t)PageNum;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
  {
    ensure = data[9];
    p->pageID   = (data[10] << 8) + data[11];
    p->mathscore = (data[12] << 8) + data[13];
  }
  else
    ensure = 0xff;
  return ensure;
}
//�ϲ�����������ģ�壩PS_RegModel
//����:��CharBuffer1��CharBuffer2�е������ļ��ϲ����� ģ��,�������CharBuffer1��CharBuffer2
//˵��:  ģ�鷵��ȷ����
uint8_t PS_RegModel(void)
{
  uint16_t temp;
  uint8_t  ensure;
  uint8_t  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x03);
  Sendcmd(0x05);
  temp = 0x01 + 0x03 + 0x05;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}
//����ģ�� PS_StoreChar
//����:�� CharBuffer1 �� CharBuffer2 �е�ģ���ļ��浽 PageID ��flash���ݿ�λ�á�
//����:  BufferID @ref charBuffer1:0x01	charBuffer1:0x02
//       PageID��ָ�ƿ�λ�úţ�
//˵��:  ģ�鷵��ȷ����
uint8_t PS_StoreChar(uint8_t BufferID, uint16_t PageID)
{
  uint16_t temp;
  uint8_t  ensure;
  uint8_t  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x06);
  Sendcmd(0x06);
  MyUsart_SendByte(BufferID);
  MyUsart_SendByte(PageID >> 8);
  MyUsart_SendByte(PageID);
  temp = 0x01 + 0x06 + 0x06 + BufferID
         + (PageID >> 8) + (uint8_t)PageID;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}
//ɾ��ģ�� PS_DeletChar
//����:  ɾ��flash���ݿ���ָ��ID�ſ�ʼ��N��ָ��ģ��
//����:  PageID(ָ�ƿ�ģ���)��Nɾ����ģ�������
//˵��:  ģ�鷵��ȷ����
uint8_t PS_DeletChar(uint16_t PageID, uint16_t N)
{
  uint16_t temp;
  uint8_t  ensure;
  uint8_t  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x07);
  Sendcmd(0x0C);
  MyUsart_SendByte(PageID >> 8);
  MyUsart_SendByte(PageID);
  MyUsart_SendByte(N >> 8);
  MyUsart_SendByte(N);
  temp = 0x01 + 0x07 + 0x0C
         + (PageID >> 8) + (uint8_t)PageID
         + (N >> 8) + (uint8_t)N;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}
//���ָ�ƿ� PS_Empty
//����:  ɾ��flash���ݿ�������ָ��ģ��
//����:  ��
//˵��:  ģ�鷵��ȷ����
uint8_t PS_Empty(void)
{
  uint16_t temp;
  uint8_t  ensure;
  uint8_t  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x03);
  Sendcmd(0x0D);
  temp = 0x01 + 0x03 + 0x0D;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}

//��������PS_HighSpeedSearch
//���ܣ��� CharBuffer1��CharBuffer2�е������ļ��������������򲿷�ָ�ƿ⡣
//		  �����������򷵻�ҳ��,��ָ����ڵ�ȷ������ָ�ƿ��� ���ҵ�¼ʱ����
//		  �ܺõ�ָ�ƣ���ܿ�������������
//����:  BufferID�� StartPage(��ʼҳ)��PageNum��ҳ����
//˵��:  ģ�鷵��ȷ����+ҳ�루����ָ��ģ�壩
uint8_t PS_HighSpeedSearch(uint8_t BufferID, uint16_t StartPage, uint16_t PageNum, SearchResult *p)
{
  uint16_t temp;
  uint8_t  ensure;
  uint8_t  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x08);
  Sendcmd(0x1b);
  MyUsart_SendByte(BufferID);
  MyUsart_SendByte(StartPage >> 8);
  MyUsart_SendByte(StartPage);
  MyUsart_SendByte(PageNum >> 8);
  MyUsart_SendByte(PageNum);
  temp = 0x01 + 0x08 + 0x1b + BufferID
         + (StartPage >> 8) + (uint8_t)StartPage
         + (PageNum >> 8) + (uint8_t)PageNum;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
  {
    ensure = data[9];
    p->pageID 	= (data[10] << 8) + data[11];
    p->mathscore = (data[12] << 8) + data[13];
  }
  else
    ensure = 0xff;
  return ensure;
}


//��ʾȷ���������Ϣ
void ShowErrMessage(uint8_t ensure)
{
//  OLED_ShowString(5,0,(u8*)EnsureMessage(ensure),OLED_8X16);
}
//¼ָ��
void Add_FR(void)
{
  uint8_t i, ensure, processnum = 0;
  uint8_t ID_NUM = 0;
  while(1)
  {
    switch (processnum)
    {
    case 0:
      i++;
      OLED_ShowChinese(32,16,"�밴��ָ");
	  OLED_Update();
      ensure = PS_GetImage();
      if(ensure == 0x00)
      {
        ensure = PS_GenChar(CharBuffer1); //��������
        if(ensure == 0x00)
        {
          OLED_ShowChinese(32, 16, "ָ������");
          OLED_ShowString(0, 32, "                ",OLED_8X16);
			OLED_Update();
          i = 0;
          processnum = 1; //�����ڶ���
        }
        else ShowErrMessage(ensure);
      }
      else ShowErrMessage(ensure);
      break;

    case 1:
      i++;
      OLED_ShowChinese(24, 16, "���ٰ�һ��");
      OLED_ShowString(0, 32, "                ",OLED_8X16);
	OLED_Update();
      ensure = PS_GetImage();
      if(ensure == 0x00)
      {
        ensure = PS_GenChar(CharBuffer2); //��������
        if(ensure == 0x00)
        {
		  OLED_ShowString(24, 16, "                ",OLED_8X16);
          OLED_ShowChinese(32, 16, "ָ������");
          OLED_ShowString(0, 32, "                ",OLED_8X16);
			OLED_Update();
          i = 0;
          processnum = 2; //����������
        }
        else ShowErrMessage(ensure);
      }
      else ShowErrMessage(ensure);
      break;

    case 2:
      OLED_ShowChinese(16, 16, "�Ա�����ָ��");
      OLED_ShowString(0, 32, "                ",OLED_8X16);
	  OLED_Update();
      ensure = PS_Match();
      if(ensure == 0x00)
      {
		OLED_ShowString(0, 16, "                ",OLED_8X16);
        OLED_ShowChinese(32, 16, "�Աȳɹ�");
        OLED_ShowString(0, 32, "                ",OLED_8X16);
		  OLED_Update();
        processnum = 3; //�������Ĳ�
      }
      else
      {
        OLED_ShowChinese(32, 16, "�Ա�ʧ��");
        OLED_ShowString(0, 32, "                ",OLED_8X16);
		  OLED_Update();
        ShowErrMessage(ensure);
        i = 0;
        processnum = 0; //���ص�һ��
      }
      Delay_ms(500);
      break;

    case 3:
      OLED_ShowChinese(16, 16, "����ָ��ģ��");
      OLED_ShowString(0, 32, "                ",OLED_8X16);
		OLED_Update();
      Delay_ms(500);
      ensure = PS_RegModel();
      if(ensure == 0x00)
      {
        OLED_ShowChinese(0, 16, "����ָ��ģ��ɹ�");
        OLED_ShowString(0, 32, "                ",OLED_8X16);
		  OLED_Update();
        processnum = 4; //�������岽
      }
      else
      {
        processnum = 0;
        ShowErrMessage(ensure);
      }
      Delay_ms(1000);
      break;

    case 4:
      OLED_ShowChinese(8, 0, "��  ��  ��  ��");
	  OLED_ShowString(24,0,"K4",OLED_8X16);
	  OLED_ShowString(56,0,", ",OLED_8X16);
	  OLED_ShowString(88,0,"K2",OLED_8X16);
	  OLED_ShowString(0, 16, "                ",OLED_8X16);
      OLED_ShowChinese(32, 16, "��  ����");
	  OLED_ShowString(48,16,"K3",OLED_8X16);
      OLED_ShowString(0, 32, "  0=< ID <=99   ", OLED_8X16);
	  OLED_Update();
      while(key_num != 3)
      {
        key_num = Keypad_scan();
        if(key_num == 2)
        {
          key_num = 0;
          if(ID_NUM > 0)
            ID_NUM--;
        }
        if(key_num == 4)
        {
          key_num = 0;
          if(ID_NUM < 99)
            ID_NUM++;
        }
        OLED_ShowString(40, 48, "ID=", OLED_8X16);
        OLED_ShowNum(65, 48, ID_NUM, 2, OLED_8X16);
		OLED_Update();
      }
      key_num = 0;
      ensure = PS_StoreChar(CharBuffer2, ID_NUM); //����ģ��
      if(ensure == 0x00)
      {
		OLED_Clear();
		OLED_ShowChinese(16, 16, "¼��ָ�Ƴɹ�");
		OLED_ShowString(0, 32, "                ",OLED_8X16);
		OLED_Update();
		Delay_ms(1500);
		OLED_Clear();
		OLED_ShowChinese(0,0,"ָ��ģ����Գ���");
		OLED_ShowString(16,16,"K1",OLED_8X16);
		OLED_ShowChinese(32,16,"�����ָ��");
		OLED_ShowString(16,32,"K3",OLED_8X16);
		OLED_ShowChinese(32,32,"��ɾ��ָ��");
		OLED_ShowString(16,48,"K5",OLED_8X16);
		OLED_ShowChinese(32,48,"����ָ֤��");
		OLED_Update();
		return ;
      }
      else
      {
        OLED_Clear();
        processnum = 0;
        ShowErrMessage(ensure);
      }
      break;
    }
    Delay_ms(400);
    if(i == 10) //����5��û�а���ָ���˳�
    {
      break;
    }
  }
}

SysPara AS608Para;//ָ��ģ��AS608����
//ˢָ��
uint8_t press_FR(void)
{
  SearchResult seach;
  uint8_t ensure;
  while(key_num != 1)
  {
    key_num = Keypad_scan();
    ensure = PS_GetImage();
    if(ensure == 0x00) //��ȡͼ��ɹ�
    {
      ensure = PS_GenChar(CharBuffer1);
      if(ensure == 0x00) //���������ɹ�
      {
        ensure = PS_HighSpeedSearch(CharBuffer1, 0, 99, &seach);
        if(ensure == 0x00) //�����ɹ�
        {
			OLED_ShowChinese(16, 16, "ָ����֤�ɹ�");
			OLED_Update();
			OLED_ShowString(8, 32, "ID:       :", OLED_8X16);
			OLED_ShowChinese(56, 32, "�÷�");
			OLED_ShowNum(32, 32, seach.pageID, 2, OLED_8X16);
			OLED_ShowNum(96, 32, seach.mathscore, 2, OLED_8X16);
			OLED_Update();
			Delay_ms(1500);
			OLED_ShowChinese(48, 48, "����");
			Delay_ms(1500);
			OLED_ShowString(0, 48, "                ", OLED_8X16);
			return 1;
        }
        else
        {
          OLED_ShowChinese(32, 16, "��֤ʧ��");
          Delay_ms(1500);
			return 0;
        }
      }
      else
			{};
      OLED_Clear();
      OLED_ShowChinese(32, 16, "�밴��ָ");
				OLED_Update();
    }
  }
  OLED_Clear();
  OLED_ShowChinese(32, 0, "ָ�ƿ���");
  OLED_ShowString(16,16,"K1",OLED_8X16);
  OLED_ShowChinese(32,16,"�����ָ��");
  OLED_ShowString(16,32,"K3",OLED_8X16);
  OLED_ShowChinese(32,32,"��ɾ��ָ��");
  OLED_ShowString(16,48,"K5",OLED_8X16);
  OLED_ShowChinese(32,48,"����ָ֤��");
  OLED_Update();
  return 0;
}

//ɾ��ָ��
void Del_FR(void)
{
	uint8_t  ensure;
	uint16_t ID_NUM = 0;
	OLED_ShowChinese(0, 0, "  ��   ��   ȷ��");
	OLED_ShowString(0, 0, "K4", OLED_8X16);
	OLED_ShowString(32, 0, ",K2", OLED_8X16);
	OLED_ShowChinese(56, 0, "��");					//һ��Ҫ�У���Ȼǰ�ߵļ����ʺ�
	OLED_ShowString(72, 0, ",K3", OLED_8X16);
	OLED_ShowChinese(16, 16, "  ���ָ�ƿ�");
	OLED_ShowString(16, 16, "K5", OLED_8X16);
	OLED_ShowChinese(0, 32, "  ����");
	OLED_ShowString(0, 32, "K1", OLED_8X16);
	OLED_ShowString(48, 32, " 0=<ID<=99", OLED_8X16);
	OLED_Update();
  while(key_num != 3)
  {
    key_num = Keypad_scan();
    if(key_num == 2)
    {
      key_num = 0;
      if(ID_NUM > 0)
        ID_NUM--;
    }
    if(key_num == 4)
    {
      key_num = 0;
      if(ID_NUM < 99)
        ID_NUM++;
    }
    if(key_num == 1)
      goto MENU ; //������ҳ��
    if(key_num == 5)
    {
      key_num = 0;
      ensure = PS_Empty(); //���ָ�ƿ�
      if(ensure == 0)
      {
        OLED_Clear();
        OLED_ShowChinese(8, 16, "���ָ�ƿ�ɹ�");
		OLED_Update();
      }
      else
        ShowErrMessage(ensure);
      Delay_ms(1500);
      goto MENU ; //������ҳ��
    }
    OLED_ShowString(40, 48, "ID=", OLED_8X16);
    OLED_ShowNum(65, 48, ID_NUM, 2, OLED_8X16);
	OLED_Update();
  }
  ensure = PS_DeletChar(ID_NUM, 1); //ɾ������ָ��
  if(ensure == 0)
  {
	OLED_Clear();
	OLED_ShowChinese(16, 16, "ɾ��ָ�Ƴɹ�");
	OLED_Update();
  }
  else
    ShowErrMessage(ensure);
  Delay_ms(1500);
MENU:
  OLED_Clear();
  OLED_ShowChinese(32, 0, "ָ�ƿ���");
  OLED_ShowString(16,16,"K1",OLED_8X16);
  OLED_ShowChinese(32,16,"�����ָ��");
  OLED_ShowString(16,32,"K3",OLED_8X16);
  OLED_ShowChinese(32,32,"��ɾ��ָ��");
  OLED_ShowString(16,48,"K5",OLED_8X16);
  OLED_ShowChinese(32,48,"����ָ֤��");
  OLED_Update();
  key_num = 0;
}
