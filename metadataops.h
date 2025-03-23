//--------------The below code was authored by Michael Leverington as a utility for the program---------------
//--------------To see the code written by Payton Watts, see simulator.c--------------------------------------

// Preprocessor directives
#ifndef METADATAOPS_H
#define METADATAOPS_H

// header files
#include <stdio.h>
#include <stdlib.h>
#include "datatypes.h"
#include "StandardConstants.h"
#include "StringUtils.h"

// GLOBAL constants
typedef enum { BAD_ARG_VAL = -1,
               NO_ACCESS_ERR,
               MD_FILE_ACCESS_ERR,
               MD_CORRUPT_DESCRIPTOR_ERR,
               OPCMD_ACCESS_ERR,
               CORRUPT_OPCMD_ERR,
               CORRUPT_OPCMD_ARG_ERR,
               UNBALANCED_START_END_ERR,
               COMPLETE_OPCMD_FOUND_MSG,
               LAST_OPCMD_FOUND_MSG } OpCodeMessages;

// function prototypes

OpCodeType *addNode( OpCodeType *localPtr, OpCodeType *newnNode );

int getCommand( char *cmd, const char *inputStr, int index );

OpCodeMessages getOpCommand( FILE *filePtr, OpCodeType *inData );

int getNumberArg( int *number, const char *inputStr, int index );

int getStringArg( char *strArg, const char *inputStr, int index );

bool isDigit( char testChar );

int updateEndCount( int count, const char *opString );

int updateStartcount( int count, const char *opString );

bool verifyFirstStringArg( const char *strArg );

bool verifyValidCommand( const char *testCmd );

bool getMetaData( const char *fileName, OpCodeType **opCodeDataHead,
                    char *endStateMsg );

void displayMetaData( const OpCodeType *localPtr );

OpCodeType *clearMetaDataList( OpCodeType *localPtr );

#endif