#include "grideye.h"

//Function For Coneverting Low Byte and High Byte to 32 bit signed Word.
void grideye_convert(uint8_t *raw_data ,int16_t *dst_addr ,int16_t Num_of_byte ){
 int32_t dst_offeset=0;
 int32_t low_byte,high_byte,converted_word;//,address_of;

 for( uint8_t i=0; i < Num_of_byte;i=i+2){

      //Convert 12 bit two complement to 32 bit two complement
      low_byte       = (int16_t) (*(raw_data + i));
      high_byte      = (int16_t) ((*(raw_data+i+1)) << 8);
      converted_word = (int16_t) ((low_byte|high_byte)<<4)/(1<<4);
      *(dst_addr+dst_offeset) =  converted_word;
   //   address_of =  (int32_t) (raw_data + i);
      dst_offeset++;

 }
}

//Function for moving process word into the grideye object.
void GridEyeMove2Segment(int16_t *a ,int16_t *b , int16_t c){
;
}
