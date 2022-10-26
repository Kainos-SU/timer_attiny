#ifndef CD4017_CONTROL_H_INCLUDED
#define CD4017_CONTROL_H_INCLUDED
#define NUMBER_OF_DIGITS 8
#define DIGITS_PER_DISPLAY 4
enum DISPLAYS { COUNTER, TIMER };
struct LedController {
    uint8_t indicatorCounter : 4;
    bool hightLevel;
    uint8_t indicatorData[NUMBER_OF_DIGITS];
    uint8_t indexesWithDot[3];
};
void initDispay();
void tick();
void setCounterValue (uint16_t value);
void setTimerValue (unsigned long long int value);
void blankDisplay(enum DISPLAYS display);

#endif // CD4017_CONTROL_H_INCLUDED
