#include "include.h"
//----------------------------------------------------------------------------------
//
// Sensirion SHT1x Humidity Sensor Library
//
//----------------------------------------------------------------------------------

#ifndef __sht_h
#define __sht_h

#define SHT_TEMPERATURE 	0x03		// Measure temp - for ShtMeasure
#define SHT_HUMIDITY 		0x05		// Measure humidity - for ShtMeasure

typedef int bool;

#define SHT_DDR				DDRC		// Port with clock and data pins
#define SHT_PORT			PORTC		// Port with clock and data pins
#define SHT_PIN				PINC		// Port with clock and data pins
#define SHT_CLOCK			PC0		// Pin used to output clock to SHT
#define SHT_DATA			PC1		// Pin used to read/output data from/to SHT
#define SHT_DELAY			25		// uS delay between clock rise/fall

#define STATUS_REG_W 		0x06 		// Command to read status register
#define STATUS_REG_R 		0x07 		// Command to write status register
#define RESET 				0x1e 		// Command for soft reset (not currently used)

#define XTAL 				8000000		// Processor clock

#define SKIPACK 1
#define NOSKIPACK 0

#define DELAY(us)			(Delay(((us*(XTAL/1000000))-16)/6)) //_delay_us(us)

void Delay(unsigned long us);
void ShtInit(void);
void ShtReset(void);
int  ShtMeasure(unsigned char mode);
void ShtCalculate(int *p_temperature, int *p_humidity, uint16_t*, uint16_t*);
char ShtReadStatus(unsigned char *p_value);
char ShtWriteStatus(unsigned char value);

#endif

