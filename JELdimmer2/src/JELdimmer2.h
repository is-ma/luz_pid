#ifndef JELDIMMER2_H
#define JELDIMMER2_H

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/atomic.h>


typedef enum
{
    OFF = false,
    ON = true
} ON_OFF_typedef;

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define DIMMER_TIMER 4
#define INT_vect INT4_vect   
#define INTx INT4
#define EICRX EICRB
#define ISCx1 ISC41
#define ISCx0 ISC40

#define TCCRxA_VALUE 0x00 // modo CTC
#define TCCRxB_VALUE 0x0A // 0b1010 (1 << WGMx2)|(1 << CSx1) preescala 8
#define OCRxAH_VALUE 0x00
#define OCRxAL_VALUE 0x95 //cada 74.5uS

#define _OCRxAH(X) OCR ## X ## AH
#define OCRxAH(X) _OCRxAH(X)
#define _OCRxAL(X) OCR ## X ## AL
#define OCRxAL(X) _OCRxAL(X)

#elif defined(__AVR_ATmega32U4__)
#define DIMMER_TIMER 1
#define INT_vect INT6_vect
#define INTx INT6
#define EICRX EICRB
#define ISCx1 ISC61
#define ISCx0 ISC60

#define TCCRxA_VALUE 0x00 // modo CTC
#define TCCRxB_VALUE 0x0A// 0b1010 (1 << WGMx2)|(1 << CSx1) preescala 8
#define OCRxAH_VALUE 0x00
#define OCRxAL_VALUE 0x95 //cada 74.5uS

#define _OCRxAH(X) OCR ## X ## AH
#define OCRxAH(X) _OCRxAH(X)
#define _OCRxAL(X) OCR ## X ## AL
#define OCRxAL(X) _OCRxAL(X)

#else
#define DIMMER_TIMER 2
#define INT_vect INT0_vect
#define INTx INT0
#define EICRX EICRA
#define ISCx1 ISC01
#define ISCx0 ISC00

#define TCCRxA_VALUE 0x02 //modo CTC
#define TCCRxB_VALUE 0x02 // 0b0010 // (1 << CSx1) preescala 8
#define OCRxAH_VALUE 0x00
#define OCRxAL_VALUE 0x95 //cada 74.5uS

#define _OCRxAH(X) OCR ## X ## A
#define OCRxAH(X) _OCRxAH(X)
#define _OCRxAL(X) OCR ## X ## A
#define OCRxAL(X) _OCRxAL(X)

#endif


static const uint8_t powerBuf[] = {
	112,111,110,109,108,107,106,105,104,103,102,101,
    100, 99, 98, 97, 96, 95, 94, 93, 92, 91,
     90, 89, 88, 87, 86, 85, 84, 83, 82, 81,
     80, 79, 78, 77, 76, 75, 74, 73, 72, 71,
     70, 69, 68, 67, 66, 65, 64, 63, 62, 61,
     60, 59, 58, 57, 56, 55, 54, 53, 52, 51,
     50, 49, 48, 47, 46, 45, 44, 43, 42, 41,
     40, 39, 38, 37, 36, 35, 34, 33, 32, 31,
     30, 29, 28, 27, 26, 25, 24, 23, 22, 21,
     20, 19, 18, 17, 16, 15, 14, 13, 12, 11,
     10,  9,  8,  7,  6,  5,  4,  3,  2,  1
};

#define ALL_DIMMERS 8

class ACdimmer 
{         
    private:
        int current_num;
		int timer_num;

		
		void port_init(void);
		static void timer_init(void);
		static void ext_int_init(void);
		
    public:   
        uint16_t pulse_begin;
        int dimmer_pin;


        ACdimmer(int user_dimmer_pin);
		void begin(ON_OFF_typedef ON_OFF);
        void setPower(int power);
		int  getPower(void);
		void setState(ON_OFF_typedef ON_OFF);
        bool getState(void);
		void changeState(void);

};

#endif
