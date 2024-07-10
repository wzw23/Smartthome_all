#ifndef __RC522_FUNCTION_H
#define __RC522_FUNCTION_H

#include "stm32f4xx.h"                  // Device header

#define          macDummy_Data              0x00

unsigned char ReadRawRC(unsigned char Address);
void WriteRawRC(unsigned char Address, unsigned char value);
char PcdReset(void);
void SetBitMask(unsigned char reg,unsigned char mask);
void ClearBitMask(unsigned char reg,unsigned char mask);
char PcdComMF522(unsigned char Command, 
                 unsigned char *pInData, 
                 unsigned char InLenByte,
                 unsigned char *pOutData, 
                 unsigned int  *pOutLenBit);
void PcdAntennaOn(void);
void PcdAntennaOff(void);
char PcdRequest(unsigned char req_code,unsigned char *pTagType);
char PcdAnticoll(unsigned char *pSnr);

#endif

