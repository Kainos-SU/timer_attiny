#include <avr/io.h>
#include <stddef.h>
#include <stdbool.h>
#include "cd4017_control.h"
#define TIMERS_DIGITS 5


const uint8_t NUMBERS_TABLE [] = {
    0x03, // 0
    0x9F, // 1
    0x25, // 2
    0x0D, // 3
    0x99, // 4
    0x49, // 5
    0x41, // 6
    0x1F, // 7
    0x01, // 8
    0x09  // 9
};

static volatile struct LedController ledController = { 0, false };

void initDispay() {
    DDRB = 0xFF;
    PORTB = 0xFF;
    for (int i = 0; i < NUMBER_OF_DIGITS; ++i) {
        ledController.indicatorData[i] = 0x00;
    }
}

// Переключення живлення на аноди
void tick() {
    PORTB = 0xFF;
    ledController.hightLevel = !ledController.hightLevel;
    if (!ledController.hightLevel) {
        ledController.indicatorCounter++;
    }
    if (ledController.indicatorCounter == NUMBER_OF_DIGITS) {
        ledController.indicatorCounter = 0;
        PORTD ^= 1 << PIN5;
        PORTB = ledController.indicatorData[ledController.indicatorCounter];
        return;
    }
    if (PORTD & (1 << PIN5)) {
        PORTD ^= 1 << PIN5;
    }
    PORTD ^= 1 << PIN6;
    if (!ledController.hightLevel) {
        PORTB = ledController.indicatorData[ledController.indicatorCounter];
    }
}

void setArrayValue (uint8_t array[4], uint8_t length, uint8_t arrayStart) {
    int i = 0;
    for (i = arrayStart; i < arrayStart + DIGITS_PER_DISPLAY; ++i) {
        ledController.indicatorData[i] = 0xFF;
    }
    for (i = arrayStart; i < arrayStart + length; ++i) {
        ledController.indicatorData[i] = array[i - arrayStart];
    }
}

void blankDisplay(enum DISPLAYS display) {
    uint8_t blankArray[4] = {0xFF, 0xFF, 0xFF, 0xFF};
    if(display == COUNTER) {
        setArrayValue(blankArray, 4, DIGITS_PER_DISPLAY);
    } else {
        setArrayValue(blankArray, 4, 0);
    }
}

// Функція отримання окремих цифр часу
uint8_t separateTimeDigits (unsigned long int seconds, uint8_t *array, uint8_t arrayLength) {
    uint8_t counter = 0;
    do {
        uint8_t digit;
        if ((counter++ % 2) && (seconds >= 6)) {
            digit = seconds % 6;
            seconds = (seconds - digit) / 6;
        } else {
            digit = seconds % 10;
            seconds /= 10;
        }
        *array = digit;
        array++;

    } while (seconds);
    return counter;
}

void setTimerValue (unsigned long long int value) { // value in miliseconds
    uint8_t digitsOfTimer[TIMERS_DIGITS];
    uint8_t digitsToDispaly[DIGITS_PER_DISPLAY];
    uint8_t counter = 0;
    unsigned long int seconds = value / 1000;
    uint8_t numberOfDigits = separateTimeDigits(seconds, digitsOfTimer, TIMERS_DIGITS);
    uint8_t *pointerToDisplayDigits = numberOfDigits > DIGITS_PER_DISPLAY ? (digitsToDispaly + DIGITS_PER_DISPLAY - 1) : (digitsToDispaly + numberOfDigits - 1);
    for (int i = numberOfDigits - 1; i >= 0; --i) {
        *pointerToDisplayDigits = NUMBERS_TABLE[digitsOfTimer[i]];
        if ((i == 2) || (i == 4)) {
            *pointerToDisplayDigits -= 1;
        }
        --pointerToDisplayDigits;
        ++counter;
        if (counter == DIGITS_PER_DISPLAY) {
            break;
        }
    }
    setArrayValue(digitsToDispaly, counter, 0);
}

void setCounterValue (uint16_t value) {
    uint8_t counter = 0;
    uint8_t digitsOfValue[DIGITS_PER_DISPLAY];
    do {
        digitsOfValue[counter] = NUMBERS_TABLE[value % 10];
        value = (uint16_t)(value / 10);
        counter++;
    } while (value);
    setArrayValue(digitsOfValue, counter, DIGITS_PER_DISPLAY);
}


