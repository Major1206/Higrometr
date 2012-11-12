#ifndef __INCLUDE__S
#define __INCLUDE__S

#define F_CPU 8000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <stdlib.h>
#include <stdint.h>

#define MAXUS 38
#define MAXMS 13

#define SET(X,Y) X|=(1<<Y)
#define USET(X,Y) X&=~(1<<Y)

#endif
