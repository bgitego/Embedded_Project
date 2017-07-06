#include <stdint.h>
#include "TM4C123GH6PM.h"
#include "delay.h"
#include "i2c.h"
#include "UART.h"
#include "grideye.h"

#define I2CSTOP          (1U << 0)
#define I2CSTART         (1U << 1)
#define I2CRUN           (1U << 2)

#define LED_RED          (1U << 1)
#define LED_BLUE         (1U << 2)
#define LED_GREEN        (1U << 3)

#define ADXL345SLVADDR   0x53
#define ADXL345DEVID     0x00
#define ADXL345PWRCTL    0x2D
#define ADXL345MMODE     (1U << 3)
#define ADXL345XAXIS    0x32
#define ADXL345YAXIS    0x34
#define ADXL345ZAXIS    0x36

#define GRIDEYESLVADDR   0x68
#define GRIDEYETRMSTORL     0x0E
#define GRIDEYETRMSTORH     0x0F
#define GRIDEYEPWRCTL    0x2D
#define GRIDEYEMMODE     (1U << 3)
#define GRIDEYEPIXADDR 0x80
#define GRIDEYENBYTE    128

int main() {

 // This objrvy eil hold the Grideye Object
 // volatile grideye_type p_grideye;
    volatile pixel_type p_segment;
     uint8_t grideye_buffer[128];
     int16_t grideye_pixel[64];

  //UART Module Initialisation
  UART_Init();
   delay(500000);

//I2C Module Initialization
  Init_I2C0();
  delay(500000);

  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

   Read_N_Byte_I2C0(GRIDEYESLVADDR,GRIDEYEPIXADDR,GRIDEYENBYTE,&grideye_buffer[0] );

   //Initialiding buffer to all 0;
  for(uint8_t i =0;i<128;i++){
    UART_OutUDec(grideye_buffer[i]);UART_OutChar(CRJ);
  }

  //Converting Pixel Data from 12bit two 32bit

   grideye_convert(&grideye_buffer[0] ,&grideye_pixel[0] , 128 );

  //Moving Capture data to designated GridEye Register
 int t;
  for( uint8_t i=0; i < 8; i++){
    for(uint8_t j=0; j< 8; j++){
      t = ((8*i)+j);
      // UART_OutUDec(t);UART_OutChar(SP);
       p_segment.pixel[i][j]= grideye_pixel[t];
    }
    UART_OutChar(CRJ);
  }

//  p_grideye.
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  //Moving Capture data to designated GridEye Registers

  for( int i=0; i < 8; i++){
    for(int j=0; j< 8; j++){
     // p_segment.pixel[i][j]= (j);
      UART_OutDec(p_segment.pixel[i][j]);UART_OutChar(SP);
    }
    UART_OutChar(CRJ);
  }

//  p_grideye.
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



  for(int i=0; i<128;i++){

   // UART_OutString("Register Value : ");UART_OutUDec(i);UART_OutString(" : ");
    //UART_OutUDec(grideye_buffer[i]);UART_OutChar(CRJ);
  }
  delay(5000);
  uint8_t DevID = 0;// ReceiveI2C0(GRIDEYESLVADDR,GRIDEYETRMSTORL);
  //DevID |= (ReceiveI2C0(GRIDEYESLVADDR,GRIDEYETRMSTORH) << 8);
 // UART_OutString("I2C Slave Device ID : ");
  //UART_OutUDec(DevID);UART_OutChar(CRJ);

  SYSCTL->RCGC2 |= (1U << 5);  /* enable clock for GPIOF */
  SYSCTL->GPIOHBCTL |= (1U << 5); /* enable AHB for GPIOF */
  GPIOF_AHB->DIR |= (LED_RED | LED_BLUE | LED_GREEN);
  GPIOF_AHB->DEN |= (LED_RED | LED_BLUE | LED_GREEN);
  //GPIOF_AHB->DATA_Bits[LED_BLUE] = LED_BLUE;

  if( DevID == 0x08){
       GPIOF_AHB-> DATA |= LED_GREEN;
  }


  while (1) {

    // DevID = ReceiveI2C0(GRIDEYESLVADDR,GRIDEYETRMSTORL);
     //DevID |= (ReceiveI2C0(GRIDEYESLVADDR,GRIDEYETRMSTORH) << 8);

      // DevID = ReceiveI2C0(ADXL345SLVADDR,ADXL345PWRCTL);
       // UART_OutString("I2C Slave Device ID : ");
        //UART_OutUDec(DevID);UART_OutChar(CRJ);

      GPIOF_AHB-> DATA |= LED_RED;
      delay(50000);

      GPIOF_AHB-> DATA &= ~LED_RED;

      delay(50000);
  }
  //return 0;
}
