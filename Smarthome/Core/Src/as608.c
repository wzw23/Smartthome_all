#include "stm32f4xx.h"                  // Device header
#include <string.h>
#include <stdio.h>
#include "delay.h"
#include "usart.h"
#include "as608.h"
#include "OLED.h"
#include "Keypad.h"

uint32_t AS608Addr = 0XFFFFFFFF; 	//默认
char str2[6] = {0};
uint8_t key_num=0;

//串口发送一个字节
static void MyUsart_SendByte(uint8_t Byte)
{
	HAL_UART_Transmit(&huart6, &Byte, 1, HAL_MAX_DELAY);
//	USART_SendData(USART2, Byte);		//将字节数据写入数据寄存器，写入后USART自动生成时序波形
//	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	/*下次写入数据寄存器会自动清除发送完成标志位，故此循环后，无需清除标志位*/
	
//	while((USART2->SR & 0X40) == 0);
//	USART2->DR = Byte;
}

//发送包头
static void SendHead(void)
{
	MyUsart_SendByte(0xEF);
	MyUsart_SendByte(0x01);
}
//发送地址
static void SendAddr(void)
{
	MyUsart_SendByte(AS608Addr >> 24);
	MyUsart_SendByte(AS608Addr >> 16);
	MyUsart_SendByte(AS608Addr >> 8);
	MyUsart_SendByte(AS608Addr);
}
//发送包标识
static void SendFlag(uint8_t flag)
{
	MyUsart_SendByte(flag);
}
//发送包长度
static void SendLength(int length)
{
	MyUsart_SendByte(length >> 8);
	MyUsart_SendByte(length);
}
//发送指令码
static void Sendcmd(uint8_t cmd)
{
	MyUsart_SendByte(cmd);
}
//发送校验和
static void SendCheck(uint16_t check)
{
  MyUsart_SendByte(check >> 8);
  MyUsart_SendByte(check);
}
//判断中断接收的数组有没有应答包
//waittime为等待中断接收数据的时间（单位1ms）
//返回值：数据包首地址
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
	
	//HAL库的写法
	HAL_UART_Receive(&huart6,(uint8_t *)USART6_RX_BUF,USART6_MAX_RECV_LEN,waittime/4);//串口6接收数据
	if(!memcmp(str,USART6_RX_BUF,7))//比对数据
	{
		data = strstr((const char*)USART6_RX_BUF, (const char*)str);
		if(data)
			return (uint8_t*)data;
	}
//  while(--waittime)
//  {
//    Delay_ms(1);
//    if(USART6_RX_STA & 0X8000) //接收到一次数据
//    {
//      USART6_RX_STA = 0;
//      data = strstr((const char*)USART6_RX_BUF, (const char*)str);
//      if(data)
//        return (uint8_t*)data;
//    }
//  }
  return 0;
}
//录入图像 PS_GetImage
//功能:探测手指，探测到后录入指纹图像存于ImageBuffer。
//模块返回确认字
uint8_t PS_GetImage(void)
{
	uint16_t temp;
	uint8_t ensure;
	uint8_t *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//命令包标识
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
//生成特征 PS_GenChar
//功能:将ImageBuffer中的原始图像生成指纹特征文件存于CharBuffer1或CharBuffer2
//参数:BufferID --> charBuffer1:0x01	charBuffer1:0x02
//模块返回确认字
uint8_t PS_GenChar(uint8_t BufferID)
{
  uint16_t temp;
  uint8_t  ensure;
  uint8_t  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
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
//精确比对两枚指纹特征 PS_Match
//功能:精确比对CharBuffer1 与CharBuffer2 中的特征文件
//模块返回确认字
uint8_t PS_Match(void)
{
  uint16_t temp;
  uint8_t  ensure;
  uint8_t  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
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
//搜索指纹 PS_Search
//功能:以CharBuffer1或CharBuffer2中的特征文件搜索整个或部分指纹库.若搜索到，则返回页码。
//参数:  BufferID @ref CharBuffer1	CharBuffer2
//说明:  模块返回确认字，页码（相配指纹模板）
uint8_t PS_Search(uint8_t BufferID, uint16_t StartPage, uint16_t PageNum, SearchResult *p)
{
  uint16_t temp;
  uint8_t  ensure;
  uint8_t  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
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
//合并特征（生成模板）PS_RegModel
//功能:将CharBuffer1与CharBuffer2中的特征文件合并生成 模板,结果存于CharBuffer1与CharBuffer2
//说明:  模块返回确认字
uint8_t PS_RegModel(void)
{
  uint16_t temp;
  uint8_t  ensure;
  uint8_t  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
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
//储存模板 PS_StoreChar
//功能:将 CharBuffer1 或 CharBuffer2 中的模板文件存到 PageID 号flash数据库位置。
//参数:  BufferID @ref charBuffer1:0x01	charBuffer1:0x02
//       PageID（指纹库位置号）
//说明:  模块返回确认字
uint8_t PS_StoreChar(uint8_t BufferID, uint16_t PageID)
{
  uint16_t temp;
  uint8_t  ensure;
  uint8_t  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
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
//删除模板 PS_DeletChar
//功能:  删除flash数据库中指定ID号开始的N个指纹模板
//参数:  PageID(指纹库模板号)，N删除的模板个数。
//说明:  模块返回确认字
uint8_t PS_DeletChar(uint16_t PageID, uint16_t N)
{
  uint16_t temp;
  uint8_t  ensure;
  uint8_t  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
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
//清空指纹库 PS_Empty
//功能:  删除flash数据库中所有指纹模板
//参数:  无
//说明:  模块返回确认字
uint8_t PS_Empty(void)
{
  uint16_t temp;
  uint8_t  ensure;
  uint8_t  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
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

//高速搜索PS_HighSpeedSearch
//功能：以 CharBuffer1或CharBuffer2中的特征文件高速搜索整个或部分指纹库。
//		  若搜索到，则返回页码,该指令对于的确存在于指纹库中 ，且登录时质量
//		  很好的指纹，会很快给出搜索结果。
//参数:  BufferID， StartPage(起始页)，PageNum（页数）
//说明:  模块返回确认字+页码（相配指纹模板）
uint8_t PS_HighSpeedSearch(uint8_t BufferID, uint16_t StartPage, uint16_t PageNum, SearchResult *p)
{
  uint16_t temp;
  uint8_t  ensure;
  uint8_t  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//命令包标识
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


//显示确认码错误信息
void ShowErrMessage(uint8_t ensure)
{
//  OLED_ShowString(5,0,(u8*)EnsureMessage(ensure),OLED_8X16);
}
//录指纹
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
      OLED_ShowChinese(32,16,"请按手指");
	  OLED_Update();
      ensure = PS_GetImage();
      if(ensure == 0x00)
      {
        ensure = PS_GenChar(CharBuffer1); //生成特征
        if(ensure == 0x00)
        {
          OLED_ShowChinese(32, 16, "指纹正常");
          OLED_ShowString(0, 32, "                ",OLED_8X16);
			OLED_Update();
          i = 0;
          processnum = 1; //跳到第二步
        }
        else ShowErrMessage(ensure);
      }
      else ShowErrMessage(ensure);
      break;

    case 1:
      i++;
      OLED_ShowChinese(24, 16, "请再按一次");
      OLED_ShowString(0, 32, "                ",OLED_8X16);
	OLED_Update();
      ensure = PS_GetImage();
      if(ensure == 0x00)
      {
        ensure = PS_GenChar(CharBuffer2); //生成特征
        if(ensure == 0x00)
        {
		  OLED_ShowString(24, 16, "                ",OLED_8X16);
          OLED_ShowChinese(32, 16, "指纹正常");
          OLED_ShowString(0, 32, "                ",OLED_8X16);
			OLED_Update();
          i = 0;
          processnum = 2; //跳到第三步
        }
        else ShowErrMessage(ensure);
      }
      else ShowErrMessage(ensure);
      break;

    case 2:
      OLED_ShowChinese(16, 16, "对比两次指纹");
      OLED_ShowString(0, 32, "                ",OLED_8X16);
	  OLED_Update();
      ensure = PS_Match();
      if(ensure == 0x00)
      {
		OLED_ShowString(0, 16, "                ",OLED_8X16);
        OLED_ShowChinese(32, 16, "对比成功");
        OLED_ShowString(0, 32, "                ",OLED_8X16);
		  OLED_Update();
        processnum = 3; //跳到第四步
      }
      else
      {
        OLED_ShowChinese(32, 16, "对比失败");
        OLED_ShowString(0, 32, "                ",OLED_8X16);
		  OLED_Update();
        ShowErrMessage(ensure);
        i = 0;
        processnum = 0; //跳回第一步
      }
      Delay_ms(500);
      break;

    case 3:
      OLED_ShowChinese(16, 16, "生成指纹模板");
      OLED_ShowString(0, 32, "                ",OLED_8X16);
		OLED_Update();
      Delay_ms(500);
      ensure = PS_RegModel();
      if(ensure == 0x00)
      {
        OLED_ShowChinese(0, 16, "生成指纹模板成功");
        OLED_ShowString(0, 32, "                ",OLED_8X16);
		  OLED_Update();
        processnum = 4; //跳到第五步
      }
      else
      {
        processnum = 0;
        ShowErrMessage(ensure);
      }
      Delay_ms(1000);
      break;

    case 4:
      OLED_ShowChinese(8, 0, "按  加  按  减");
	  OLED_ShowString(24,0,"K4",OLED_8X16);
	  OLED_ShowString(56,0,", ",OLED_8X16);
	  OLED_ShowString(88,0,"K2",OLED_8X16);
	  OLED_ShowString(0, 16, "                ",OLED_8X16);
      OLED_ShowChinese(32, 16, "按  保存");
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
      ensure = PS_StoreChar(CharBuffer2, ID_NUM); //储存模板
      if(ensure == 0x00)
      {
		OLED_Clear();
		OLED_ShowChinese(16, 16, "录入指纹成功");
		OLED_ShowString(0, 32, "                ",OLED_8X16);
		OLED_Update();
		Delay_ms(1500);
		OLED_Clear();
		OLED_ShowChinese(0,0,"指纹模块测试程序");
		OLED_ShowString(16,16,"K1",OLED_8X16);
		OLED_ShowChinese(32,16,"键添加指纹");
		OLED_ShowString(16,32,"K3",OLED_8X16);
		OLED_ShowChinese(32,32,"键删除指纹");
		OLED_ShowString(16,48,"K5",OLED_8X16);
		OLED_ShowChinese(32,48,"键验证指纹");
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
    if(i == 10) //超过5次没有按手指则退出
    {
      break;
    }
  }
}

SysPara AS608Para;//指纹模块AS608参数
//刷指纹
uint8_t press_FR(void)
{
  SearchResult seach;
  uint8_t ensure;
  while(key_num != 1)
  {
    key_num = Keypad_scan();
    ensure = PS_GetImage();
    if(ensure == 0x00) //获取图像成功
    {
      ensure = PS_GenChar(CharBuffer1);
      if(ensure == 0x00) //生成特征成功
      {
        ensure = PS_HighSpeedSearch(CharBuffer1, 0, 99, &seach);
        if(ensure == 0x00) //搜索成功
        {
			OLED_ShowChinese(16, 16, "指纹验证成功");
			OLED_Update();
			OLED_ShowString(8, 32, "ID:       :", OLED_8X16);
			OLED_ShowChinese(56, 32, "得分");
			OLED_ShowNum(32, 32, seach.pageID, 2, OLED_8X16);
			OLED_ShowNum(96, 32, seach.mathscore, 2, OLED_8X16);
			OLED_Update();
			Delay_ms(1500);
			OLED_ShowChinese(48, 48, "开锁");
			Delay_ms(1500);
			OLED_ShowString(0, 48, "                ", OLED_8X16);
			return 1;
        }
        else
        {
          OLED_ShowChinese(32, 16, "验证失败");
          Delay_ms(1500);
			return 0;
        }
      }
      else
			{};
      OLED_Clear();
      OLED_ShowChinese(32, 16, "请按手指");
				OLED_Update();
    }
  }
  OLED_Clear();
  OLED_ShowChinese(32, 0, "指纹开锁");
  OLED_ShowString(16,16,"K1",OLED_8X16);
  OLED_ShowChinese(32,16,"键添加指纹");
  OLED_ShowString(16,32,"K3",OLED_8X16);
  OLED_ShowChinese(32,32,"键删除指纹");
  OLED_ShowString(16,48,"K5",OLED_8X16);
  OLED_ShowChinese(32,48,"键验证指纹");
  OLED_Update();
  return 0;
}

//删除指纹
void Del_FR(void)
{
	uint8_t  ensure;
	uint16_t ID_NUM = 0;
	OLED_ShowChinese(0, 0, "  加   减   确认");
	OLED_ShowString(0, 0, "K4", OLED_8X16);
	OLED_ShowString(32, 0, ",K2", OLED_8X16);
	OLED_ShowChinese(56, 0, "减");					//一定要有，不然前边的减是问号
	OLED_ShowString(72, 0, ",K3", OLED_8X16);
	OLED_ShowChinese(16, 16, "  清空指纹库");
	OLED_ShowString(16, 16, "K5", OLED_8X16);
	OLED_ShowChinese(0, 32, "  返回");
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
      goto MENU ; //返回主页面
    if(key_num == 5)
    {
      key_num = 0;
      ensure = PS_Empty(); //清空指纹库
      if(ensure == 0)
      {
        OLED_Clear();
        OLED_ShowChinese(8, 16, "清空指纹库成功");
		OLED_Update();
      }
      else
        ShowErrMessage(ensure);
      Delay_ms(1500);
      goto MENU ; //返回主页面
    }
    OLED_ShowString(40, 48, "ID=", OLED_8X16);
    OLED_ShowNum(65, 48, ID_NUM, 2, OLED_8X16);
	OLED_Update();
  }
  ensure = PS_DeletChar(ID_NUM, 1); //删除单个指纹
  if(ensure == 0)
  {
	OLED_Clear();
	OLED_ShowChinese(16, 16, "删除指纹成功");
	OLED_Update();
  }
  else
    ShowErrMessage(ensure);
  Delay_ms(1500);
MENU:
  OLED_Clear();
  OLED_ShowChinese(32, 0, "指纹开锁");
  OLED_ShowString(16,16,"K1",OLED_8X16);
  OLED_ShowChinese(32,16,"键添加指纹");
  OLED_ShowString(16,32,"K3",OLED_8X16);
  OLED_ShowChinese(32,32,"键删除指纹");
  OLED_ShowString(16,48,"K5",OLED_8X16);
  OLED_ShowChinese(32,48,"键验证指纹");
  OLED_Update();
  key_num = 0;
}
