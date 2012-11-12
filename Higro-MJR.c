#include "include.h"
#include "Sht.h"

#define t 0x78
#define e 0x79
#define s 0x6D
#define b 0x7F
#define l 0x38
#define a 0x77
#define d 0x3f

uint8_t mSHT[4]={0,0,0,0x39}; 
uint8_t mSHT2[4]={t,e,s,t};
uint8_t mSHT3[4]={b,l,a,d};
uint16_t cPomiar=0;
void vConversion(uint16_t *aftemp, uint16_t *afhumi)
{
	mSHT[3]^=0x4A;
	
	cPomiar=(~mSHT[3]>>1&1)*(*aftemp)+(mSHT[3]>>1&1)*(*afhumi);
	// For testing	cPomiar=(~mSHT[0]>>1&1)*255+(mSHT[0]>>1&1)*123;
	mSHT[0]=cPomiar/100;
	mSHT[1]=(cPomiar%100)/10;
	mSHT[2]=cPomiar%10;
	
	int i;
	for(i=0;i<3;i++){

		switch(mSHT[i])
		{
			case 0:
			mSHT[i]=0x3F;
			break;
			case 1:
			mSHT[i]=0x06;
			break;
			case 2:
			mSHT[i]=0x5B;
			break;
			case 3:
			mSHT[i]=0x4F;
			break;
			case 4:
			mSHT[i]=0x66;
			break;
			case 5:
			mSHT[i]=0x6D;
			break;
			case 6:
			mSHT[i]=0x7D;
			break;
			case 7:
			mSHT[i]=0x07;
			break;
			case 8:
			mSHT[i]=0x7F;
			break;
			case 9:
			mSHT[i]=0x6F;
			break;
			//C:
			//P:0x73

			default:
			mSHT[i]=0x01;
			break;
		}
	}
}

void vDisplay(uint8_t cycles,uint8_t mSHT[4])
{
	int i;
	for(i=0;i<cycles;i++)
	{
		PORTD=0x01;
		PORTA=mSHT[0];
		_delay_ms(3);
		PORTD=0x02;
		PORTA=mSHT[1]|0x80;
		_delay_ms(3);
		PORTD=0x04;
		PORTA=mSHT[2]&~0x80;
		_delay_ms(3);
		PORTD=0x08;
		PORTA=0x4F;
		PORTA=mSHT[3];
		_delay_ms(3);
		
	}
}

uint8_t checksum;

int main(){
	
	unsigned int rawmeastemp=0,rawmeashumi=0;
	float measrestemp=0.0, measreshumi=0.0;

	_delay_ms(11);
	mSHT[3]=0x39;
	vDisplay(4000,mSHT2);
	vDisplay(4000,mSHT2);
	vDisplay(4000,mSHT2);
	PORTA=0x00;
	ShtInit();
	
	while(1){
		rawmeastemp=(0x3fff)&ShtMeasure(SHT_TEMPERATURE); //Use AND to mask bits for 12bit/14bit resolution
		rawmeashumi=(0x0fff)&ShtMeasure(SHT_HUMIDITY);
		ShtCalculate(&rawmeastemp,&rawmeashumi,&measrestemp,&measreshumi);
		vConversion(&measrestemp,&measreshumi);
		if (rawmeastemp==0x3FFF || rawmeashumi==0x0FFF){
		
		vDisplay(4000,mSHT3);
		vDisplay(4000,mSHT3);
		}		
		else
		{
		vDisplay(4000,mSHT);
		vDisplay(4000,mSHT);	
		}	
		PORTA=0x00;
		ShtReset();
	}

	return 0;
}