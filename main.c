/*
 */

#include <avr/io.h>
#include<avr/interrupt.h>
#include <stdbool.h>
#include "modules/cd4017_control.h"
#include <util/delay.h>
#define DEBOUNCE 60
#define HOLD 200


volatile unsigned long long int milliseconds = 0;
volatile unsigned int counterToDisplay = 0;
volatile bool timerStarted = false;

volatile struct simpleButtonStruct {
    bool defaultState: 1;
    bool prevState: 1;
    unsigned int debounceTimer: 6;
} simpleButton;

volatile struct fullButtonStruct {
    bool defaultState: 1;
    bool pressed: 1;
    bool prevState: 1;
    uint16_t timer: 8;
} fullButton;

bool handleSimpleButton(bool currentState) {
    if ((simpleButton.defaultState == simpleButton.prevState) && (simpleButton.prevState != currentState) && !simpleButton.debounceTimer) {
        simpleButton.debounceTimer = DEBOUNCE;
        simpleButton.prevState = currentState;
        return true;
    }
    if ((simpleButton.defaultState != simpleButton.prevState) && (simpleButton.prevState != currentState) && !simpleButton.debounceTimer) {
        simpleButton.debounceTimer = DEBOUNCE;
        simpleButton.prevState = currentState;
        return false;
    }
    return false;
}

/*
0 - no status
1 - press
2 - hold
*/
char handleFullButton (bool currentStatus) {
    if ((fullButton.defaultState == fullButton.prevState) && (fullButton.prevState != currentStatus)) {
        fullButton.prevState = currentStatus;
        fullButton.pressed = true;
        fullButton.timer = 0;
        return 0;
    }
    if ((fullButton.defaultState == currentStatus) && fullButton.pressed && (fullButton.timer >= HOLD)) {
        fullButton.pressed = false;
        fullButton.prevState = currentStatus;
        return 2;
    }
    if ((fullButton.defaultState != fullButton.prevState) && (fullButton.prevState != currentStatus) && (fullButton.timer >= DEBOUNCE) && (fullButton.timer < HOLD)) {
        fullButton.pressed = false;
        fullButton.prevState = currentStatus;
        return 1;
    }
    return 0;
}

ISR(TIMER0_COMPA_vect) {
    tick();
}

ISR(TIMER1_COMPA_vect) {
    if (timerStarted) {
        ++milliseconds;
    }
    if (simpleButton.debounceTimer) {
        --simpleButton.debounceTimer;
    }
    ++fullButton.timer;
}

int main(void)
{
    DDRD = (1 << PIN5) | (1 << PIN6);
    PORTD = 0;
    initDispay();
    OCR0A = 150;
    TCCR0A |= 1 << WGM01;
    TCCR0B |= 1 << CS00;
    simpleButton.defaultState = (PIND >> PIN4) & 1;
    TCCR1B |= (1 << WGM12) | (1 << CS10) | (1 << CS11);
    OCR1AL = 187;
    OCR1AH = 0;
    TIMSK = (1 << OCIE0A) | (1 << OCIE1A);
    sei();
    _delay_ms(200);

    while(1) {
        setTimerValue(milliseconds);
        setCounterValue(counterToDisplay);
        if (handleSimpleButton((PIND >> PIN4) & 1)) {
            if (timerStarted) ++counterToDisplay;
            timerStarted = true;
            milliseconds = 0;
        }
        uint8_t secondButton = handleFullButton((PIND >> PIN3) & 1);
        if (secondButton == 2) {
            counterToDisplay = 0;
            timerStarted = false;
            milliseconds = 0;
        } else if (secondButton == 1) {
            --counterToDisplay;
        }
    }

    return 0;
}
