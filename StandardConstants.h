//--------------The below code was authored by Michael Leverington as a utility for the program----------
//--------------With some additions written by Payton----------------------------------------------------

// Preprocessor directives
#ifndef STANDARD_CONSTANTS_H
#define STANDARD_CONSTANTS_H

// Include Boolean
#include <stdbool.h>

// global constants

// ------------------Function Comment Template------------------

/*
Name: 
Process: 
Function Input/Parameters: 
Function Output/Parameters: 
Function Output/Returned: 
Device Input/device: 
Device Output/monitor: 
Dependencies: 
*/

// constant for end of c-string null character
#define CARRIAGE_RETURN_CHAR '\r'

//constant for a COLON
#define COLON ':'

//constant for a COMMA
#define COMMA ','

//Constant to print a dash
#define DASH '-'

// constant for large string length
#define LARGE_STR_LEN 96

// max queue length for PCB queue- limit 100, I doubt a config will have over
// 100 processes requested
#define MAX_QUEUE_SIZE 100

// constant for end of c-string null character
#define NEWLINE_CHAR '\n'

// constant for small string length
#define MIN_STR_LEN 32

// constant for maximum string length
#define MAX_STR_LEN 128

// constant for end of c-string null character
#define NULL_CHAR '\0'

// constant for end of c-string newline char
#define NEWLINE_CHAR '\n'

// constant for default non printable char
#define NON_PRINTABLE_CHAR (char)127

// constant for semicolon
#define SEMICOLON ';'

//Constant for a period
#define PERIOD '.'

// constant for space character
#define SPACE ' '

// constant for standard string length
#define STD_STR_LEN 64

// constant for equal strings
#define STR_EQ 0

// constant for substring search failure
#define SUBSTRING_NOT_FOUND -1

#endif  // STANDARD_CONSTANTS


