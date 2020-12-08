//
// Created by Kevin Funderburg on 12/8/20.
//
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include "csp.h"
#include "sp.h"
using namespace std;



/**
 * parse arguments for execution
 * @param argc
 * @param argv
 */
void parseArgs(int argc, char *argv[])
{
    if (argc < 3) {
        show_usage();
        return;
    }
    if (*argv[2] == 3 && argc < 5) {
        printf("Expected 4 arguments and got %d\n", argc);
        show_usage();
        return;
    }

}


static void show_usage()
{
    printf("Usage: sim [123] [average arrival rate] [average service time] [quantum interval]\n\n"
           "Options:\n"
           "\t1 : First-Come First-Served (_FCFS)\n"
           "\t2 : Shortest Remaining Time First (_SRTF)\n"
           "\t3 : Round Robin, with different quantum values (_RR) (requires 4 arguments)\n");
}


int main(int argc, char *argv[] )
{
    parseArgs(argc, argv);
    generate_report();
    return 0;
}