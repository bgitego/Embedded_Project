#include "TM4C123GH6PM.h"
#include "i2c.h"
#include "UART.h"

#define I2C0CLCKEN      (1U << 0)
#define ENGPIOB	        (1U << 1)
#define I2C0SCLPIN      (1U << 2)
#define I2C0SDAPIN      (1U << 3)
#define I2C0MSTREN      (1u << 5)

#define I2C0SCLPCTL     (3U << 8)
#define I2C0SDAPCTL     (3U << 12)

#define I2C0SCLAFSEL    (1U << 2)
#define I2C0SDAAFSEL    (1U << 3)
#define I2C0SDAODR      (1U << 3)
#define I2C0INIMCR      (1U << 4)
#define I2C0MTPR        (1U << 0) //400KHZ
//#define I2C0MTPR        ((1U << 0)|(1U << 1)|(1U << 2))  //100Khz

#define I2CRUN          (1U << 0)
#define I2CSTART        (1U << 1)
#define I2CSTOP         (1U << 2)
#define I2CACK          (1U << 3)


#define I2C0MTRW        (1U << 0)

//I2CMCS Bit fields
#define MCSBUSY         (1U << 0)
#define MCSERROR        (1U << 1)
#define MCSADRACK       (1U << 2)
#define MCSDATACK       (1U << 3)
#define MCSARBLST       (1U << 4)
#define MCSBUSBSY       (1U << 6)

void Init_I2C0(void){

    UART_OutString("Initialising I2C: ");UART_OutChar(CRJ);

    SYSCTL -> RCGCI2C |= I2C0CLCKEN;                              /* 1.  Enable clock for I2C Module0 */
    SYSCTL -> RCGCGPIO |= ENGPIOB;                               	/* 2.  Enable clock for GPIOB*/
    SYSCTL -> GPIOHBCTL |= (1U << 1);                         	/* 2a. Enable High Speed for GPIOB */
    GPIOB_AHB -> DEN   |= (I2C0SCLPIN   | I2C0SDAPIN);          /* 2b. Enable Digital Mode for I2C0SCL,I2C0SDA */
    GPIOB_AHB -> AFSEL |= (I2C0SCLAFSEL | I2C0SDAAFSEL);        /* 3.. Enable Alternate Functionality for Pins PB2,PB3 As Peripheral*/
    GPIOB_AHB -> AMSEL &= ~(I2C0SCLAFSEL | I2C0SDAAFSEL);        /* 3.. Disable Analog Functionality for Pins PB2,PB3 As Peripheral*/
    GPIOB_AHB -> ODR |= I2C0SDAODR;                             /* 4. Set I2C0SDA(PB3) As Open Drain */
    GPIOB_AHB -> PCTL |= (I2C0SCLPCTL | I2C0SDAPCTL);          	/* 5. Enable Alternate Functionality for Pins PB2,PB3 Port Control */
    I2C0 -> MCR  |=  I2C0INIMCR;                                /* 6. Initialise I2C0Master*/
    I2C0 -> MTPR =  I2C0MTPR;                                  /* 7. Using System Clock of 16Mhz set the SCL Rate to 400Kbps* TPR = (16000000Hz/(2*(6+4)*400000))-1;*/

   // GPIOB_AHB -> DIR   |= (I2C0SCLPIN   | I2C0SDAPIN);           /* 2a. Configured I2C0SCL,I2C0SDA for Output */


}



void Send_I2C0(uint8_t SLAVE_ADDR, uint8_t SLAVE_REG, uint8_t WRITE_DATA ){

  //Troubleshooting Code
  UART_OutString("Iniside Writing I2C Functtion: ");UART_OutChar(CRJ);
  UART_OutString("Data Being Written Follow By Condition: ");
  UART_OutUDec(WRITE_DATA);UART_OutChar(CRJ);

  Set_SlaveAddres(SLAVE_ADDR);
  Set_ReadWrite(0);                           //Set write Bit in the MSA Register
  I2C0 -> MDR  = SLAVE_REG;
  I2C0 -> MCS |= (0|I2CSTART|I2CRUN);        //Set Start Condition
  Wait_I2C0();                                 // Check if Transmission is Done
  ErrorRoutine_I2C0();
  I2C0 -> MDR  = WRITE_DATA;
  I2C0 -> MCS |= (0|I2CSTOP|I2CRUN);         //Set Run and Stop Condition
  Wait_I2C0();
  ErrorRoutine_I2C0();
}

uint8_t Receive_I2C0(uint8_t SLAVE_ADDR, uint8_t SLAVE_REG ){

  //Troubleshooting Code
  //UART_OutString("Iniside Reading I2C Functtion: ");UART_OutChar(CRJ);
  //UART_OutString("Data Being Written Follow By Condition: ");

  Set_SlaveAddres(SLAVE_ADDR);
  Set_ReadWrite(0);                            //Set write Bit in the MSA Register
  I2C0 -> MDR  = SLAVE_REG;
  I2C0 -> MCS = (I2CACK|I2CSTART|I2CRUN);     //Set Start Condition
  Wait_I2C0();
  ErrorRoutine_I2C0();

  Set_ReadWrite(1);                            //Set Read Bit in the MSA Register
  I2C0 -> MCS =  (I2CSTOP|I2CRUN);	          //Stop and Run Condition

  Wait_I2C0();
  ErrorRoutine_I2C0();
  return I2C0 -> MDR;

}


void Read_N_Byte_I2C0(uint8_t SLAVE_ADDR, uint8_t SLAVE_REG,uint8_t N_Byte, uint8_t *STORE_ADDR ){

	//(1) Write Slave Address
	Set_SlaveAddres(SLAVE_ADDR);
	//(2) Set write Bit in the MSA Register
	Set_ReadWrite(0);
	//(3) Write Slave Register
	I2C0 -> MDR  = (SLAVE_REG);
	//(4) Set Start,Run,Ack Condition
	I2C0 -> MCS = (I2CSTART|I2CRUN);
	//Call the wait for bus to go idle
	Wait_I2C0();
	//Call the check for error function
	ErrorRoutine_I2C0();
	//Set the Read Bits, on the Slave Address
	Set_ReadWrite(1);
	//Initiate a repeat condition to start reading slave registers
	I2C0 -> MCS =  (I2CACK|I2CRUN|I2CSTART);

	//Run Loop to get next data from slave untilN_Byte-1 is read, then send a NACK and Stop condition to read last byte.
	for(uint8_t i=1;i<=N_Byte;i++){
		//Set Read Bit in the MSA Register
		Wait_I2C0();
		// Breakout of Forloop whenever An I2C Error is Detected
		if( ErrorRoutine_I2C0() !=0){
		   UART_OutString("Exiting For Loop Due to Error At Index : ");UART_OutUDec(i);UART_OutChar(CRJ);
		   break;
		}
		//(I-1) Ensures that the first element of buffer array is populated(buff(0))
		*(STORE_ADDR + (i-1)) = I2C0 -> MDR;

		if( i < (N_Byte-1)){
		  //Acknowledge Current byte
		  I2C0 -> MCS =  (I2CACK|I2CRUN);
		}else if( i== (N_Byte-1)){
		  //Don't Acknowledge and Send a stop on Last Byte
		  I2C0 -> MCS =  (I2CSTOP|I2CRUN);
		  UART_OutString("Receiving Last Byte Of Data At Index: ");UART_OutUDec(i);UART_OutChar(CRJ);
		}
		//Troubleshooting Code
		//UART_OutString("Register Value : ");UART_OutUDec(i);UART_OutString(" : ");UART_OutChar(CRJ);
	  }
}


void Set_SlaveAddres(uint8_t SLAVE_ADDRESS){                          //Function to Set Slave Address
    I2C0 ->MSA |=(SLAVE_ADDRESS<<1);
    UART_OutString("Slave Address Is Set To: ");
    UART_OutUHex(I2C0 ->MSA);UART_OutChar(CRJ);
}

void Set_ReadWrite(uint8_t I2CREAD){                                  // Function to Set Read the Read Bit to 1 for I2C Slave Read, and 0 for I2c Slave Write
  if(I2CREAD !=0){
    I2C0 ->MSA |=(1U<<0);
  }
  else{
    I2C0 ->MSA &= ~(1U<<0);
  }
}

uint8_t ErrorRoutine_I2C0(){
       //Troubleshooting Code
     //  UART_OutString("Inside Error Routine ");UART_OutChar(CRJ);

          //Error with With I2C Send Command.

      if (((I2C0 ->MCS) & MCSADRACK) !=0 ){                     //Slave Address not Acknowledge Error
           I2C0 ->MCS |= (I2CSTOP);                             // When Error is detected, Stop Transation
           UART_OutString("I2C Transaction Stopped ");UART_OutChar(CRJ);
           UART_OutString("I2C Error Slave Address Not Acknowledged ");UART_OutChar(CRJ);
           return 1;
      }
      else if(((I2C0 ->MCS) & MCSDATACK)!=0 ){           // Data not Acknowledge Error
          UART_OutString("I2C Error Data Not Acknowledged ");UART_OutChar(CRJ);
           I2C0 ->MCS |= (I2CSTOP);                           // When Error is detected, Stop Transation
           UART_OutString("I2C Transaction Stopped ");UART_OutChar(CRJ);
           return 1;
      }
      else if(((I2C0 ->MCS) & MCSARBLST)!=0 ){
           UART_OutString("I2C Error Arbiration Lost ");UART_OutChar(CRJ);
           I2C0 ->MCS |= (I2CSTOP);                           // When Error is detected, Stop Transation
           UART_OutString("I2C Transaction Stopped ");UART_OutChar(CRJ);
           return 1;
      } else{
          return 0;
      }
}

//Function Returns 0 if there's an error Condition. Else 1
void Wait_I2C0(){

  //UART_OutString("Running the XferDone Function: ");UART_OutChar(CRJ);

  while ( ((I2C0 ->MCS) & MCSBUSY) !=0 ){
    ;
    //Troubleshooting Code
    //UART_OutString("Bus Is Busy MCS Value: ");
    //UART_OutUDec(I2C0 -> MCS);UART_OutChar(CRJ);
  }

}
