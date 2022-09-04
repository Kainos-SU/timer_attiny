/*
 */

#include <avr/io.h>
#include<avr/interrupt.h>
#include <stdbool.h>
#include "modules/cd4017_control.h"
#include <util/delay.h>


volatile unsigned long long int milliseconds = 0;


ISR(TIMER0_COMPA_vect) {
    tick();
}

int main(void)
{
    DDRD = (1 << PIN5) | (1 << PIN6);
    PORTD = 0;
    initDispay();
    OCR0A = 150;
    TCCR0A |= 1 << WGM01;
    TCCR0B |= 1 << CS00;
    TIMSK = (1 << OCIE0A);
    sei();
    _delay_ms(200);
    setTimerValue(7152);
    setCounterValue(0);

    while(1) ;

    return 0;
}
