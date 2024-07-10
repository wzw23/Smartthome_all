#ifndef __DHT11_H
#define __DHT11_H

void DHT11_Start(void);
uint8_t DHT11_Receive_Bit(void);
uint8_t DHT11_Receive_Byte(void);
uint8_t DHT11_Receive_Data(uint8_t *TH, uint8_t *TL, uint8_t *HH, uint8_t *HL);

#endif

