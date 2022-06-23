// na podstawie opracowań z książki własnego autorstwa pt. "Systemy Embedded w FPGA", wydawnictwo BTC, Legionowo 2019
#ifndef PARSER_H_
#define PARSER_H_

#include <stdbool.h>

typedef void(*callbackFunction)(char *args);

typedef struct ATCommand {
    callbackFunction function;
    char *name;
    struct ATCommand* next;
} ATCommand;

bool addATCommand(ATCommand *command);

void ATCommandProcess(void);

int parseNumber(char *str, int *pos);

#endif  // PARSER_H_
