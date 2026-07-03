#include "led_control.h"
#include "mode_handler.h"



extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim14;
extern TIM_HandleTypeDef htim15;
extern TIM_HandleTypeDef htim16;

extern uint8_t state_change_flag;
const int32_t intensitydepth[4] = { 50, 75, 89, 85 };
int x =0;
//const int32_t intensityTable[3][10] = {
//  { 35, 45, 55, 65, 75, 80, 85, 87, 89, 92 },  // Cool White
//  { 35, 45, 55, 65, 75, 80, 85, 87, 89, 92 },  // Warm White
//  { 35, 45, 55, 65, 75, 80, 85, 87, 89, 92 }   // Neutral White
//};

const int32_t intensityTable[3][10] = {
  { 50, 55, 60, 65, 75, 80, 85, 87, 89, 92 },  // Cool White
  { 50, 55, 60, 65, 75, 80, 85, 87, 89, 92 },  // Warm White
  { 50, 55, 60, 65, 75, 80, 85, 87, 89, 92 }   // Neutral White
};



static ColorMode currentColorMode = NEUTRAL_WHITE;
static int currentIntensityLevel = 1;


void initializeLEDs(void) {



  // Set default states
  lampState = 1;                     // Lamp on
  endoMode = 0;                      // Endo mode off
  currentColorMode = NEUTRAL_WHITE;  // Default color mode
  currentIntensityLevel = 1;         // Default intensity level
                           //
                                     //
  setColorMode(NEUTRAL_WHITE, 1);


  // Smoothly transition all LEDs to full brightness
  //    for (int port = inner_port1_warm; port <= outer_port3_white; port++) {
  //        smoothSetLEDIntensity(port, 0, intensityTable[currentColorMode][currentIntensityLevel - 1], 5); // Step delay of 5 ms
  //    }
  //
  //    // Initialize red and green lights to off
  //    smoothSetLEDIntensity(green_light, 0, 0, 5);
  //    smoothSetLEDIntensity(red_light, 0, 0, 5);

  // Additional hardware-specific initialization
  // For example, configuring timers, setting GPIOs, etc.
}

int chan = 0;
int va = 0;
void led_pwm_setvalue(int _channel, int32_t value) {
  value = 100 - value;
  chan = _channel;
  va = value;
  switch (_channel) {
    case inner_port1_white:
      TIM1->CCR3 = value;
      break;
    case inner_port1_warm:
      TIM1->CCR4 = value;
      break;
    case outer_port1_white:
      TIM3->CCR4 = value;
      break;
    case outer_port1_warm:
      TIM3->CCR1 = value;
      break;
    default:
      break;
  }
}



void setLEDIntensity(int port, int32_t intensity) {
  if (port == ALL_PORTS) {
    led_pwm_setvalue(inner_port1_warm, intensity);
    led_pwm_setvalue(inner_port1_white, intensity);
    led_pwm_setvalue(outer_port1_warm, intensity);
    led_pwm_setvalue(outer_port1_white, intensity);
  } else {
    led_pwm_setvalue(port, intensity);
  }
}
void setFieldIntensity(FieldType mode) {
  int32_t intensityValue;
  int32_t intensityValue1;
  if (fieldmode) {
    if (mode == small) {
      //    		focusValue = 2;
      intensityValue = intensitydepth[small - 1];
      intensityValue1 = intensitydepth[small - 1];
    }
    if (mode == medium) {
      //    		focusValue = 5;
      intensityValue = intensitydepth[medium - 1];
      intensityValue1 = intensitydepth[medium - 1];
    }
    if (mode == large) {
      //    		focusValue = 9;
      intensityValue = intensitydepth[large - 1];
      intensityValue1 = intensitydepth[3];
    }
    //    	int32_t intensityValue = intensityTable[NEUTRAL_WHITE][focusValue];
    setLEDIntensity(inner_port1_white, intensityValue1);
    setLEDIntensity(inner_port1_warm, intensityValue1);
    setLEDIntensity(outer_port1_white, intensityValue);
    setLEDIntensity(outer_port1_warm, intensityValue);
  }
}
int32_t getIntensityLevel(int level) {
  if (level < 1 || level > 10) return 0;
  return intensityLevels[level - 1];
}




void setColorMode(ColorMode mode, int level) {
  currentColorMode = mode;        // Update current color mode
  currentIntensityLevel = level;  // Update current intensity level

  int32_t intensityValue = intensityTable[mode][level - 1];
  if (mode == COOL_WHITE) {
    setLEDIntensity(inner_port1_white, intensityValue);
    setLEDIntensity(outer_port1_white, intensityValue);


    setLEDIntensity(inner_port1_warm, 0);
    setLEDIntensity(outer_port1_warm, 0);

  } else if (mode == WARM_WHITE) {
    setLEDIntensity(inner_port1_warm, intensityValue);
    setLEDIntensity(outer_port1_warm, intensityValue);


    setLEDIntensity(inner_port1_white, 0);
    setLEDIntensity(outer_port1_white, 0);

  } else if (mode == NEUTRAL_WHITE) {
    setLEDIntensity(inner_port1_white, intensityValue);
    setLEDIntensity(inner_port1_warm, intensityValue);
    setLEDIntensity(outer_port1_white, intensityValue);
    setLEDIntensity(outer_port1_warm, intensityValue);
  }
}

void setColorModeToEndo(ColorMode mode, int level) {
  currentColorMode = mode;        // Update current color mode
  currentIntensityLevel = level;  // Update current intensity level

  int32_t intensityValue = intensityTable[mode][level - 1];
  if (mode == COOL_WHITE) {
    setLEDIntensity(inner_port1_white, intensityValue);
    setLEDIntensity(inner_port1_warm, 0);  // Turn off warm for cool mode
  } else if (mode == WARM_WHITE) {
    setLEDIntensity(inner_port1_warm, intensityValue);
    setLEDIntensity(inner_port1_white, 0);  // Turn off cool for warm mode
  } else if (mode == NEUTRAL_WHITE) {
    setLEDIntensity(inner_port1_white, intensityValue);
    setLEDIntensity(inner_port1_warm, intensityValue);
  }
  setLEDIntensity(outer_port1_warm, 0);
  setLEDIntensity(outer_port1_white, 0);
}

void lampOn(void) {
  // Full brightness based on color mode
  int32_t intensityValue;
  int32_t intensityValue1;
  if (fieldmode) {
    if (prvFocusMode == small) {
      //    		focusValue = 2;
      intensityValue = intensitydepth[0];
      intensityValue1 = intensitydepth[0];
    }
    if (prvFocusMode == medium) {
      //    		focusValue = 5;
      intensityValue = intensitydepth[1];
      intensityValue1 = intensitydepth[1];
    }
    if (prvFocusMode == large) {
      //    		focusValue = 9;
      intensityValue = intensitydepth[2];
      intensityValue1 = intensitydepth[3];
    }
    //    	int32_t intensityValue = intensityTable[NEUTRAL_WHITE][focusValue];
    setLEDIntensity(inner_port1_white, intensityValue1);
    setLEDIntensity(inner_port1_warm, intensityValue1);
    setLEDIntensity(outer_port1_white, intensityValue);
    setLEDIntensity(outer_port1_warm, intensityValue);
  } else {
    int32_t intensityValue = intensityTable[currentColorMode][9];

    if (!endoMode) {
      if (currentColorMode == COOL_WHITE) {
        setLEDIntensity(inner_port1_white, intensityValue);
        setLEDIntensity(outer_port1_white, intensityValue);
      } else if (currentColorMode == WARM_WHITE) {
        setLEDIntensity(inner_port1_warm, intensityValue);
        setLEDIntensity(outer_port1_warm, intensityValue);
      } else if (currentColorMode == NEUTRAL_WHITE) {
        setLEDIntensity(inner_port1_white, intensityValue);
        setLEDIntensity(inner_port1_warm, intensityValue);
        setLEDIntensity(outer_port1_white, intensityValue);
        setLEDIntensity(outer_port1_warm, intensityValue);
      }
    } else {
      setLEDIntensity(outer_port1_warm, 0);
      setLEDIntensity(outer_port1_white, 0);

      if (currentColorMode == COOL_WHITE) {
        setLEDIntensity(inner_port1_white, intensityValue);

      } else if (currentColorMode == WARM_WHITE) {
        setLEDIntensity(inner_port1_warm, intensityValue);

      } else if (currentColorMode == NEUTRAL_WHITE) {
        setLEDIntensity(inner_port1_white, intensityValue);
        setLEDIntensity(inner_port1_warm, intensityValue);
      }
    }
  }
}

void lampOff(void) {
  setLEDIntensity(ALL_PORTS, 0);  // Turn off all LEDs
}

void setEndoMode(int mode) {
  int32_t intensityValue = intensityTable[currentColorMode][9];  // Full brightness based on color mode
  if (mode == 1) {
    if (currentColorMode == COOL_WHITE) {
      setLEDIntensity(inner_port1_white, intensityValue);
      setLEDIntensity(inner_port1_warm, 0);
    } else if (currentColorMode == WARM_WHITE) {
      setLEDIntensity(inner_port1_warm, intensityValue);
      setLEDIntensity(inner_port1_white, 0);
    } else if (currentColorMode == NEUTRAL_WHITE) {
      setLEDIntensity(inner_port1_white, intensityValue);
      setLEDIntensity(inner_port1_warm, intensityValue);
    }
    setLEDIntensity(outer_port1_warm, 0);
    setLEDIntensity(outer_port1_white, 0);
  } else {
    //        setLEDIntensity(ALL_PORTS, 0); // Turn off all LEDs in Endo mode
    if (currentColorMode == COOL_WHITE) {
      setLEDIntensity(inner_port1_white, intensityValue);
      setLEDIntensity(outer_port1_white, intensityValue);
    } else if (currentColorMode == WARM_WHITE) {
      setLEDIntensity(inner_port1_warm, intensityValue);
      setLEDIntensity(outer_port1_warm, intensityValue);
    } else if (currentColorMode == NEUTRAL_WHITE) {
      setLEDIntensity(inner_port1_white, intensityValue);
      setLEDIntensity(inner_port1_warm, intensityValue);
      setLEDIntensity(outer_port1_white, intensityValue);
      setLEDIntensity(outer_port1_warm, intensityValue);
    }
  }
}

void setDepthMode(int mode) {

  if (mode == 1) {
    int32_t intensityValue = intensityTable[currentColorMode][9];  // Full brightness based on color mode
                                              //    		focusValue = 9;
    intensityValue = intensitydepth[2];
    setLEDIntensity(ALL_PORTS, intensityValue);
//    setLEDIntensity(inner_port1_white, intensityValue);
//    setLEDIntensity(inner_port1_warm, intensityValue);
//    setLEDIntensity(outer_port1_white, intensityValue);
//    setLEDIntensity(outer_port1_warm, intensityValue);

  } else {
    setLEDIntensity(currentColorMode, currentIntensityLevel);

  }
}


void smoothSetLEDIntensity(int port, int32_t startIntensity, int32_t endIntensity, int stepDelay) {
  if (startIntensity < endIntensity) {
    for (int32_t value = startIntensity; value <= endIntensity; value++) {
      setLEDIntensity(port, value);
      HAL_Delay(stepDelay);  // Adjust delay for smoothness
    }
  } else if (startIntensity > endIntensity) {
    for (int32_t value = startIntensity; value >= endIntensity; value--) {
      setLEDIntensity(port, value);
      HAL_Delay(stepDelay);  // Adjust delay for smoothness
    }
  }
}
