//
// Created by Kevin Funderburg on 12/8/20.
//

#ifndef CSP_H
#define CSP_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <syslog.h>

#include "frame.h"

static void err_doit(int, int, const char *, va_list);

int getDescriptor(int clientIndex);

int sendToSP(int num, char* msg);

int reply(char* msg);

int processFrame(char* frameText);










#endif //CSP_H
