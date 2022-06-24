#include<stdio.h>
#include<sys/types.h>
#include<signal.h>
#include<unistd.h>
#include<stdlib.h>
#include<time.h>

#include "parser.h"

// general purpose text buffer - size not optimized
char buffer[2048];

void printLog(char *str, bool p){
    FILE *fp;
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    fp = fopen("log.txt", "a");
    fprintf(fp, "%04d-%02d-%02d %02d:%02d:%02d\t", timeinfo->tm_year + 1900,  timeinfo->tm_mon + 1, timeinfo->tm_mday,
        timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
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
    int pos = 1;  // ommit comma
    sprintf(buffer, "AT+UDCONF=50%s", arg);
    printLog(buffer, false);

    if(strlen(arg) > 0){  // set command
        printLog("OK", true);
        sim_hot_insertion = parseNumber(arg, &pos);
    }else{  // get command
        sprintf(buffer, "+UDCONF=50,%d", sim_hot_insertion);
        printLog(buffer, true);
        printLog("OK", true);
    }
}

ATCommand AT_UDCONF_50_item = {
    .function = AT_UDCONF_50,
    .name = "AT+UDCONF=50"
};

// this command is to set mode: dual or tri - as needed by AT+URAT
volatile int urat_mode = 2;
void AT_URM(char *arg){
    int pos = 1;  // ommit comma
    sprintf(buffer, "AT+URM%s", arg);
    printLog(buffer, false);

    if(strlen(arg) > 0){  // set command
        printLog("OK", true);
        urat_mode = parseNumber(arg, &pos);
    }else{  // get command
        sprintf(buffer, "+URM: %d", urat_mode);
        printLog(buffer, true);
        printLog("OK", true);
    }
}

ATCommand AT_URM_item = {
    .function = AT_URM,
    .name = "AT+URM"
};

// command to finish program on a computer - for a normal embedded application it will run forever
void AT_KILL(char *arg){
    printf("OK\n");
    exit(0);
}

ATCommand AT_KILL_item = {
    .function = AT_KILL,
    .name = "AT+KILL"
};

void AT_UREG(char *arg){
    static int n = 0;
    sprintf(buffer, "AT+UREG%s", arg);
    printLog(buffer, false);
    if(arg[0] == '?'){  // get command - state is random
        sprintf(buffer, "+UREG: %d,%d", n, rand()%10);
        printLog(buffer, true);
        printLog("OK", true);
    }else if(arg[0] == '='){
        if(arg[1] == '?'){  // test command
            printLog("+UREG: (0-1)", true);
            printLog("OK", true);
        }else{  // set command
            int pos = 1;  // start getting number after '='
            n = parseNumber(arg, &pos);
            printLog("OK", true);
        }
    }
}

ATCommand AT_UERG_item = {
    .function = AT_UREG,
    .name = "AT+UREG"
};

void AT_URAT(char *arg){
    static int selectedAct = 0;
    static int prefferedAct = 0;
    static int prefferedAct2 = 0;
    sprintf(buffer, "AT+URAT%s", arg);
    printLog(buffer, false);
    if(arg[0] == '?'){  // get command - state is random
        sprintf(buffer, "+URAT: %d", selectedAct);
        if(urat_mode > 0){
            sprintf(buffer, "%s,%d", buffer, prefferedAct);
        }
        if(urat_mode > 1){
            sprintf(buffer, "%s,%d", buffer, prefferedAct2);
        }
        printLog(buffer, true);
        printLog("OK", true);
    }else if(arg[0] == '='){
        if(arg[1] == '?'){  // test command
            printLog("+URAT: (0-6),(0,2,3),(0,2,3)", true);  // this is to represent physical capabilities of the hardware... so no info on that
            printLog("OK", true);
        }else{  // set command
            int pos = 1;  // start getting number after '='
            selectedAct = parseNumber(arg, &pos);
            if(arg[pos] != 0){
                prefferedAct = parseNumber(arg, &pos);
            }
            if(arg[pos] != 0){
                prefferedAct2 = parseNumber(arg, &pos);
            }
            printLog("OK", true);
        }
    }
}

ATCommand AT_URAT_item = {
    .function = AT_URAT,
    .name = "AT+URAT"
};

int main(){
    srand(time(NULL)); 
    printf("WELCOME\n");
    addATCommand(&AT_UDCONF_50_item);
    addATCommand(&AT_KILL_item);
    addATCommand(&AT_UERG_item);
    addATCommand(&AT_URAT_item);
    addATCommand(&AT_URM_item);
    while(1){
        ATCommandProcess();
    }
}
