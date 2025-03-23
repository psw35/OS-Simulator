//--------------The below code was authored by Michael Leverington as a utility for the program---------------
//--------------To see the code written by Payton Watts, see simulator.c--------------------------------------

// header files
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "StandardConstants.h"
#include "StringUtils.h"

// global constants

// function implementations

/*
Name: getStringLength
Process: finds the length of a string by counting characters
 up to the NULL_CHAR character
Function Input/Parameters: c-style string (char *)
Function Output/Parameters: none
Function Output/Returned: length of string
Device Input/Keyboard: none
Device Output/Monitor: result displayed
Dependencies: none
*/
int getStringLength( const char *testStr )
    {
        // intialize function/variables
        int index = 0;

        // loop to end of string, protect from overflow
        while( index < STD_STR_LEN && testStr[index] != NULL_CHAR )
            {    
                // update index
                index++;
            }
        // end loop

        // return index/length
        return index;
    }

/*
Name: compareString
Process: compares two strings with the following results:
        if left string < right string, returns less than 0
        if left string > right string, returns greater than 0
        if left string = right string, returns 0
        - equals test includes length
Function Input/Parameters: c-style left and right strings (char *)
Function Output/Parameters: none
Function Output/Returned: results as specified(int)
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: getStringLength
*/
int compareString( const char *oneStr, const char *otherStr )
    {
        // initizliaze function/variables
        int diff, index = 0;

        // loop to end of shortest string
        // with overrun protection
        while( oneStr[ index ] != NULL_CHAR 
               && otherStr[ index ] != NULL_CHAR 
               && index < MAX_STR_LEN )
            {
                // get difference in chars
                diff = oneStr[ index ] - otherStr[ index ];

                // check for difference between chars
                if( diff != 0 )
                    {
                        // return difference
                        return diff;
                    }
                // else increment index
                index++;
            }
        // end loop

        // return difference in lengths, if any
            // function: getStringLength
        return getStringLength( oneStr ) - getStringLength( otherStr );

    }

/*
Name: copyString
Process: copies one string into another, overwriting data in
the destination string
Function Input/Parameters: c-style source string (char *)
Function Output/Parameters: c-style destination string (char *)
Function Output/Returned: none
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: getStringLength
*/
void copyString( char *destStr, const char *sourceStr )
    {
        // initialize function/variables
        int index = 0;

        // check for src/dest not the same (aliasing)
        if( destStr != sourceStr )
            {
                // loop to end of src string
                while( sourceStr[ index ] != NULL_CHAR && index < MAX_STR_LEN )
                    {
                        // assign chars to end of dest string
                        destStr[ index ] = sourceStr[ index ];

                        // update index
                        index++;

                        // set temp end of dest string
                        destStr[ index ] = NULL_CHAR;
                    }

            }
            // end loop

    }

/*
Name: concatenateString
Process: appends one string onto another
Function Input/Parameters: c-style source string (char *)
Function Output/Parameters: c-style destination string (char *)
Function Output/Returned: none
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: getStringLength, copyString, malloc
*/
void concatenateString( char *destStr, const char *sourceStr )
    {
        // initializae function/variables
        
            // set dest index
                // function: getStringLength
            int destIndex = getStringLength( destStr );

            // set src string legnth
                // function: getStringLength
            int sourceStrLen = getStringLength( sourceStr );
            
            // create temp string pointer
            char *tempStr;

            // create other variables
            int sourceIndex = 0;

        // copy src string in case of aliasing
            // function: malloc, copyString
        tempStr = (char *)malloc( sizeof( sourceStrLen + 1 ) );
        copyString( tempStr, sourceStr );

        // loop to end of src string
        while( tempStr[ sourceIndex ] != NULL_CHAR && destIndex < MAX_STR_LEN )
            {
                // assign chars to end of destination str
                destStr[ destIndex ] = tempStr[ sourceIndex ];

                // update indices
                destIndex++;
                sourceIndex++;

                // set temp end of dest str
                destStr[ destIndex ] = NULL_CHAR;
            }
        // end loop

        // release memory used for temp str
            // function: free
        free( tempStr );
        
    }

/*
Name: findSubString
Process: linear search for given substring within larger string
Function Input/Parameters: c-style source test string (char *),
                            c-style source search string (char *)
Function Output/Parameters: none
Function Output/Returned: index of found substring, or SUBSTRING_NOT_FOUND
                            constant if string not found
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: getStringLength
*/
int findSubString( const char *testStr, const char *searchSubStr )
    {
        // initialize function/variables
            
            // initialize test str length
                // function: getStringLength
            int testStrLen = getStringLength( testStr );

            // initialize master index - location of sub string start point
            int masterIndex = 0;

            // initialize other variables
            int searchIndex, internalIndex;

        // loop across test str
        while( masterIndex < testStrLen )
            {
                // set internal loop index to current test str index
                internalIndex = masterIndex;

                // set internal search index to 0
                searchIndex = 0;

                // loop to end of test str while test and sub string chars the same
                while( internalIndex <= testStrLen && testStr[ internalIndex ]
                 == searchSubStr[ searchIndex ] )
                    {
                        // increment test str, substring indices
                        internalIndex++;
                        searchIndex++;

                        // check for end of substring (search completed)
                        if( searchSubStr[ searchIndex ] == NULL_CHAR )
                            {
                                //  return beginning location of sub string
                                return masterIndex;

                            }

                    }
                // end internal loop comparison
            
                // increment current beginning location index
                masterIndex++;
            }
        // end loop across test str

        // assume test has failed at this point, return SUBSTRING_NOT_FOUND
        return SUBSTRING_NOT_FOUND;

    }

/*
Name: getStringConstrained
Process: captures a string from the input stream  w/ various constraints
            Note: consumes delimiter
Function Input/Parameters: input stream (FILE *)
                            clears leading non printable (bool),
                            clears leading space (bool),
                            stops at non printable (bool),
                            stops at specified delimiter (char)
                            Note: consumes delimiter
Function Output/Parameters: string returned (char *)
Function Output/Returned: success of operation (bool)
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: fgetc
*/
bool getStringConstrained( FILE *inStream,
                        bool clearLeadingNonPrintable, 
                        bool clearLeadingSpace,
                        bool stopAtNonPrintable,
                        char delimiter,
                        char *capturedString )
    {
        // initialize function/variables
        int intChar = EOF, index = 0;

        // initialize output str
        capturedString[ index ] = NULL_CHAR;
        
        // capture first value in stream
            // function: fgetc
        intChar = fgetc( inStream );

        // loop to clear non printable or space, if indicated
        while( ( intChar != EOF ) &&
         ( ( clearLeadingNonPrintable && intChar < (int)SPACE )
          || ( clearLeadingSpace && intChar == (int)SPACE ) ) )
            {
                // get next char
                    // function: fgetc
                intChar = fgetc( inStream );
            
            }
        // end clear non printable/space loop

        // check for end of file found
        if( intChar == EOF )
            {
                // return failed operation
                return false;

            }
        // loop to capture input
        while (
            // continue if not at end of file and max str length not reached
            ( intChar != EOF && index < MAX_STR_LEN - 1 )
            // AND
            // continues if not printable flag set and chars are printable
            // OR continues if not printable flag not set
            && ( ( stopAtNonPrintable && intChar >= (int)SPACE )
             || ( !stopAtNonPrintable ) )
            // AND
            // continues if specified delimiter is not found
            && ( intChar != (char)delimiter )
        )
            {
                // place char in array element
                capturedString[index] = (char)intChar;

                // increment array index
                index++;

                // set next element to null char/end of c-string
                capturedString[ index ] = NULL_CHAR;

                // get next char as integer
                    // function: fgetc
                intChar = fgetc( inStream );

            }    
        // end loop

        // return successful operation
        return true;

    }

/*
Name: getStringToDelimiter
Process: captures a string from the input stream to a specific delimiter
            Note: consumes delimiter
Function Input/Parameters: input stream (FILE *)
                            stops at specified delimiter (char)
Function Output/Parameters: string returned (char *)
Function Output/Returned: success of operation (bool)
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: getStringConstrained
*/
bool getStringToDelimiter(FILE *inStream, char delimiter, char *capturedString)
    {
        // call engine function w/ delimiter
            // function: getStringConstrained
        return getStringConstrained( inStream, true, true, true,
         delimiter, capturedString );
    }

/*
Name: getStringConstrained
Process: captures a string from the input stream to the end of the line
Function Input/Parameters: input stream (FILE *)
Function Output/Parameters: string returned (char *)
Function Output/Returned: success of operation (bool)
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: getStringConstrained
*/
bool getStringToLineEnd( FILE *inStream,
                        char *capturedString )
    {
        // call engine function w/ delimiter
            // function: getStringConstrained
        return getStringConstrained( inStream, true, true, true,
         NON_PRINTABLE_CHAR, capturedString );
    }

/*
Name: getSubString
Process: capture sub string within larger string
        between two inclusive indices
        returns empty string if either index is out of range,
        assumes enough memory in destination string
Function Input/Parameters: c-style source string (char *),
                             start and end indices (int)
Function Output/Parameters: c-style destination string (char *)
Function Output/Returned: none
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: getStringLength, malloc, copyString, free
*/
void getSubString( char *destStr, const char *sourceStr,
 int startIndex, int endIndex )
    {
        // initialize function/variables

            // set length of src string
                // function: getStringLength
            int sourceStrLen = getStringLength( sourceStr );

            // initialize the dest index to 0
            int destIndex = 0;

            // initialize src index to start index (parameter)
            int sourceIndex = startIndex;

            // create pointer for temp string
            char *tempStr;

        // check for indices within limits
        if( startIndex >= 0 && startIndex <= endIndex && endIndex < sourceStrLen )
            {
                // create temp string
                    // function: malloc, copyString
                tempStr = (char *)malloc( sourceStrLen + 1 );
                copyString( tempStr, sourceStr );
                
                // loop across requested substring (indices)
                while( sourceIndex <= endIndex )
                    {
                        // assign src char to dest element
                        destStr[ destIndex ] = tempStr[ sourceIndex ];

                        // increment both indices
                        destIndex++;
                        sourceIndex++;
                        
                        // set temp end of dest str
                        destStr[ destIndex ] = NULL_CHAR;
                    }
                // end loop

                // return memory for temp str
                    // function: free
                free( tempStr );

            }

    }

/*
Name: toLowerCase
Process: if char is upper case, sets it to lower case, otherwise nothing done
Function Input/Parameters: test character (char)
Function Output/Parameters: none
Function Output/Returned: character to set to lower case, if appropriate
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: none
*/
char toLowerCase( char testChar )
    {
        // check for upper case letter
        if( testChar >= 'A' && testChar <= 'Z' )
            {
                // return lower case letter
                return testChar - 'A' + 'a';
            }

        // otherwise, assume no upper case letter, return char unchanged
        return testChar;
        
    }

/*
Name: setStrToLowerCase
Process: iterates through string, sets any upper case letter to lower
Function Input/Parameters: c-style source string (char *),
Function Output/Parameters: c-style destination string (char *)
Function Output/Returned: none
Device Input/Keyboard: none
Device Output/Monitor: none
Dependencies: toLowerCase, getStringLength, copyString, malloc, free
*/
void setStrToLowerCase( char *destStr, const char *sourceStr )
    {
        // initialize function/variables

            // get src string length
                // function: getStringLength
            int sourceStrLen = getStringLength( sourceStr );

            // create temp string ptr
            char *tempStr;

            // create other variables
            int index = 0;

        // copy src string in case of aliasing
            // function: malloc, copyString
        tempStr = (char *)malloc( sizeof( sourceStrLen + 1 ) );
        copyString( tempStr, sourceStr );

        // loop across src string
        while( tempStr[ index ] != NULL_CHAR && index < MAX_STR_LEN )
            {
                // set individual char to lower case as needed
                // assign destination to str
                destStr[ index ] = toLowerCase( tempStr[ index ] );
                
                // update index
                index++;

                // set temp end of dest str
                destStr[ index ] = NULL_CHAR;

            }
        // end loop

        // release memory from temp str
            // function: free
        free( tempStr );

    }
