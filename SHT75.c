#include "include.h"
#include "SHT75.h"



void SHT_DelayUs(unsigned int deltime){
  if(deltime<16){
    int ticksperus=SHT_FCPU;
  
    if(deltime>(36535/SHT_FCPU)) deltime=(36535/SHT_FCPU);
  
    asm("ldi r16,0\t\n"
        "in r17,__SREG__\t\n"
        "cli\t\n" ::);

    OCR1A=deltime*ticksperus;
    TCCR1B=(1<<WGM12);
    while(TIFR&(1<<OCF1A));
    TIFR=(1<<OCF1A);

    asm("out __SREG__,r17\t\n"::);
  }
}

void SHT_Start(){
  //Begin transmission:
  SDAH; SCKL; SHT_DelayUs(2*SHT_DEL);
  SCKH; SHT_DelayUs(2*SHT_DEL);
  SDAL; SHT_DelayUs(SHT_DEL);
  SCKL; SHT_DelayUs(2*SHT_DEL);
  SCKH; SHT_DelayUs(SHT_DEL);
  SDAH; SHT_DelayUs(SHT_DEL);
  SCKL;
  SHT_DelayUs(SHT_DEL*5);
}

uint8_t SHT_Cmd(SHT_Command x){
  for(int i=0x80;i>0;i/=2){
    if(i&x){
      SDAH;
    }else{
      SDAL;
    }
    
    SCKH;
    SHT_DelayUs(2*SHT_DEL);
    SCKL;
  }

  uint8_t ex=0xff;

  SCKH;
  SDAIN;
  SHT_DelayUs(SHT_DEL);
  ex=bit_is_set(SHT_PIN,SHT_DATA);
  SCKL;
  return ex;
}

