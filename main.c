/*
 */

#include <avr/io.h>
#include<avr/interrupt.h>
#include <stdbool.h>
#include "modules/cd4017_control.h"


volatile unsigned long long int miliseconds = 0;

void tickReset() {
    PORTD ^= 1 << PIN5;
}

void tickClock() {
    if (PORTD & (1 << PIN5)) {
            tickReset();
            return;
    }
    PORTD ^= 1 << PIN6;
}


ISR(TIMER0_COMPA_vect) {
    if (tick()) {
        tickReset();
        return;
    }
    tickClock();
}

ISR(TIMER1_COMPA_vect) {
    miliseconds++;
}

int main(void)
{
    DDRD = (1 << PIN5) | (1 << PIN6);
    PORTD = 0;
    initDispay();
    OCR0A = 150;
    TCCR0A |= 1 << WGM01;
    TCCR0B |= 1 << CS00;
    // TCCR1A |= ;
    TCCR1B |= (1 << WGM12) | (1 << CS11);
    OCR1AH = 0x05;
    OCR1AL = 0xDB;
    TIMSK = (1 << OCIE0A) | (1 << OCIE1A);
    sei();

    while(1) {
        setTimerValue(miliseconds);
    }

    return 0;
}
