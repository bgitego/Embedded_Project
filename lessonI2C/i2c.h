#ifndef __I2C_H__
#define __I2C_H__

#include "stdint.h"

void InitI2C0(void);
void SendI2C0(uint8_t,uint8_t,uint8_t );
uint8_t ReceiveI2C0(uint8_t,uint8_t);
void SetReadWrite(uint8_t);
void SetSlaveAddres(uint8_t);
uint8_t I2CErrorRoutine(void);
void I2CWait(void);
void ReadNByteI2C0(uint8_t,uint8_t,uint8_t,uint8_t * );
#endif // __DELAY_H__11