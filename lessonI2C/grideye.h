//****************************************************************************
//This grideye.h file has been created to define the data type used for
//The Panasonic Grideye Themral Sensor.
// Creator: Bahati D Gitego 06/28/2017
//****************************************************************************

//****************************************************************************

#ifndef __GRIDEYE_H__
#define __GRIDEYE_H__

#include "stdint.h"
void grideye_convert(uint8_t * ,uint32_t * , uint32_t );
void GridEyeMove2Segment(uint32_t * ,uint32_t * , uint32_t );

typedef struct
{
	int32_t pixel[8][8];

} pixel_type;

typedef struct
{
     uint8_t  PCTL;                 // Register for setting operating mode of device.{0x00=Normal,0x10=Sleep,0x20=Stanby_60Sec,0x21=Stanby_10Sec}
     uint8_t  RST;                  // Software Reset
     uint8_t  FPSC;                 // Frame rate {0x00=10/s, 0x01=1/s}
     uint8_t  AVE;                  // Moving Average Output Mode {Set Bit5 to 1 For Moving Average Mode}
     uint32_t THT;                  // Thermistor Output Value {Max Range 12bit, with 1 Bit resolution of 0.0625°C}
     pixel_type Segment[10][10];        // Hold segment of 8x8 set of pixel Segment can hold up to 80x80 pixel data.

} grideye_type;


#endif  //__GRIDEYE_H__