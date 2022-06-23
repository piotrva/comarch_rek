// na podstawie opracowań z książki własnego autorstwa pt. "Systemy Embedded w FPGA", wydawnictwo BTC, Legionowo 2019
#include "parser.h"
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <stdlib.h>

static ATCommand *head = NULL;

static char buffer[2048];

static ATCommand* getByName(char *command);

static bool storeChar(void);

int parseNumber(char* str, int* pos){
	int number = atoi(str+(*pos));

	while(*(str+(*pos)) != ',' && *(str+(*pos)) != '\0') (*pos)++;

	if(*(str+(*pos)) == ',') (*pos)++;

	return number;
}

bool addATCommand(ATCommand *command){
    ATCommand *cmd = head;
    // block adding imcomplete commands, as they might corrupt operation of the module
    if(command == NULL) return false;
    if(command->function == NULL) return false;
    if(command->name == NULL) return false;
    
    if(head == NULL){
        head = command;
        head->next = NULL;
        return true;
    }

    while(cmd->next != NULL){
        cmd = cmd->next;
    }
    cmd->next = command;
    command->next = NULL;

    return true;
}

void ATCommandProcess(void){
    if(storeChar()){
        ATCommand* command = getByName(buffer);
        if(command != NULL){
            command->function(&buffer[strlen(command->name)+1]);
        }
        memset(buffer, 0, sizeof(buffer)/sizeof(buffer[0]));
    }
}

bool storeChar(void){
    static int pos = 0;
    char c;
    if(pos >= sizeof(buffer)/sizeof(buffer[0]) - 1){
        pos = 0;
        memset(buffer, 0, sizeof(buffer)/sizeof(buffer[0]));
        printf("AT+OVERFLOW\n");
    }
    if(_kbhit()){
        c = _getch();
        printf("%c", c);  // this provides echo
        if(c == '\n' || c == '\r'){
            pos = 0;
            printf("\n\n");
            return true;
        }else{
            buffer[pos] = c;
            pos++;
        }
    }
    return false;
}

ATCommand* getByName(char *command){
    volatile ATCommand *actual = head;
    while(actual != NULL){
        if(memcmp(actual->name, command, strlen(actual->name)) == 0){
            // there is an assumption that there is no command which is a prefix of another command
            // ex. AT+CM and AT+CMD
            // otherwise additional steps need to be done here
            return (ATCommand*)actual;
        }
        actual = actual->next;
    }
    return NULL;
}