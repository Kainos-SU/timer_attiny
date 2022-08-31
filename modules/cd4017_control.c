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
        ledController.indicatorData[i] = 0x00;
    }
}

bool tick() {
    ledController.hightLevel = !ledController.hightLevel;
    if (!ledController.hightLevel) {
        ledController.indicatorCounter++;
    }
    if (ledController.indicatorCounter == NUMBER_OF_DIGITS) {
        ledController.indicatorCounter = 0;
        PORTB = ledController.indicatorData[ledController.indicatorCounter];
        return true;
    }
    PORTB = ledController.indicatorData[ledController.indicatorCounter];
    return false;
}

void setTimerValue (unsigned long int value) { // value in miliseconds
    uint8_t minutes = 0;
    uint8_t seconds = 0;
    unsigned long int temp = value;
    uint8_t counter = 0;
    uint8_t digitsToDisplay[7];
    for (int i = 6; i >= 0; --i) {
        digitsToDisplay[i] = temp % 10;
        temp /= 10;
        if (i == 4) {
            seconds = temp % 60;
            minutes = (uint8_t)(temp / 60);
            temp = seconds;
        }
        if (i == 2) {
            temp = minutes;
        }
    }
    for (int i = 0; i < 7; ++i) {
        if (i > 3) {
            ledController.indicatorData[counter++] = digitsToDisplay[i];
        } else {
            if (digitsToDisplay[i] || counter) {
                ledController.indicatorData[counter++] = digitsToDisplay[i];
            }
        }
        if (counter == DIGITS_PER_DISPLAY) {
            break;
        }
    }
    if (counter < DIGITS_PER_DISPLAY) {
        for (; counter <= 4; ++counter) {
            for (int i = counter; i >= 0; --i) {
                ledController.indicatorData[i + 1] = ledController.indicatorData[i];
                ledController.indicatorData[i] = 0xFF;
            }
        }
    }

}
