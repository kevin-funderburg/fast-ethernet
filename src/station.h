//
// Created by Kevin Funderburg on 12/10/20.
//

#ifndef STATION_H
#define STATION_H

#include "tcp.h"

enum instrType {SEND_INST, WAIT_INSTR};

char* getInput();

char* parseLine(char* line);

void str_cli(FILE *fp, int sockfd);

int main(int argc, char **argv);



#endif //STATION_H
