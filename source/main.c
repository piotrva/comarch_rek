#include<stdio.h>
#include<sys/types.h>
#include<signal.h>
#include<unistd.h>

#include "parser.h"

char buffer[2048];

void printLog(char *str, bool p){
    FILE *fp;
    fp = fopen("log.txt", "a");
    fprintf(fp, str);
    fprintf(fp, "\n");
    if(p){
        printf(str);
        printf("\n");
    }
    fclose(fp);
}

void AT_UDCONF_50(char *arg){
    static int sim_hot_insertion = 0;
    int pos = 0;
    if(strlen(arg) > 0){  // set command
        sprintf(buffer, "AT+UDCONF=50,%s", arg);
        printLog(buffer, false);
        printLog("OK", true);
        sim_hot_insertion = parseNumber(arg, &pos);
    }else{  // get command
        sprintf(buffer, "AT+UDCONF=50");
        printLog(buffer, false);
        sprintf(buffer, "+UDCONF=50,%d", sim_hot_insertion);
        printLog(buffer, true);
        printLog("OK", true);
    }
}

ATCommand AT_UDCONF_50_item = {
    .function = AT_UDCONF_50,
    .name = "AT+UDCONF=50"
};

void AT_KILL(char *arg){
    printf("OK\n");
    exit(0);
}

ATCommand AT_KILL_item = {
    .function = AT_KILL,
    .name = "AT+KILL"
};

int main(){
    printf("WELCOME\n");
    addATCommand(&AT_UDCONF_50_item);
    addATCommand(&AT_KILL_item);
    while(1){
        ATCommandProcess();
    }
}
