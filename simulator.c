// header files
#include "simulator.h"
#include <unistd.h>
/*
Name: runSim
Process: primary simulation driver
Function Input/Parameters: config data (ConfigDataType *),
                            metadata (OpCodeType *)
Function Output/Parameters: none
Function Output/Returned: none
Device Input/device: none
Device Output/monitor: none
Dependencies: tbd
*/
void runSim( ConfigDataType *configPtr, OpCodeType *metaDataMstrPtr )
    {
        // printing flags
        bool printFlag = false;
        bool fileFlag = false;

        // preemption variables
        bool preempFlag = false;

        // index variable
        int index = 0;

        // interrupt handler
        interruptInfo *interruptStatus = (interruptInfo *)malloc(sizeof(interruptInfo));

        // initialize other variables of interruptStatus
        interruptStatus->procTime = 0;
        interruptStatus->interrupted = false;
        
        // check scheduling code to determine if preempting or not:
        if( (configPtr->cpuSchedCode == CPU_SCHED_FCFS_P_CODE) || (configPtr->cpuSchedCode == CPU_SCHED_SRTF_P_CODE)
         || (configPtr->cpuSchedCode == CPU_SCHED_RR_P_CODE) )
            {
                // if preempting, set preemption flag to true
                preempFlag = true;
                // initialize the wait queue
                interruptStatus->waitQueue = makeQueue();
                // initialize the temporary PCB holder
                interruptStatus->current = (PCB *)malloc(sizeof(PCB));
            }
        
        // create outpute file
        FILE *outFile;

        // determine what we're outputting to
        // if both
        if( configPtr->logToCode == LOGTO_BOTH_CODE )
            {
                // set print to file and printing to monitor true
                fileFlag = true;
                printFlag = true;
                // open outfile as designated name
                outFile = fopen( configPtr->logToFileName, "w" );
            }
        // else if just file
        else if( configPtr->logToCode == LOGTO_FILE_CODE )
            {
                // set only file print true
                fileFlag = true;
                // open outfile as designated name
                outFile = fopen( configPtr->logToFileName, "w" );
            }
        // else just print to monitor
        else
            {
                // set print to monitor flag to true
                printFlag = true;
            }
        
        // set temp OpCodeType node
        OpCodeType *curNode = metaDataMstrPtr;
        // initialize pthread
        //pthread_t *ioThread = (pthread_t *)malloc(sizeof(pthread_t));
        // get CPU and IO times
        int CPUTime = configPtr->procCycleRate;
        int IOTime = configPtr->ioCycleRate;
        // initialize timer string and string for input/output instruction use
        char timeStr[MAX_STR_LEN];
        char inOut[STD_STR_LEN];

        // get memory info
        // total range of available memory = amount designated by config file, minus 1
        // this makes up for zero-based indexing
        int totalSize = configPtr->memAvailable;
        // check if displaying memory or not
        bool memFlag = configPtr->memDisplay;
        // array of our registers we can use
        int memArray[totalSize];

        // print simulator run title
        logger( outFile, fileFlag, printFlag, "Simulator Run\n" );
        logger( outFile, fileFlag, printFlag, "-------------\n\n" );

        // start sim timer
        accessTimer( ZERO_TIMER, timeStr );

        // print time 0 and sim start
        logger( outFile, fileFlag, printFlag, " %s, OS: Simulator Start\n", timeStr );
            
        // create PCB queue
        // all processes starting in state NEW
        Queue *PCBQueue = createPCBs( configPtr, curNode->nextNode );
        
        // initialize a temp PCB
        PCB *currentPCB = NULL;
        // get the number of PCBs in the element
        int numPCBS = PCBQueue->numElements;
        
        // for each PCB that exists in the queue, set from NEW to READY
        // lap time
        accessTimer( LAP_TIMER, timeStr );
        for( index = 0; index < numPCBS; index++ )
            {
                // dequue a PCB, set it's process state to READY, enqueue it again, then print message
                currentPCB = dequeue( PCBQueue );
                currentPCB->processState = "READY";
                enqueue( PCBQueue, currentPCB );
                logger( outFile, fileFlag, printFlag, " %s, OS: Process %d set to READY state from NEW state\n", timeStr, currentPCB->pid );
                free(currentPCB); 
            }

        // initialize full memory
        MMU( memArray, MEM_INIT, 0, totalSize, totalSize, memFlag, fileFlag, printFlag, 0, outFile );
        
        // if scheduling code, SJF, sort queue by time
        if( configPtr->cpuSchedCode == CPU_SCHED_SJF_N_CODE || configPtr->cpuSchedCode == CPU_SCHED_SRTF_P_CODE )
            {
                // call sorting function
                sortQueueSJF( PCBQueue );
            }
        // loop through PCB queue and run each PCB's tasks/instructions
        // master loop
        // keep running it while processes still present in main queue, or wait queue too if preempting
        while( !isEmpty( PCBQueue ) || (preempFlag && !isEmpty( interruptStatus->waitQueue ) ) )
            {
                // if no interrupts
                if( !interruptStatus->interrupted )
                    {
                        // execute processes as normal
                        executeProcesses( configPtr ,outFile, currentPCB, PCBQueue, interruptStatus->waitQueue, fileFlag, printFlag, memFlag,
                        preempFlag, interruptStatus, timeStr, inOut, CPUTime, IOTime, memArray, totalSize );
                    }
                // else there are preempted processes we need to return to
                else
                    {
                        // if SRTF, sort the wait queue by shortest process runtime first
                        if( configPtr->cpuSchedCode == CPU_SCHED_SRTF_P_CODE )
                            {
                                sortQueueSJF( interruptStatus->waitQueue );
                            }
                        // run a process from wait queue
                        executeProcesses( configPtr, outFile, currentPCB, interruptStatus->waitQueue, interruptStatus->waitQueue, fileFlag, printFlag, memFlag,
                        preempFlag, interruptStatus, timeStr, inOut, CPUTime, IOTime, memArray, totalSize );
                    }
            }
        // end master loop
        // lap time
        accessTimer( LAP_TIMER, timeStr );
        // print system stop
        logger( outFile, fileFlag, printFlag, " %s, OS: System stop\n", timeStr );
        // free queue
        free( PCBQueue );

        // final memory free
        MMU( memArray, FINAL_CLEAR, 0, totalSize, totalSize, memFlag, fileFlag, printFlag, 0, outFile );
        // lap time
        accessTimer( LAP_TIMER, timeStr );
        // print simulation end
        logger( outFile, fileFlag, printFlag, " %s, OS: Simulation end\n", timeStr );
    }

// I/O thread handler function
// takes in parameter from pthread_create call
void *ioHandler(void *arg) 
    {
        // recast as long(int causes issues), then runTimer
        runTimer((long)arg);

        // return NULL
        return NULL;
    }

// I/O thread handler function for preempted
// takes in parameter from pthread_create call containing interrupt instructions
void *ioHandlerPreempted(void *arg)
    {
        interruptInfo *inputArgs = (interruptInfo *)arg;
        // save pid for this process
        int time = inputArgs->procTime;
        PCB *currentPCB = deepCopyPCB( inputArgs->current );
        // run timer for request amount of time
        runTimer(time);
        // time to do an interrupt, turn on interrupt signal and notify which process this is
        inputArgs->interrupted = true;
        // enqueue into wait list
        enqueue( inputArgs->waitQueue, currentPCB);
        // return NULL
        pthread_exit(NULL);
    }

// PCB creation function
Queue *createPCBs( ConfigDataType *masterCFGPtr, OpCodeType *masterMDPtr )
    {
        // initialize variables
        int pid = 0;
        int timeRemaining = 0;
        PCB *currentPCB = NULL;
        Queue *PCBQueue = makeQueue();
        OpCodeType *currentNode = masterMDPtr;

        // loop through metadata file
        while( !( (compareString( currentNode->strArg1, "end" ) == STR_EQ ) && ( compareString( currentNode->command, "sys" ) == STR_EQ )) )
            {
                // if current opcode "app start, 0", create new PCB
                if( ( compareString( currentNode->command, "app") == STR_EQ )
                 && ( compareString( currentNode->strArg1, "start" ) == STR_EQ ) )
                {
                    // allocate memory, initialize PCB
                    currentPCB = createPCB( pid, "NEW", 0, peekInstrs( currentNode ) );
                    currentPCB->headPtr = copyNode( currentNode->nextNode );
                    currentPCB->stack = currentPCB->headPtr;
                }
                // else if "app end", time for new PCB is coming, increment pid
                else if( ( compareString( currentNode->command, "app") == STR_EQ )
                 && ( compareString( currentNode->strArg1, "end" ) == STR_EQ ) )
                {
                    // increment pid
                    pid++;
                    // set finished PCB's time
                    currentPCB->time = timeRemaining;
                    // reset time
                    timeRemaining = 0;
                    // for whatever reason, the head ptr likes be before where it should be, so we fix it
                    currentPCB->headPtr = currentPCB->headPtr->nextNode;
                    // enqueue finished pcb
                    enqueue( PCBQueue, currentPCB );
                    freePCB(currentPCB);
                    // prevent hanging ptr
                    currentPCB = NULL;
                }
                // else is normal instruction, add to current PCB's stack
                else
                    {
                        // set PCB's stack at current element to current metadata node
                        currentPCB->stack->nextNode = copyNode( currentNode );
                        currentPCB->stack = currentPCB->stack->nextNode;
                        
                        // add to total runtime
                        // if not CPU or mem, it's an I/O instruction, calculate with I/O cycle time
                        if( ( compareString( currentNode->command, "cpu" ) != STR_EQ ) && ( compareString( currentNode->command, "mem" ) != STR_EQ ) )
                            {
                                timeRemaining = timeRemaining + ( currentNode->intArg2 * masterCFGPtr->ioCycleRate );
                                
                            }
                        // else calculate with CPU cycle time
                        else if( compareString( currentNode->command, "cpu" ) == STR_EQ )
                            {
                                timeRemaining = timeRemaining + (  currentNode->intArg2 * masterCFGPtr->procCycleRate );
                            }
                        // ignore mem here, no instruction times given
            
                    }
                // go to next instruction
                currentNode = currentNode->nextNode;
            }
        // return PCB queue
        return PCBQueue;
    }

// function to count number of instructions(used to initialize stack)
int peekInstrs( OpCodeType *startingNode )
    {
        // initialize variables
        OpCodeType *curNode = startingNode->nextNode;
        int numInstructions = 0;
        // assuming function called on process start(app start, 0)
        // loop and count until we hit process end(app end)
        while( !( (compareString( curNode->nextNode->strArg1, "end" ) == STR_EQ ) && ( compareString( curNode->nextNode->command, "app" ) == STR_EQ )) )
            {
                numInstructions++;
                curNode = curNode->nextNode;
            }
        // return number of instructions counted
        return numInstructions;
    }

// PCB initializer function
PCB *createPCB( int inputPid, char *state, int priority, int numInstructions )
    {
        // allocate memory
        PCB *newPCB = ( PCB *) malloc( sizeof( PCB ) );
        // set pid
        newPCB->pid = inputPid;
        // set total runtime
        newPCB->time = 0;
        // set number of instructions
        newPCB->numInstrs = numInstructions;
        // set process state
        newPCB->processState = state;
        // set process priority
        newPCB->processPriority = priority;
        // set process stack
        newPCB->stack = NULL;
        // set headptr of stack
        newPCB->headPtr = NULL;
        // set next LL node to NULL
        newPCB->next = NULL;
        // set prev instruction to NULL
        newPCB->prevInstr = NULL;
        // return created PCB
        return newPCB;
    }

// create a new queue
Queue *makeQueue()
    {
        // allocate mem
        Queue *newQueue = ( Queue *) malloc( sizeof( Queue ) );
        // set front index to reat index, and both to NULL
        newQueue->front = NULL;
        newQueue->rear = NULL;
        newQueue->numElements = 0;
        return newQueue;
    }

// enqueue(add element to queue) function
void enqueue( Queue *inputQueue, PCB *inputPCB )
    {
        // save a new PCB to prevent memory issues
        PCB *newPCB = deepCopyPCB( inputPCB );
        // if queue empty, new node is front and rear
        if( inputQueue->rear == NULL )
            {
                inputQueue->front = inputQueue->rear = newPCB;
            }
        // else just add to end of the queue
        else
            {
                inputQueue->rear->next = newPCB;
                inputQueue->rear = newPCB;
            }
        inputQueue->numElements++;
    }

PCB *deepCopyPCB(PCB *inputPCB)
    {
        // create a new pcb for results
        PCB *newPCB = (PCB *)malloc(sizeof(PCB));
        
        // copy over the non dynamically allocated variabkes
        newPCB->pid = inputPCB->pid;
        newPCB->time = inputPCB->time;
        newPCB->numInstrs = inputPCB->numInstrs;
        newPCB->processPriority = inputPCB->processPriority;
        newPCB->processState = inputPCB->processState;
        newPCB->next = NULL;
        newPCB->prevInstr = NULL;

        newPCB->stack = NULL;
        newPCB->headPtr = NULL;
        
        // copy over head ptr
        newPCB->headPtr = (OpCodeType *)malloc(sizeof(OpCodeType));
        *(newPCB->headPtr) = *(inputPCB->headPtr);
        newPCB->headPtr->nextNode = NULL;

        // copy over previous instruction if present
        if( inputPCB->prevInstr != NULL )
            {
                newPCB->prevInstr = (OpCodeType *)malloc(sizeof(OpCodeType));
                *(newPCB->prevInstr) = *(inputPCB->prevInstr);
                newPCB->prevInstr->nextNode = NULL;
            }

        // temp variables
        OpCodeType *currentInputNode = inputPCB->headPtr->nextNode;
        OpCodeType *currentNewNode = newPCB->headPtr;
        
        // while current node not null
        while( currentInputNode != NULL )
            {
                // copy over the current node to newNode
                OpCodeType *newNode = (OpCodeType *)malloc(sizeof(OpCodeType));
                *newNode = *currentInputNode;
                newNode->nextNode = NULL;

                currentNewNode->nextNode = newNode;

                // increment nodes
                currentInputNode = currentInputNode->nextNode;
                currentNewNode = newNode;
            }
        
        // set stack to head ptr
        newPCB->stack = newPCB->headPtr;
        
        // return new pcb we made
        return newPCB;
    }

// dequeue(remove element from queue) function
PCB *dequeue( Queue *inputQueue )
    {
        // if queue empty return error
        if( inputQueue->front == NULL )
            {
                printf( "Error! Queue is empty.\n" );
                return NULL;
            }
        // else carry on as normal

        // save front node
        PCB *temp = inputQueue->front;
        inputQueue->front = inputQueue->front->next;
        inputQueue->numElements--;

        // if this empties queue(front is now NULL), set rear to NULL
        if( inputQueue->front == NULL )
            {
                inputQueue->rear = NULL;
            }
        
        // return temp
        return temp;
    }

// free PCB function
void freePCB( PCB *inputPCB )
    {
        // initialize variables
        OpCodeType *currentStackElem = inputPCB->headPtr;
        OpCodeType *nextStackElem;

        // free stack elements
        while( currentStackElem != NULL )
            {
                nextStackElem = currentStackElem->nextNode;

                free( currentStackElem );

                currentStackElem = nextStackElem;
            }
        
        // free PCB itself
        free( inputPCB );
    }

// is queue empty?
bool isEmpty( Queue *queue )
    {
        // check for front being NULL
        return queue->front == NULL;
    }

// copy LL node function
OpCodeType *copyNode( OpCodeType *inputNode )
    {
        // initialize new node
        OpCodeType *newNode = ( OpCodeType *) malloc( sizeof( OpCodeType ) );

        // set new node to input node and new node's next ptr to NULL
        *newNode = *inputNode;
        newNode->nextNode = NULL;

        // return new node
        return newNode;
    }

// logger function able to take in input string to print to file/monitor/both
// able to take unlimited arguments thanks to stdarg.h, this allows
// use of any variables we need to print without having to know exactly
// what they are
void logger( FILE *fileName, bool toFile, bool toMonitor, const char *inputStr, ... )
    {
        // arguments for the string
        va_list args;

        // if printing to monitor(Monitor or Both)
        if( toMonitor )
            {
                // initialize argument list to be anything after inputStr parameter
                va_start( args, inputStr );
                // print formatted str to monitor
                vprintf( inputStr, args );
                // clean up argument list
                va_end( args );
            }

        // if printing to file(Both or File)
        if( toFile )
            {
                // initialize argument list to be anything after inputStr parameter
                va_start( args, inputStr );
                // print formatted str to file fileName
                vfprintf( fileName, inputStr, args );
                // ensure output is written
                fflush( fileName );
                // clean up argument list
                va_end( args );
            }

    }

// execute non-preemptive instructions function
void executeProcesses( ConfigDataType *configPtr, FILE *outFile, PCB *currentPCB, Queue *PCBQueue, Queue *waitQueue,
                     bool fileFlag, bool printFlag, bool memFlag, bool preempFlag, interruptInfo *interruptStatus, char *timeStr,
                      char *inOut, int CPUTime, int IOTime, int *memoryArr, int totalMemSize )
    {
        // initialize variables
        MemCodes memAction, memResult;
        bool segFault = false;
        OpCodeType *curNode = (OpCodeType *)malloc(sizeof(OpCodeType));
        // flag for stopping a process in its tracks
        bool interruptProcess = false;
        int index;
        pthread_t *ioThread = (pthread_t *)malloc(sizeof(pthread_t));

        // get the current pcb
        currentPCB = dequeue(PCBQueue);
        // reset seg fault flag
        segFault = false;
        
        // if process blocked, set it to ready
        accessTimer( LAP_TIMER, timeStr );
        if( compareString( currentPCB->processState, "BLOCKED") == STR_EQ )
            {
                currentPCB->processState = "READY";
                logger( outFile, fileFlag, printFlag, " %s, OS: Process %d set from BLOCKED to READY\n", timeStr, currentPCB->pid );
            }
        // announce process selected
        logger( outFile, fileFlag, printFlag, " %s, OS: Process %d selected with %d ms remaining\n", timeStr, currentPCB->pid, currentPCB->time );
        // set process from ready to running
        accessTimer( LAP_TIMER, timeStr );
        currentPCB->processState = "RUNNING";
        logger( outFile, fileFlag, printFlag, " %s, OS: Process %d set from READY to RUNNING\n\n", timeStr, currentPCB->pid );
        
        // set current node to head ptr
        curNode = currentPCB->headPtr;
        // loop through metadata instructions, end if seg fault occurs
        for( index = 0; index <= currentPCB->numInstrs; index++ )
            {
                if( !segFault && !interruptProcess )
                    {
                        // if cpu instruction
                        if( compareString( curNode->command, "cpu" ) == STR_EQ )
                            {
                                // print start of instruction
                                logger( outFile, fileFlag, printFlag, " %s, Process: %d, %s process operation start\n", timeStr, currentPCB->pid, curNode->command );
                                // wait appropriate time, lap timer
                                runTimer(curNode->intArg2 * CPUTime);
                                accessTimer( LAP_TIMER, timeStr );
                                // print process end
                                logger( outFile, fileFlag, printFlag, " %s, Process: %d, %s process operation end\n", timeStr, currentPCB->pid, curNode->command );
                                // if preempted, make sure the total runtime is updated for future reference:
                                if( preempFlag )
                                    {
                                        currentPCB->time = currentPCB->time - (curNode->intArg2 * CPUTime);
                                    }
                            }
                        // else if mem instruction
                        else if( compareString( curNode->command, "mem" ) == STR_EQ )
                            {
                                // if allocate request
                                if( compareString( curNode->strArg1, "allocate" ) == STR_EQ )
                                    {
                                        // set mem action, print message
                                        memAction = MEM_ALLOCATE;
                                        logger( outFile, fileFlag, printFlag, " %s, Process: %d, %s allocate request (%d, %d)\n", timeStr, currentPCB->pid, 
                                            curNode->command, curNode->intArg2, curNode->intArg3 );
                                    }
                                // else if access request
                                else if( compareString( curNode->strArg1, "access" ) == STR_EQ )
                                    {
                                        // set mem action to access, and print message
                                        memAction = MEM_ACCESS;
                                        logger( outFile, fileFlag, printFlag, " %s, Process: %d, %s access request (%d, %d)\n", timeStr, currentPCB->pid, 
                                            curNode->command, curNode->intArg2, curNode->intArg3 );
                                    }
                                // else is a clear request
                                else
                                    {
                                        // set action to clear
                                        memAction = MEM_CLEAR;
                                    }
                                // call MMU function to do requested mem action on registers specified in intArg2 and 3, while remembering max mem size
                                memResult = MMU( memoryArr, memAction, curNode->intArg2, curNode->intArg3, totalMemSize, memFlag, fileFlag, printFlag, currentPCB->pid, outFile );
                                
                                // if successful allocation
                                if( memResult == MEM_SUCCESS_ALLOC )
                                    {
                                        // lap timer, print successful allocation
                                        accessTimer( LAP_TIMER, timeStr );
                                        logger( outFile, fileFlag, printFlag, " %s, Process: %d, successful %s allocate request\n", timeStr, currentPCB->pid, 
                                            curNode->command, curNode->intArg2, curNode->intArg3 );
                                    }
                                // else if successful access
                                else if( memResult == MEM_SUCCESS_ACCESS )
                                    {
                                        // lap timer, print successful access
                                        accessTimer( LAP_TIMER, timeStr );
                                        logger( outFile, fileFlag, printFlag, " %s, Process: %d, succesful %s access request\n", timeStr, currentPCB->pid, 
                                            curNode->command, curNode->intArg2, curNode->intArg3 );
                                    }
                                // else if failure to allocate
                                else if( memResult == SEG_FAULT_ALLOC )
                                    {
                                        // lap timer, print allocation failure
                                        accessTimer( LAP_TIMER, timeStr );
                                        logger( outFile, fileFlag, printFlag, " %s, Process: %d, failed %s allocate request\n", timeStr, currentPCB->pid, 
                                            curNode->command, curNode->intArg2, curNode->intArg3 );
                                        // print segmentation fault error, set flag to true to halt process
                                        logger( outFile, fileFlag, printFlag, "\n %s, Segmentation fault, Process %d ended\n", timeStr, currentPCB->pid );
                                        segFault = true;
                                    }
                                // else failed access
                                else
                                    {
                                        // lap timer, print access failure
                                        accessTimer( LAP_TIMER, timeStr );
                                        logger( outFile, fileFlag, printFlag, " %s, Process: %d, failed %s access request\n", timeStr, currentPCB->pid, 
                                            curNode->command, curNode->intArg2, curNode->intArg3 );
                                        // print segmentation fault error, set flag to true to halt process
                                        logger( outFile, fileFlag, printFlag, "\n %s, Segmentation fault, Process %d ended\n", timeStr, currentPCB->pid );
                                        segFault = true;
                                    }
                            }
                        // else is I/O instruction
                        else
                            {
                                    // having issues with concatenate string function, so using this to detect and change in and out to
                                    // input or output respectively
                                    if( compareString( curNode->inOutArg, "in" ) == STR_EQ )
                                        {
                                            inOut = "input";
                                        }
                                    else
                                        {
                                            inOut = "output";
                                        }
                                    // print I/O operation start
                                    logger( outFile, fileFlag, printFlag, " %s, Process: %d, %s %s operation start\n", timeStr, currentPCB->pid, curNode->strArg1, inOut );
                                   
                                    // if not preempting, wait for I/O thread to terminate before resuming main processing
                                    if( !preempFlag )
                                        {
                                             // start thread, feed into it the I/O cycle time for this particular instruction
                                            // have to cast the passed in cycle time formula as a long int to avoid complications
                                            // with casting as (void *)
                                            pthread_create( ioThread, NULL, ioHandler, (void *)((long)(curNode->intArg2 * IOTime)) );
                                            // wait for thread termination
                                            pthread_join( *ioThread, NULL ); 
                                            // lap timer
                                            accessTimer( LAP_TIMER, timeStr );
                                            // print I/O operation end
                                            logger( outFile, fileFlag, printFlag, " %s, Process: %d, %s %s operation end\n", timeStr, currentPCB->pid, curNode->strArg1, inOut );
                                        }
                                    // else if preempted
                                    else
                                        {
                                            // make sure the total runtime is updated for future reference:
                                            currentPCB->time = currentPCB->time - (curNode->intArg2 * IOTime);
                                            // make sure we're not putting a finished process on the wait queue
                                            if( curNode->nextNode != NULL )
                                                {
                                                    // save current instruction for interrupt message
                                                    currentPCB->prevInstr = curNode;

                                                    // ensure that when process resumes, it starts where we left off
                                                    currentPCB->headPtr = curNode->nextNode;
                                                    
                                                    // change process state to BLOCKED
                                                    currentPCB->processState = "BLOCKED";
                                                    // lap time
                                                    accessTimer( LAP_TIMER, timeStr );
                                                    // print process sent to BLOCKED
                                                    logger( outFile, fileFlag, printFlag, " %s, OS: Process %d blocked for %s operation\n", timeStr, currentPCB->pid, inOut );
                                                    logger( outFile, fileFlag, printFlag, " %s, OS: Process %d set from RUNNING to BLOCKED\n", timeStr, currentPCB->pid );
                                                    
                                                    // stop processing instructions for this process
                                                    interruptProcess = true;
                                                }
                                            // set the current interrupt info:
                                            interruptStatus->currentPid = currentPCB->pid;
                                            interruptStatus->procTime = curNode->intArg2 * IOTime;
                                            interruptStatus->current = currentPCB;
                                            
                                            pthread_create( ioThread, NULL, ioHandlerPreempted, interruptStatus );
                                            // introduce a slight amount of delay- if this is not done, process thread information
                                            // will sometimes overwrite eachother if processing is done extraordinarily fast!
                                            runTimer(15);
                                        }
                            }

                        // if interrupt detected, proceed if not using RR, otherwise wait for cycles to match quantum time
                        if( configPtr->cpuSchedCode != CPU_SCHED_RR_P_CODE || (index + 1) == configPtr->quantumCycles )
                            {
                                if( interruptStatus->interrupted && !interruptProcess && !isEmpty(waitQueue) && !segFault )
                                    {
                                        // end this process for now
                                        // make sure we're not putting a finished process on the wait queue
                                        if( curNode->nextNode != NULL )
                                            {
                                                // ensure that when process resumes, it starts where we left off
                                                currentPCB->headPtr = curNode->nextNode;
                                                // change process state to BLOCKED
                                                currentPCB->processState = "BLOCKED";
                                                // lap time
                                                accessTimer( LAP_TIMER, timeStr );
                                                // announce interrupt
                                                logger( outFile, fileFlag, printFlag, " %s, OS: Interrupted by process %d, %s operation\n", timeStr, waitQueue->front->pid, waitQueue->front->prevInstr->strArg1 );
                                                // print process sent to BLOCKED
                                                logger( outFile, fileFlag, printFlag, " %s, OS: Process %d set from RUNNING to BLOCKED\n", timeStr, currentPCB->pid );
                                                // enqueue process onto wait queue for easy access later on
                                                enqueue(waitQueue, currentPCB);
                                                // stop processing instructions for this process
                                                interruptProcess = true;
                                            }
                                        // don't enqueue process if no next instruction
                                        else
                                            {
                                                //interruptProcess = true;
                                            }
                                    }
                            }
                        // identify next opcode if next not null
                        if( curNode->nextNode != NULL)
                            {
                                curNode = curNode->nextNode;
                            }
                    }
            }
        
        // no need to print this message if segmentation fault error has already mentioned process end
        if( !segFault )
            {
                // get end time
                accessTimer( LAP_TIMER, timeStr );
                // print process end
                logger( outFile, fileFlag, printFlag, "\n %s, OS: Process %d ended\n", timeStr, currentPCB->pid );
            }
        // if no interrupts happening
        if( !interruptProcess )
            {
                // clear memory
                MMU( memoryArr, MEM_CLEAR, 0, totalMemSize, totalMemSize, memFlag, fileFlag, printFlag, currentPCB->pid, outFile );
                // change process state to EXIT
                currentPCB->processState = "EXIT";
                // lap time
                accessTimer( LAP_TIMER, timeStr );
                // print process sent to EXIT
                logger( outFile, fileFlag, printFlag, " %s, OS: Process %d set to EXIT\n", timeStr, currentPCB->pid );
            }
        // otherwise if interrupts detected
        else
            {
                // and if queue we're dequeuing from is empty and interrupts in progress
                // if we've preempted as far as we can
                if( isEmpty(PCBQueue) && interruptProcess )
                    {
                        // print that CPU is idle
                        logger( outFile, fileFlag, printFlag, " %s, OS: CPU idle, all active processes blocked\n", timeStr );
                        // wait for a thread to finish
                        // can I get the pid from the second parameter?
                        pthread_join( *ioThread, NULL );
                        // when done, lap timer
                        accessTimer( LAP_TIMER, timeStr );
                        logger( outFile, fileFlag, printFlag, " %s, OS: CPU interrupt, end idle\n", timeStr );
                        // announce interrupt
                        logger( outFile, fileFlag, printFlag, " %s, OS: Interrupted by process %d, %s operation\n", timeStr, waitQueue->front->pid, waitQueue->front->prevInstr->strArg1 );
                    }
            }
        // free PCB
        freePCB( currentPCB );

    }

// function for sorting queue linked list into proper SJF order
void sortQueueSJF( Queue *inputQueue )
    {
        // initialize variables
        bool swapped = true;
        PCB *current;
        PCB *prev = NULL;
        
        // bubble sort loop one- if a pass through the queue occurred w/ no
        // swpas, it means the list is sorted
        while( swapped )
            {
                // set swapped to false, current pcb to input queue's front
                swapped = false;
                current = inputQueue->front;

                // bubble sort loop two- while loop that avoids repeating
                // comparisons
                while( current->next != prev )
                    {
                        // if current pcb's time is more than next
                        if( current->time > current->next->time )
                            {
                                // swap the pcbs
                                swapPCBs( current, current->next );
                                // set swapped to true
                                swapped = true;
                            }
                        // move to next
                        current = current->next;
                    }
                // last sorted element
                prev = current;
            }
    }

// PCB swapping function
void swapPCBs( PCB *a, PCB *b )
    {
        // initialize variables and PCBs
        PCB temp = *a;
        *a = *b;
        *b = temp;

        // initialize temporaru next pointer and set next ptrs
        PCB *tempNext = a->next;
        a->next = b->next;
        b->next = tempNext;
    }

// memory management function
int MMU( int *inputArr, MemCodes action, int base, int limit, int total, bool memFlag, bool fileFlag, bool printFlag, int pid, FILE *outFile )
    {
        // initialize variables
        int index;
        int memRange = ( base + limit ) - 1;
        total = total - 1;
        // int realLimit = limit - 1; -----old variable, might use later
        int result;
        bool inMemBlock = false;
        int tempBase = 0; 
        int tempRange = 0;
        int count = 0;
        int tempLimit = 0;
        
        // if initialization requested
        if( action == MEM_INIT )
            {
                // for every element in the memory array, set to 0
                for( index = 0; index <= memRange; index++ )
                    {
                        inputArr[index] = 0;
                    }
                // successful initialization result
                result = MEM_SUCCESS_INIT;
            }
        // else if allocation requested
        else if( action == MEM_ALLOCATE )
            {
                // for every element in the memory array, set to 1 to signify allocated
                for( index = base; index <= memRange; index++ )
                    {
                        // if there's already 1s in this range of elements, allocation overlap is occuring
                        if( inputArr[index] == 1 || memRange > total )
                            {
                                // return a seg fault, end loop
                                result = SEG_FAULT_ALLOC;
                                break;
                            }
                        inputArr[index] = 1;
                        // set successful allocation result
                        result = MEM_SUCCESS_ALLOC;
                    }
            }
        // else if access requested
        else if( action == MEM_ACCESS )
            {
                // for every element in the memory array, check for 1s(allocated memory)
                for( index = base; index <= memRange; index++ )
                    {
                        // if 0s detected, attempting to access unallocated memory
                        if( inputArr[index] == 0 || memRange > total )
                            {
                                // return a seg fault and end loop
                                result = SEG_FAULT_ACCESS;
                                break;
                            }
                        // set successful access result
                        result = MEM_SUCCESS_ACCESS;
                    }
            }
        // else a clear was requested
        else
            {
                // set every element to 0
                for( index = base; index <= memRange; index++ )
                    {
                        inputArr[index] = 0;
                    }
                // if normal clear, return a normal clear flag, else was final clear, return final clear flag
                if( action == MEM_CLEAR )
                    {
                        result = MEM_SUCCESS_CLEAR;
                    }
                else
                    {
                        result = FINAL_CLEAR_SUCCESS;
                    }
            }

        // if printing memory actions
        if( memFlag == true )
            {
                // print in proper format
                logger( outFile, fileFlag, printFlag, "--------------------------------------------------\n" );
                // if initialization, print appropriate memory initialization statement and end the function
                if( result == MEM_SUCCESS_INIT )
                    {
                        logger( outFile, fileFlag, printFlag, "After memory initialization\n0 [ Used, P#: 0-0 ] %d\n", total );
                        logger( outFile, fileFlag, printFlag, "--------------------------------------------------\n" );
                        return MEM_SUCCESS_INIT;
                    }
                // else if allocation success, print proper response
                else if( result == MEM_SUCCESS_ALLOC )
                    {
                        logger( outFile, fileFlag, printFlag, "After allocate success\n" );
                    }
                // else if access success, print proper response
                else if( result == SEG_FAULT_ACCESS )
                    {
                        logger( outFile, fileFlag, printFlag, "After access failure\n" );
                    }
                // else if segmentation fault when allocating, print proper error
                else if( result == SEG_FAULT_ALLOC )
                    {
                        logger( outFile, fileFlag, printFlag, "After allocate overlap failure\n" );
                    }
                // else if segmentation fault when accessing, print proper error
                else if( result == MEM_SUCCESS_ACCESS )
                    {
                        logger( outFile, fileFlag, printFlag, "After access success\n" );
                    }
                // else if final clear, print final clear message and end the function
                else if( result == FINAL_CLEAR_SUCCESS )
                    {
                        logger( outFile, fileFlag, printFlag, "After clear all process success\n" );
                        logger( outFile, fileFlag, printFlag, "No memory configured\n" );
                        logger( outFile, fileFlag, printFlag, "--------------------------------------------------\n" );
                        return FINAL_CLEAR_SUCCESS;
                    }
                // else was a normal clear, print clear message and end the function
                else
                    {
                        logger( outFile, fileFlag, printFlag, "After clear process %d success\n0 [ Used, P#: 0-0 ] %d\n", pid, total );
                        logger( outFile, fileFlag, printFlag, "--------------------------------------------------\n" );
                        return MEM_SUCCESS_CLEAR;
                    }
                
                // fall through to here if not a memory initialization, final clear, or normal clear
                // loop through mem array
                for( index = 0; index < total; index++ )
                    {
                        // if element is 1
                        if( inputArr[index] == 1 )
                            {
                                // if not confirmed to already be in a block of memory, assume so now
                                if( !inMemBlock )
                                    {
                                        // set in memory block to true, and a tempoarary base value
                                        inMemBlock = true;
                                        tempBase = index;
                                    }
                            }
                        // else if element is 0, last element was a 1, and index is not element 0(to prevent looking at element -1)
                        else if( index > 0 && inputArr[index - 1] == 1 && inputArr[index] == 0 )
                            {
                                // memory block has ended
                                inMemBlock = false;
                                // the last index where we had a 1 is the range, thus that range - base is the limit value
                                tempRange = index - 1;
                                tempLimit = tempRange - tempBase;
                                // print memory block
                                logger( outFile, fileFlag, printFlag, "%d [ Used, P#: %d, %d-%d ] %d\n", count, pid, tempBase, tempRange, tempLimit );
                                // reset base and range just to be safe
                                tempBase = 0;
                                tempRange = 0;
                                // increment count(count is used in the case of multiple memory blocks being allocated in a single process)
                                count++;
                            }
                    }
                // print the ending memory action message statements
                logger( outFile, fileFlag, printFlag, "%d [ Open, P#: x, 0-0 ] %d\n", limit, total );
                logger( outFile, fileFlag, printFlag, "--------------------------------------------------\n" );
            }
        // return bool result(success or error)
        return result;
    }