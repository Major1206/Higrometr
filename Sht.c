#include "Sht.h"
#include "include.h"

// MJR - Replaced all DELAY(SHT_DELAY)  with functions  built in <util/delay.h> (_delay_us(SHT_DELAY)

inline void datdirout(void) { SET(SHT_DDR, SHT_DATA); }	  // Drive DATA pin
inline void datdirin(void)	{ USET(SHT_DDR, SHT_DATA);}   // DATA pin
inline void clkdirout(void)	{ SET(SHT_DDR, SHT_CLOCK);}   // DATA pin
inline void dath(void)		  { SET(SHT_PORT, SHT_DATA);}	  // DATA pin high
inline void datl(void)  	  { USET(SHT_PORT, SHT_DATA); } // DATA pin low
inline void clkh(void)	    { SET(SHT_PORT, SHT_CLOCK); }	// CLOCK pin high
inline void clkl(void)		  { USET(SHT_PORT, SHT_CLOCK); }// CLOCK pin low

inline void transmith(void){
  datdirout(); clkdirout();
  clkl(); datl();
  _delay_us(SHT_DELAY/2);
  dath(); _delay_us(SHT_DELAY/2); clkh();
  _delay_us(SHT_DELAY);
  clkl(); datl();
  _delay_us(SHT_DELAY);
}

inline void transmitl(void){
  datdirout(); clkdirout();
  clkl(); datl();
  _delay_us(SHT_DELAY);
  clkh(); datl();
  _delay_us(SHT_DELAY);
  clkl(); datl();  
  _delay_us(SHT_DELAY);
}

inline int inputin(void){
  int test=0;

  clkdirout();
  datdirin(); //no pullup, is in the circuit
  clkl();
  _delay_us(SHT_DELAY);
  clkh();
  _delay_us(SHT_DELAY/2);
  test=bit_is_set(SHT_PIN,SHT_DATA);
  _delay_us(SHT_DELAY/2);
  clkl();
  _delay_us(SHT_DELAY);

  return test;
}

void ShtInit(void)
{
  clkdirout();
  datdirout();
  dath();
  clkl();
}

void transstart(void)
{
  datdirout();
  //Assert : ShtInit has been called
  clkl();
  dath();
  _delay_us(SHT_DELAY);  _delay_us(SHT_DELAY);

  clkh();
  dath();
  _delay_us(SHT_DELAY);  _delay_us(SHT_DELAY);

  clkh();
  datl();
  _delay_us(SHT_DELAY);  _delay_us(SHT_DELAY);

  clkl();
  datl();
  _delay_us(SHT_DELAY);  _delay_us(SHT_DELAY);

  clkh();
  datl();
  _delay_us(SHT_DELAY);  _delay_us(SHT_DELAY);

  clkh();
  dath();
  _delay_us(SHT_DELAY);  _delay_us(SHT_DELAY);

  clkl();
  dath();
  _delay_us(SHT_DELAY);  _delay_us(SHT_DELAY);

}

void ShtReset(void)
{
  for(int ax=0x80; ax>0; ax/=2){
    transmith();
  }
  transmith();
  transstart();
}

char read_byte(int ack)
{
  uint8_t i,val=0;
  datdirin();
  for(i=0x80;i>0;i/=2){    
    clkh();
    _delay_us(SHT_DELAY/2);
    if(bit_is_set(SHT_PIN,SHT_DATA)) val|=i;
    _delay_us(SHT_DELAY/2);
    clkl();
    _delay_us(SHT_DELAY);
  }
  if(ack==SKIPACK){
    datdirout();
    clkh();
    _delay_us(SHT_DELAY);
    clkl();
    _delay_us(SHT_DELAY);
  }else{
    datdirout();datl();
    datl();clkh();
    _delay_us(SHT_DELAY);
    clkl();
    _delay_us(SHT_DELAY);
  }

  return val;
}

char write_byte(unsigned char value)
{
  int test=0;
  for(int x=0x80;x>0;x/=2){
    if(value&x){
      transmith();
    }else{
      transmitl();
    }
  }

  datdirin(); dath();  //idea: enable pullup
  _delay_us(SHT_DELAY);
  clkh(); 
  _delay_us(SHT_DELAY/2);
  test=bit_is_set(SHT_PIN,SHT_DATA);
  _delay_us(SHT_DELAY/2);
  clkl();

  return test&(1<<SHT_DATA);
}

//----------------------------------------------------------------------------------
// Read humidity or temperature from the sensor.
// Returns the value in ticks. Use sht_calc() to convert to real world units.
// Returns 0xFFFF if the measurment failed
//----------------------------------------------------------------------------------
int ShtMeasure(unsigned char mode)
{
	unsigned int 	temp = 0x0000;
	extern uint8_t	checksum;
	unsigned char 	c=0;

	// Signal start of communications
	transstart();

	// Request measurement
	c=write_byte(mode);

  // Sensor lowers the data line when measurement
  // is complete. Wait up to 2 seconds for this.
  for (c=0; c<40; c++)
    {
      if (! bit_is_set(SHT_PIN, SHT_DATA)){
        break;
      }
      _delay_us(1000000/10);
    }

  // Read the measurement
  if (!bit_is_set(SHT_PIN, SHT_DATA) && (c!=(1<<SHT_DATA))){
    temp = read_byte(NOSKIPACK);
    temp = temp << 8;
    temp += read_byte(NOSKIPACK);
    checksum+=read_byte(SKIPACK);
  }else{
    temp=0xffff;
  }
	
  return temp;
}

//----------------------------------------------------------------------------------------
// Calculates tempurature in ^C and humidity in %RH (temperature compensated)
// sht_measure() returns temp and humidity in ticks. Use this function to convert
// to compensated values in real world units.
//
// This function returns integers with 2 assumed decimal places. For example 2550
// means 25.50. This is to avoid including the floating point math library.
//
// input :	humi [Ticks] (12 bit)
// 		temp [Ticks] (14 bit)
// output: 	humi [%RH] (2 fixed decimals)
// 		temp [^C]  (2 fixed decimals)
//----------------------------------------------------------------------------------------
void ShtCalculate(int *p_temperature, int *p_humidity, uint16_t *aftemp, uint16_t *afhumi)
{
  //float humid=-2.0468+0.0367*((float)(*p_humidity))-0.000001595*((float)(*p_humidity)*(float)(*p_humidity));
  //float temp = 0.01*(float)(*p_temperature)-40.1;
 // float humi = (temp-25.0f)*(0.01f+0.00008f*((float)(*p_humidity)))+humid;
int temp=round(-401+0.1*(float)(*p_temperature));
int humi=round((+0.1*(float)(*p_temperature)-651)*(0.01+0.00008*((float)(*p_humidity)))-20.468+0.367*((float)(*p_humidity))-1.5955e-5*((float)(*p_humidity))*((float)(*p_humidity)));

  if(humi>999) humi=999;
  if(humi<0.1) humi=0.100;

  *aftemp=temp;
  *afhumi=humi;
}

//----------------------------------------------------------------------------------
// Reads the status register with checksum (8-bit)
//----------------------------------------------------------------------------------
char ShtReadStatus(unsigned char *p_value)
{
	unsigned char error=0;
	unsigned char checksum=0;

	transstart(); 					//transmission start
	error = write_byte(STATUS_REG_R); 	//send command to sensor
	*p_value = read_byte(NOSKIPACK); 			//read status register (8-bit)
	checksum = read_byte(NOSKIPACK); 			//read checksum (8-bit)
	return error; 						//error=1 in case of no response form the sensor
}


//----------------------------------------------------------------------------------
// Writes the status register . Note this library only supports the default
// 14 bit temp and 12 bit humidity readings.
//----------------------------------------------------------------------------------
char ShtWriteStatus(unsigned char value)
{
	unsigned char error=0;
	transstart(); 					//transmission start
	error += write_byte(STATUS_REG_W);	//send command to sensor
	error += write_byte(value); 		//send value of status register
	return error; 						//error>=1 in case of no response form the sensor
}
