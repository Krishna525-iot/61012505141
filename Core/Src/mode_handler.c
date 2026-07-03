/* ============================= */
/*       mode_handler.c          */
/* ============================= */
#include "mode_handler.h"
#include "led_control.h"
#include "main.h"

uint8_t a = 0;

uint8_t lampState = 0;  // 0: Off, 1: On
uint8_t depthMode = 0;  // 0: Off, 1: On
uint8_t fieldmode = 0;  // 0: Off, 1: On
uint8_t endoMode = 0;   // 0: Off, 1: On
uint8_t prevEndoMode = 0;
uint8_t prevIntensity = 1;
uint8_t prvFocusMode = 0;

static ColorMode prevColorMode = NEUTRAL_WHITE;
// mode_handler.h or led_control.h

Command parseCommand(const char *cmdString) {

  //    HAL_Delay(500);  // 500ms delay for visible blink
  Command cmd = { CMD_UNKNOWN, 0, 0 };
  if (cmdString[0] != '@') {
    memset(getRxBuffer(), 0, 5);
    return cmd;
  }


  switch (cmdString[1]) {

    case 'I':
      cmd.type = CMD_INTENSITY;
      if (cmdString[2] == '0' && cmdString[3] == ':') {
        cmd.value = 10;  // Special case for intensity level 10
      } else {
        cmd.value = atoi(&cmdString[2]);  // Normal case for intensity levels 1–9
      }
      break;
    case 'C':
      cmd.type = CMD_COLOR;
      cmd.subType = cmdString[2];
      cmd.value = atoi(&cmdString[3]);
      break;
    case 'L':

      cmd.type = CMD_LAMP;
      cmd.value = cmdString[3] - '0';
      break;
    case 'E':
      cmd.type = CMD_ENDO;
      cmd.value = cmdString[3] - '0';
      break;
    case 'D':
      cmd.type = CMD_DEPTH;
      cmd.value = cmdString[3] - '0';
      break;
    case 'F':
      cmd.type = CMD_FIELD;
      cmd.subType = cmdString[2];
      cmd.value = cmdString[3] - '0';

      if ((cmd.value == 1 || cmd.value == 2 || cmd.value == 3)) {
        fieldmode = 1;
      } else {
        fieldmode = 0;
      }
      break;
    case 'M':  // Relay 3 (H1)
      cmd.type = CMD_RELAY;
      cmd.subType = cmdString[1];      // Identify which relay (B, P, H)
      cmd.value = cmdString[3] - '0';  // Parse ON (1) or OFF (0)
      break;
    default:
      break;
  }
  return cmd;
}


void handleCommand(const Command cmd) {
  // Check if lamp is off, ignore all commands except lamp commands
  if (cmd.type == CMD_DEPTH) {
    if (cmd.value) {
      depthMode = 1;
      setDepthMode(depthMode);

    } else {
      depthMode = 0;
      setDepthMode(depthMode);
    }
  }

  if (lampState == 0 && cmd.type != CMD_LAMP) {
    if (cmd.type == CMD_INTENSITY || cmd.type == CMD_COLOR || cmd.type == CMD_ENDO || cmd.type == CMD_DEPTH)
      ;
    lampOff();
    return;
  }

  // Check if depth mode is on, allow only lamp commands
  if (depthMode == 1 && cmd.type != CMD_LAMP) {
    return;
  }
  if (fieldmode == 1) {
    if (cmd.type == CMD_INTENSITY || cmd.type == CMD_COLOR || cmd.type == CMD_ENDO || cmd.type == CMD_DEPTH)
      ;
  }

  switch (cmd.type) {
    case CMD_INTENSITY:
      prevIntensity = cmd.value;  // Save intensity for restoration
      if (endoMode == 1) {
        // Directly set color mode to Endo with intensity
        setColorModeToEndo(prevColorMode, cmd.value);
      } else {
        setColorMode(prevColorMode, cmd.value);
      }
      break;
    case CMD_COLOR:
      prevColorMode = (cmd.subType == '+') ? COOL_WHITE : (cmd.subType == '-') ? WARM_WHITE
                                                                               : NEUTRAL_WHITE;  // Save color mode
      if (endoMode == 1) {
        if (cmd.subType == '+') {
          setColorModeToEndo(COOL_WHITE, cmd.value);
        } else if (cmd.subType == '-') {
          setColorModeToEndo(WARM_WHITE, cmd.value);
        } else if (cmd.subType == '0') {
          setColorModeToEndo(NEUTRAL_WHITE, cmd.value);
        }
      } else {
        if (cmd.subType == '+') {
          setColorMode(COOL_WHITE, cmd.value);
        } else if (cmd.subType == '-') {
          setColorMode(WARM_WHITE, cmd.value);
        } else if (cmd.subType == '0') {
          setColorMode(NEUTRAL_WHITE, cmd.value);
        }
      }
      break;
    case CMD_LAMP:
      lampState = cmd.value;
      if (lampState == 1) {
        if (fieldmode) {
          if (prvFocusMode == small) {
            setFieldIntensity(small);
          } else if (prvFocusMode == medium) {
            setFieldIntensity(medium);
          } else if (prvFocusMode == large) {
            setFieldIntensity(large);
          }
        } else {
          lampOn();
          if (endoMode == 0) {
            setColorMode(prevColorMode, prevIntensity);
          } else if (endoMode == 1) {
            setColorModeToEndo(prevColorMode, prevIntensity);
          }
        }
      } else {
        lampOff();
      }
      break;

    case CMD_ENDO:
      prevEndoMode = endoMode;
      if (cmd.value == 1) {
        endoMode = cmd.value;
        setEndoMode(endoMode);
        setColorModeToEndo(prevColorMode, prevIntensity);
      } else if (cmd.value == 0) {
        endoMode = cmd.value;
        setEndoMode(endoMode);
        setColorMode(prevColorMode, prevIntensity);
      }

      break;
    case CMD_DEPTH:
      //            depthMode = cmd.value;
      if (depthMode == 0) {
        // Restore previous conditions when depth mode is turned off
        if (endoMode == 1) {
          setEndoMode(1);
          setColorModeToEndo(prevColorMode, prevIntensity);
        } else {
          setColorMode(prevColorMode, prevIntensity);
        }
      } else if (depthMode == 1) {
        setDepthMode(depthMode);
      }
      break;

    case CMD_FIELD:
      if (fieldmode) {
        if (cmd.value == small) {
          setFieldIntensity(small);
          prvFocusMode = small;
        } else if (cmd.value == medium) {
          setFieldIntensity(medium);
          prvFocusMode = medium;
        } else if (cmd.value == large) {
          setFieldIntensity(large);
          prvFocusMode = large;
        }
      } else {
        fieldmode = 0;
        if (endoMode == 1) {
          // Directly set color mode to Endo with intensity
          setColorModeToEndo(prevColorMode, prevIntensity);
        } else {
          setColorMode(prevColorMode, prevIntensity);
        }
      }
      break;



    case CMD_RELAY:
      if (cmd.subType == 'M') {  // Turn ON the relay
        switch (cmd.value) {
          case 1:                                                             // Relay 1
            HAL_GPIO_WritePin(relay1_GPIO_Port, relay1_Pin, GPIO_PIN_RESET);  // Turn on relay1
            HAL_Delay(100);
            HAL_GPIO_WritePin(relay1_GPIO_Port, relay1_Pin, GPIO_PIN_SET);  // Turn off relay1
            break;
          case 2:                                                             // Relay 2
            HAL_GPIO_WritePin(relay2_GPIO_Port, relay2_Pin, GPIO_PIN_RESET);  // Turn on relay2
            HAL_Delay(100);
            HAL_GPIO_WritePin(relay2_GPIO_Port, relay2_Pin, GPIO_PIN_SET);  // Turn off relay2
            break;
          case 3:                                                             // Relay 3
            HAL_GPIO_WritePin(relay3_GPIO_Port, relay3_Pin, GPIO_PIN_RESET);  // Turn on relay3
            HAL_Delay(100);
            HAL_GPIO_WritePin(relay3_GPIO_Port, relay3_Pin, GPIO_PIN_SET);  // Turn off relay3
            break;
        }
      } else {  // Turn OFF all relays
        HAL_GPIO_WritePin(GPIOA, relay1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, relay2_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, relay3_Pin, GPIO_PIN_SET);
      }
      break;
    default:
      break;
  }
}
