//
// Created by Kevin Funderburg on 12/10/20.
//

#ifndef STATION_H
#define STATION_H

#include "tcp.h"

enum instrType {SEND_INST, WAIT_INSTR};

void getInput();

int sendRequest();

char* parseLine(char* line);

void runner(FILE *fp);

int main(int argc, char **argv);



#endif //STATION_H
