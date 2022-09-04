#include <avr/io.h>
#include <stddef.h>
#include <stdbool.h>
#include "cd4017_control.h"


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
        ledController.indicatorData[i] = NUMBERS_TABLE[i + 1] - 1;
        // ledController.indicatorData[i] = 0xFF;
    }
}

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

void setTimerValue (unsigned long long int value) { // value in miliseconds
    uint8_t digitsOfTimer[7];
    uint8_t digitsToDispaly[DIGITS_PER_DISPLAY];
    unsigned long int temp = value / 1000;
    uint8_t seconds = temp % 60;
    uint8_t minutes = temp / 60;
    uint8_t counter = 0;
    temp = value;
    for (int i = 6; i >= 0; --i) {
        if (i == 3) {
            temp = seconds;
        }
        if (i == 1) {
            temp = minutes;
        }
        digitsOfTimer[i] = temp % 10;
        temp = temp / 10;
    }
    for (int i = 0; i < 7; ++i) {
        if (!digitsOfTimer[i] && !counter) {
            continue;
        }
        uint8_t index = DIGITS_PER_DISPLAY - (counter + 1);
        digitsToDispaly[index] = NUMBERS_TABLE[digitsOfTimer[i]];
        if ((i == 1) || (i == 3)) {
            digitsToDispaly[index] -= 1;
        }
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


