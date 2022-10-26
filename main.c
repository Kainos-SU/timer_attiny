#include <avr/io.h>
#include<avr/interrupt.h>
#include <stdbool.h>
#include "modules/cd4017_control.h"
#include <util/delay.h>
#define DEBOUNCE 560


volatile unsigned long long int milliseconds = 0;
volatile unsigned int counterToDisplay = 0;
volatile bool timerStarted = false;

struct simpleButtonStruct {
    bool defaultState: 1;
    bool prevState: 1;
    unsigned int debounceTimer: 14;
};

struct simpleButtonStruct incrementButton = {false, false, 0};
struct simpleButtonStruct decrementButton = {false, false, 0};

bool handleSimpleButton(bool currentState, struct simpleButtonStruct* button) {
    if ((button->defaultState == button->prevState) && (button->prevState != currentState) && !(button->debounceTimer)) {
        button->debounceTimer = DEBOUNCE;
        button->prevState = currentState;
        return true;
    }
    if ((button->defaultState != button->prevState) && (button->prevState != currentState) && !(button->debounceTimer)) {
        button->debounceTimer = DEBOUNCE;
        button->prevState = currentState;
        return false;
    }
    return false;
}

ISR(TIMER0_COMPA_vect) {
    tick();
}

ISR(TIMER1_COMPA_vect) {
    if (timerStarted) {
        ++milliseconds;
    }
    if (incrementButton.debounceTimer) {
        --incrementButton.debounceTimer;
    }else if (decrementButton.debounceTimer) {
        --decrementButton.debounceTimer;
    }
}

int main(void)
{
    DDRD = (1 << PIN5) | (1 << PIN6);
    PORTD = 0;
    initDispay();
    OCR0A = 150;
    TCCR0A |= 1 << WGM01;
    TCCR0B |= 1 << CS00;
    incrementButton.defaultState = (PIND >> PIN4) & 1;
    decrementButton.defaultState = (PIND >> PIN3) & 1;
    TCCR1B |= (1 << WGM12) | (1 << CS10) | (1 << CS11);
    OCR1AL = 186;
    OCR1AH = 0;
    TIMSK = (1 << OCIE0A) | (1 << OCIE1A);
    sei();
    _delay_ms(200);
    blankDisplay(TIMER);

    while(1) {
        if (timerStarted) {
            setTimerValue(milliseconds);
        }
        setCounterValue(counterToDisplay);
        if (handleSimpleButton((PIND >> PIN4) & 1, &incrementButton)) {
            if (timerStarted) ++counterToDisplay;
            timerStarted = true;
            milliseconds = 0;
        }
        if (handleSimpleButton((PIND >> PIN3) & 1, &decrementButton)) {
            if (counterToDisplay) {
                --counterToDisplay;
            }
        }
    }

    return 0;
}
