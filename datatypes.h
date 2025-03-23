//--------------OpCodeAraryCapacity through OpCodeType weer authored by Michael Leverington as a utility for the program---------------
//--------------PCB through interruptInfo were written by Payton Watts--------------------------------------

// Preprocessor directives
#ifndef DATATYPES_H
#define DATATYPES_H

#include <stdio.h>
#include "StandardConstants.h"

// GLOBAL CONSTANTS - may be used in other files

typedef enum { CMD_STR_LEN = 5,
               IO_ARG_STR_LEN = 5,
               STR_ARG_LEN = 15 } OpCodeArrayCapacity;

typedef enum { NEW_STATE,
               READY_STATE,
               RUNNING_STATE,
               BLOCKED_STATE,
               EXIT_STATE } ProcessState;

// codes used for memory actions and results
typedef enum { MEM_INIT,
               MEM_ALLOCATE,
               MEM_ACCESS,
               MEM_CLEAR,
               FINAL_CLEAR,
               SEG_FAULT_ALLOC,
               SEG_FAULT_ACCESS,
               MEM_SUCCESS_INIT,
               MEM_SUCCESS_ALLOC,
               MEM_SUCCESS_ACCESS,
               MEM_SUCCESS_CLEAR,
               FINAL_CLEAR_SUCCESS } MemCodes;

typedef struct ConfigDataType
   {
    double version;
    char metaDataFileName[ LARGE_STR_LEN ]; 
    int cpuSchedCode;
    int quantumCycles;
    bool memDisplay;
    int memAvailable;
    int procCycleRate;
    int ioCycleRate;
    int logToCode;
    char logToFileName[ LARGE_STR_LEN ];
   } ConfigDataType;

typedef struct OpCodeType
   {
    int pid;                          // pid, added when PCB is created
    char command[ CMD_STR_LEN ];      // three letter command quantity
    char inOutArg[ IO_ARG_STR_LEN ];  // for device in/out
    char strArg1[ STR_ARG_LEN ];      // arg 1 descriptor, up to 12 chars
    int intArg2;                      // cycles or memory, assumes 4 byte int
    int intArg3;                      // memory, assumes 4 byte int
                                      //   also non/premption indicator
    double opEndTime;                 // size of time string returned from accessTimer
    struct OpCodeType *nextNode;      // pointer to next node as needed
   } OpCodeType;

// PCB struct
typedef struct PCB
    {
        int pid;
        int time;
        int numInstrs;
        char *processState;
        int processPriority;
        // instruction stack
        struct OpCodeType *stack;
        // used to hold previous instruction for future reference
        struct OpCodeType *prevInstr;
        // head ptr of stack
        struct OpCodeType *headPtr;
        struct PCB *next;
    } PCB;


// define the queue struct
typedef struct Queue
    {
        PCB *front, *rear;
        int numElements;
    } Queue;

// define a memory block struct
typedef struct memBlock
    {
        int base;
        int limit;
        int range;
        int dirtyBit;
    } memBlock;

// struct used to pass interrupt info into execProcesses and check interrupt status in runSim
typedef struct interruptInfo
    {
        struct Queue *waitQueue;
        struct PCB *current;
        int procTime;
        int currentPid;
        bool interrupted;
    } interruptInfo;

#endif // DATATYPES