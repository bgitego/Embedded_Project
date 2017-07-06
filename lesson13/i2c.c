#include "tm4c_cmsis.h"
#include "i2c.h"

#define I2C0CLCKEN      (1U << 12)
#define I2C0SCLPIN      (1U << 2)
#define I2C0SDAPIN      (1U << 3)
#define I2C0MSTREN      (1u << 5)

#define I2C0SCLPCTL     (3U << 8)
#define I2C0SDAPCTL     (3U << 12)

#define I2C0SCLAFSEL    (1U << 2)
#define I2C0SDAAFSEL    (1U << 3)
#define I2C0SDAODR      (1U << 3)
#define I2C0INIMCR      (1U << 4)
#define I2C0MTPR        (1U << 0)

#define I2CSTOP         (1U << 0)
#define I2CSTART        (1U << 1)
#define I2CRUN          (1U << 2)


#define I2C0MTRW        (1U <<0)

//I2CMCS Bit fields
#define MCSBUSY         (1U << 0)
#define MCSERROR        (1U << 1)
#define MCSADRACK       (1U << 2)
#define MCSDATACK       (1U << 3)
#define MCSARBLST       (1U << 4)
#define MCSBUSBSY       (1U << 6)

void InitI2C0(void){
  
    SYSCTL -> RCGC1 |= I2C0CLCKEN;                              /* 1.  Enable clock for I2C Module0 */
    SYSCTL -> RCGC2 |= (1U << 1);                               /* 2.  Enable clock for GPIOB*/
    SYSCTL -> GPIOHSCTL |= (1U << 1);                           /* 2a. Enable High Speed for GPIOB */
    GPIOB_HS -> DIR |= (I2C0SCLPIN | I2C0SDAAFSEL);             /* 2a. Configured I2C0SCL,I2C0SDA for Output */
    GPIOB_HS -> DEN |= (I2C0SCLPIN | I2C0SDAAFSEL);             /* 2b. Enable Digital Mode for I2C0SCL,I2C0SDA */ 
    GPIOB_HS -> AFSEL |= (I2C0SCLAFSEL |I2C0SDAAFSEL);          /* 3.. Enable Alternate Functionality for Pins PB2,PB3 As Peripheral*/
    GPIOB_HS -> ODR |= I2C0SDAODR;                              /* 4. Set I2C0SDA(PB3) As Open Drain */
    GPIOB_HS -> PCTL |= (I2C0SCLPCTL | I2C0SDAPCTL);            /* 5. Enable Alternate Functionality for Pins PB2,PB3 Port Control */     
    I2C0_MASTER ->MCR |= I2C0INIMCR;                            /* 6. Initialise I2C0Maste*/    
    I2C0_MASTER ->MTPR |=  I2C0MTPR;                            /* 7. Using System Clock of 16Mhz set the SCL Rate to 400Kbps* TPR = (16000000Hz/(2*(6+4)*400000))-1;*/
}
void SendI2C0(uint8_t WRITE_DATA, uint8_t CONDITION ){

    I2C0_MASTER -> MDR  = WRITE_DATA;
    I2C0_MASTER ->MCS |= (CONDITION);
    
    while ( ((I2C0_MASTER ->MCS) & MCSBUSY) !=0 ){
      ;
    } 
	I2CErrorRoutine();
}

uint8_t ReceiveI2C0(uint8_t WRITE_DATA, uint8_t CONDITION ){
    
	I2C0_MASTER -> MDR  = WRITE_DATA;
    I2C0_MASTER -> MCS |= (CONDITION);


    while ( ((I2C0_MASTER -> MCS) & MCSBUSY) !=0 ){
      ;
    } 
	if (((I2C0_MASTER -> MCS) & MCSERROR) !=0 ){       
		I2CErrorRoutine();
		return 0;
	}
	else{
		return I2C0_MASTER -> MDR;
	}	
}

void SetSlaveAddres(uint8_t SLAVE_ADDRESS){                          //Function to Set Slave Address
    I2C0_MASTER ->MSA |=(SLAVE_ADDRESS<<1);
}

void SetReadWrite(uint8_t I2CREAD){                                  // Function to Set Read the Read Bit to 1 for I2C Slave Read, and 0 for I2c Slave Write
  if(I2CREAD !=0){
    I2C0_MASTER ->MSA |=(1U<<0);
  }
  else{
    I2C0_MASTER ->MSA &= ~(1U<<0);	
  }
}

void I2CErrorRoutine(){

	  //Error with With I2C Send Command.
  
  if (((I2C0_MASTER ->MCS) & MCSADRACK) !=0 ){              //Slave Address not Acknowledge Error
	;
  }
  else if(((I2C0_MASTER ->MCS) & MCSDATACK)!=0 ){           // Data not Acknowledge Error
	; 
  }
  else if(((I2C0_MASTER ->MCS) & MCSARBLST)!=0 ){           // Data not Acknowledge Error
	; // Do something for Multi Master Mode
  }
  I2C0_MASTER ->MCS |= (I2CSTOP);                           // When Error is detected, Stop Transation
}