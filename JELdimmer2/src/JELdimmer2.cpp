#include "Arduino.h"
#include "JELdimmer2.h"
#include <util/atomic.h>

#define _TCNT(X) TCNT ## X
#define TCNT(X) _TCNT(X)
#define _TCCRxA(X) TCCR ## X ## A
#define TCCRxA(X) _TCCRxA(X)
#define _TCCRxB(X) TCCR ## X ## B
#define TCCRxB(X) _TCCRxB(X)
#define _TIMSKx(X) TIMSK ## X
#define TIMSKx(X) _TIMSKx(X)
#define _TIFRx(X) TIFR ## X
#define TIFRx(X) _TIFRx(X)
#define _OCFxA(X) OCF ## X ## A
#define OCFxA(X) _OCFxA(X)
#define _OCFxB(X) OCF ## X ## B
#define OCFxB(X) _OCFxB(X)
#define _TOIEx(X) TOIE ## X
#define TOIEx(X) _TOIEx(X)
#define _OCIExA(X) OCIE ## X ## A
#define OCIExA(X) _OCIExA(X)
#define _TIMER_COMPA_VECTOR(X) TIMER ## X ## _COMPA_vect
#define TIMER_COMPA_VECTOR(X) _TIMER_COMPA_VECTOR(X)


int pulseWidth = 1;
volatile int current_dim = 0;


static ACdimmer* dimmer[ALL_DIMMERS];
volatile int dimPower[ALL_DIMMERS];
volatile int dimOutPin[ALL_DIMMERS];
volatile int zeroCross[ALL_DIMMERS];

volatile ON_OFF_typedef dimState[ALL_DIMMERS];
volatile int dimCounter[ALL_DIMMERS];
static uint16_t dimPulseBegin[ALL_DIMMERS];


ACdimmer::ACdimmer(int user_dimmer_pin):
	dimmer_pin(user_dimmer_pin)
{
	current_dim++;
	dimmer[current_dim-1] = this;
	current_num = current_dim-1;
	timer_num = DIMMER_TIMER;
	
	dimPulseBegin[current_dim-1] = 1;
	dimOutPin[current_dim-1] = user_dimmer_pin;
	dimCounter[current_dim-1] = 0;
	zeroCross[current_dim-1] = 0;

	pinMode(user_dimmer_pin, OUTPUT);
}
 
void ACdimmer::timer_init(void)
{
	TCCRxA(DIMMER_TIMER) &= ~(0xFF); // TCCRxA = 0
	TCCRxB(DIMMER_TIMER) &= ~(0xFF); //  TCCRxB = 0

	TIMSKx(DIMMER_TIMER) |= (1 << OCIExA(DIMMER_TIMER)); //habilita interrupción por comparación A

	TCCRxA(DIMMER_TIMER) = TCCRxA_VALUE; //configura el temporizador
	TCCRxB(DIMMER_TIMER) = TCCRxB_VALUE;
	if (DIMMER_TIMER != 2) OCRxAH(DIMMER_TIMER) = OCRxAH_VALUE;
	OCRxAL(DIMMER_TIMER) = OCRxAL_VALUE;

	//TIMSKx(DIMMER_TIMER) |= (1 << TOIEx(DIMMER_TIMER)); //timer interrupt enable
}

void ACdimmer::ext_int_init(void) 
{ 
	EICRX &= ~0xFF; 

	EIMSK |= (1 << INTx); 
	EICRX |= (1 << ISCx1)|(1 << ISCx0);//0b00000011, interrupción en flanco de subida
}


void ACdimmer::begin(ON_OFF_typedef ON_OFF)
{
	dimState[this->current_num] = ON_OFF;
	timer_init();
	ext_int_init();	
}

void ACdimmer::setPower(int power)
{	
    power+=7;
    
	if (power >= 105) 
	{
		power = 105;
	}
	dimPower[this->current_num] = power;
	dimPulseBegin[this->current_num] = powerBuf[power];
	
	delay(1);
}

int ACdimmer::getPower(void)
{
	if (dimState[this->current_num] == ON)
		return dimPower[this->current_num]-6;
	else return 0;
}

void ACdimmer::setState(ON_OFF_typedef ON_OFF)
{
	dimState[this->current_num] = ON_OFF;
}

bool ACdimmer::getState(void)
{
	bool ret;
	if (dimState[this->current_num] == ON) ret = true;
	else ret = false;
	return ret;
}

void ACdimmer::changeState(void)
{
	if (dimState[this->current_num] == ON) dimState[this->current_num] = OFF;
	else 
		dimState[this->current_num] = ON;
}


 
ISR(INT_vect)
{
	for (int i = 0; i < current_dim; i++ ) 
		if (dimState[i] == ON) 
		{
			zeroCross[i] = 1;
		}
}


static int k;
ISR (TIMER_COMPA_VECTOR(DIMMER_TIMER))
{	

	for (k = 0; k < current_dim; k++)
	{
		if (zeroCross[k] == 1 )
		{
			dimCounter[k]++;


			if (dimCounter[k] >= dimPulseBegin[k] )
			{
				digitalWrite(dimOutPin[k], HIGH);	
			}

			if (dimCounter[k] >=  (dimPulseBegin[k] + pulseWidth) )
			{
				digitalWrite(dimOutPin[k], LOW);
				zeroCross[k] = 0;
				dimCounter[k] = 0;
			}
		}
	}

	//TIFRx(DIMMER_TIMER) |= ((1<<OCFxB(DIMMER_TIMER))|(1<<OCFxA(DIMMER_TIMER)));	//limpia banderas de interrupción
	TIFRx(DIMMER_TIMER) |= (1<<OCFxA(DIMMER_TIMER));	//limpia bandera de interrupción
}
