#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include "stm32f0xx_hal.h"
#include <stdint.h>

// Enum to define port IDs
typedef enum {
    inner_port1_warm = 0,
    inner_port1_white = 1,
    outer_port1_warm = 2,
    outer_port1_white = 3,
    ALL_PORTS = 10
} LEDPort;

// Enum for color modes
typedef enum {
    COOL_WHITE = 0,
    WARM_WHITE = 1,
    NEUTRAL_WHITE = 2
} ColorMode;

// Externally accessible data
extern const int32_t intensityLevels[];
extern const int32_t intensityTable[3][10];

// Function declarations
void initializeLEDs(void);
void led_pwm_setvalue(int _channel, int32_t value);
void setLEDIntensity(int port, int32_t intensity);
int32_t getIntensityLevel(int level);
void handleSensorInput(void);
void setColorMode(ColorMode mode, int level);
void setColorModeToEndo(ColorMode mode, int level);
void lampOn(void);
void lampOff(void);
void setEndoMode(int mode);
void setDepthMode(int mode);

void smoothSetLEDIntensity(int port, int32_t startIntensity, int32_t endIntensity, int stepDelay);

#endif // LED_CONTROL_H
