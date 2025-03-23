// Preprocessor directives
#ifndef SIMULATOR_H
#define SIMULATOR_H

// header files
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdarg.h>
#include "configops.h"
#include "simtimer.h"
#include "datatypes.h"
#include "StandardConstants.h"
#include "StringUtils.h"

// data types
// see datatypes.h

// function prototypes

void runSim( ConfigDataType *configPtr, OpCodeType *metaDataMstrPtr );

// function to count number of instructions(used to initialize stack)
int peekInstrs( OpCodeType *startingNode );

// I/O thread handler function
void *ioHandler(void *arg);

// I/O thread handler function for preemption
void *ioHandlerPreempted(void *arg);

// PCB creation function
Queue *createPCBs( ConfigDataType *masterCFGPtr, OpCodeType *masterMDPtr );

// PCB initializer function
PCB *createPCB( int inputPid, char *state, int priority, int numInstructions );

// create a new queue
Queue *makeQueue();

// enqueue(add element to queue) function
void enqueue( Queue *inputQueue, PCB *inputPCB );

PCB *deepCopyPCB(PCB *inputPCB);

// dequeue(remove element from queue) function
PCB *dequeue( Queue *inputQueue );

// free PCB function
void freePCB( PCB *inputPCB );

// is queue empty?
bool isEmpty( Queue *queue );

// copy LL node function
OpCodeType *copyNode( OpCodeType *inputNode );

// log to file/monitor/both function
void logger( FILE *fileName, bool toFile, bool toMonitor, const char *inputStr, ... );

void executeProcesses( ConfigDataType *configPtr, FILE *outFile, PCB *currentPCB, Queue *PCBQueue, Queue *waitQueue,
                     bool fileFlag, bool printFlag, bool memFlag, bool preempFlag, interruptInfo *interruptStatus, char *timeStr,
                      char *inOut, int CPUTime, int IOTime, int *memoryArr, int totalMemSize );

void sortQueueSJF( Queue *inputQueue );

// function for sorting the wait queue based on the pids of processes waiting for service
void sortWaitQueue( interruptInfo *handler, Queue *inputQueue );

// function for updating the interrupt handler
void updateInterruptHandler( interruptInfo *inputHandler );

void swapPCBs( PCB *a, PCB *b );

int MMU( int *inputArr, MemCodes action, int base, int limit, int total, bool memFlag, bool fileFlag, bool printFlag, int pid, FILE *outFile );

#endif