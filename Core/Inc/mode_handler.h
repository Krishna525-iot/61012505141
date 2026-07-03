/* ============================= */
/*       mode_handler.h          */
/* ============================= */
#ifndef MODE_HANDLER_H
#define MODE_HANDLER_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>


extern uint8_t lampState;
extern uint8_t endoMode;
extern uint8_t prevEndoMode;
extern uint8_t prevIntensity;
extern uint8_t prvFocusMode;
extern uint8_t fieldmode;


typedef enum {
	small =1,
	medium,
	large
} FieldType;


typedef enum {
    CMD_UNKNOWN,
    CMD_INTENSITY,
    CMD_COLOR,
    CMD_LAMP,
    CMD_ENDO,
	CMD_FIELD,
    CMD_DEPTH,
	CMD_RELAY,

} CommandType;

typedef struct {
    CommandType type;
    char subType;
    int value;
} Command;

Command parseCommand(const char *cmdString);
void handleCommand(const Command cmd);

#endif
