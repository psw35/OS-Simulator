//--------------The below code was authored by Michael Leverington as a utility for the program---------------
//--------------To see the code written by Payton Watts, see simulator.c--------------------------------------

// Preprocessor directive
#ifndef STRING_UTILS_H
#define STRING_UTILS_H

// header files
#include <stdio.h>
#include <stdbool.h>
#include "StandardConstants.h"

// prototypes
int getStringLength( const char *testStr );

int compareString( const char *oneStr, const char *otherStr );

void copyString( char *destStr, const char *sourceStr );

void concatenateString( char *destStr, const char *sourceStr );

int findSubString( const char *testStr, const char *searchSubStr );

bool getStringConstrained( FILE *inStream,
                        bool clearLeadingNonPrintable, 
                        bool clearLeadingSpace,
                        bool stopAtNonPrintable,
                        char delimiter,
                        char *capturedString );

bool getStringToDelimiter(FILE *inStream, char delimiter, char *capturedString);

bool getStringToLineEnd( FILE *inStream,
                        char *capturedString );

void getSubString( char *destStr, const char *sourceStr,
 int startIndex, int endIndex );

char toLowerCase( char testChar );

void setStrToLowerCase( char *destStr, const char *sourceStr );


#endif // STRING_UTILS_H