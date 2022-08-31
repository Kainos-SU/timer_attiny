#ifndef CD4017_CONTROL_H_INCLUDED
#define CD4017_CONTROL_H_INCLUDED
#define NUMBER_OF_DIGITS 4
#define DIGITS_PER_DISPLAY 4
struct LedController {
    uint8_t indicatorCounter : 4;
    bool hightLevel;
    uint8_t indicatorData[NUMBER_OF_DIGITS];
    uint8_t indexesWithDot[3];
};
void initDispay();
bool tick();
void setTimerValue (unsigned long int value);

#endif // CD4017_CONTROL_H_INCLUDED
