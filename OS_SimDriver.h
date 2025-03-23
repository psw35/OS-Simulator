//--------------The below code was authored by Michael Leverington as a utility for the program---------------
//--------------To see the code written by Payton Watts, see simulator.c--------------------------------------

// protect from multiple compiling
#ifndef OS_SIM_DRIVER_H
#define OS_SIM_DRIVER_H

// header files
#include <stdio.h>
#include <stdlib.h>
#include "datatypes.h"
#include "StringUtils.h"
#include "configops.h"
#include "metadataops.h"
#include "simulator.h"

// Program constants
typedef enum { MIN_NUM_ARGS = 3, LAST_FOUR_LETTERS = 4 } PRGRM_CONSTANTS;

// Command  line struct for storing command line switch settings
typedef struct CmdLineDataStruct
    {
        bool programRunFlag;
        bool configDisplayFlag;
        bool mdDisplayFlag;
        bool runSimFlag;

        char fileName[ STD_STR_LEN ];
    } CmdLineData;

// Function Prototypes

bool processCmdLine( int numArgs, char **strVector, CmdLineData *clDataPtr );

void showCommandLineFormat();

void clearCmdLineStruct( CmdLineData *clDataPtr );

#endif