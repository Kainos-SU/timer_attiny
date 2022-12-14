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

void timerInit() {
    // Timer/Counter 0 initialization
    // Clock source: System Clock
    // Clock value: 187,500 kHz
    // Mode: CTC top=OCR0A
    // OC0A output: Disconnected
    // OC0B output: Disconnected
    // Timer Period: 0,8 ms
    TCCR0A=(0<<COM0A1) | (0<<COM0A0) | (0<<COM0B1) | (0<<COM0B0) | (1<<WGM01) | (0<<WGM00);
    TCCR0B=(0<<WGM02) | (0<<CS02) | (0<<CS01) | (1<<CS00);
    OCR0A=150;

    // Timer/Counter 1 initialization
    // Clock source: System Clock
    // Clock value: 12000,000 kHz
    // Mode: CTC top=OCR1A
    // OC1A output: Disconnected
    // OC1B output: Disconnected
    // Noise Canceler: Off
    // Input Capture on Falling Edge
    // Timer Period: 1 ms
    // Timer1 Overflow Interrupt: Off
    // Input Capture Interrupt: Off
    // Compare A Match Interrupt: On
    // Compare B Match Interrupt: Off
    TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (0<<WGM10);
    TCCR1B=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (1<<WGM12) | (0<<CS12) | (0<<CS11) | (1<<CS10);
    OCR1AH=0x2E;
    OCR1AL=0xDF;

    // Timer(s)/Counter(s) Interrupt(s) initialization
    TIMSK=(0<<TOIE1) | (1<<OCIE1A) | (0<<OCIE1B) | (0<<ICIE1) | (0<<OCIE0B) | (0<<TOIE0) | (1<<OCIE0A);

    // Global enable interrupts
    sei();
}

volatile struct simpleButtonStruct incrementButton = {false, false, 0};
volatile struct simpleButtonStruct decrementButton = {false, false, 0};


bool handleSimpleButton(bool currentState, volatile struct simpleButtonStruct* button) {
    bool result = false;
    if ((button->prevState != currentState) && (button->debounceTimer == 0)) {
        button->debounceTimer = DEBOUNCE;
        button->prevState = currentState;
        if (button->defaultState != currentState) {
            result = true;
        }
    }
    return result;
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
    }
    if (decrementButton.debounceTimer) {
        --decrementButton.debounceTimer;
    }
}

int main(void)
{
    DDRD = (1 << PIN5) | (1 << PIN6);
    PORTD = 0;
    initDispay();
    incrementButton.defaultState = (PIND >> PIN4) & 1;
    decrementButton.defaultState = (PIND >> PIN3) & 1;
    timerInit();
    PORTB = 1;
    _delay_ms(200);
    blankDisplay(TIMER);

    while(1) {
        if (timerStarted) {
            setTimerValue(milliseconds);
        }
        setCounterValue(counterToDisplay);
        if (handleSimpleButton((PIND >> PIN4) & 1, &incrementButton)) {
            if (timerStarted) {
                ++counterToDisplay;
            }
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
