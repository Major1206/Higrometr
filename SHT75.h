#ifndef SHT75
#define SHT75

#define SHT_FCPU 20

#define SHT_TEMPERATURE 	0x03
#define SHT_HUMIDITY 		0x05

#define SHT_DDR			DDRC
#define SHT_PORT		PORTC
#define SHT_PIN			PINC

#define SHT_CLOCK		5
#define SHT_DATA		4

typedef uint8_t SHT_Command ;
typedef uint16_t SHT_Data ;

#define SET(X,Y) X|=(1<<Y)
#define UNSET(X,Y) X&=~(1<<Y)

#define SCKH {SET(SHT_DDR,SHT_CLOCK); SET(SHT_DDR,SHT_CLOCK);} 
#define SCKL {SET(SHT_DDR,SHT_CLOCK); UNSET(SHT_PORT,SHT_CLOCK);}

#define SDAH {SET(SHT_DDR,SHT_DATA); SET(SHT_PORT,SHT_DATA); }
#define SDAL {SET(SHT_DDR,SHT_DATA); UNSET(SHT_PORT,SHT_DATA);}

#define SDAIN {UNSET(SHT_DDR,SHT_DATA); SET(SHT_PORT,SHT_DATA);}

#define SHT_DEL 25

void SHT_DelayUs(unsigned int deltime);

void SHT_Start();
uint8_t SHT_Cmd(SHT_Command);
void SHT_Read(SHT_Data*);

#endif
