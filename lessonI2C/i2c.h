#ifndef __I2C_H__
#define __I2C_H__

#include "stdint.h"

void Init_I2C0(void);
void Send_I2C0(uint8_t,uint8_t,uint8_t );
uint8_t Receive_I2C0(uint8_t,uint8_t);
void Set_ReadWrite(uint8_t);
void Set_SlaveAddres(uint8_t);
uint8_t ErrorRoutine_I2C0(void);
void Wait_I2C0(void);
void Read_N_Byte_I2C0(uint8_t,uint8_t,uint8_t,uint8_t * );
#endif // __DELAY_H__11