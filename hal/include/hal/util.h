/*
General utility functions
*/
#ifndef _UTIL_H_
#define _UTIL_H_

//open, read a single line, and close with safety
//now it behaves like a proper function! yay!
int file_read(char *fileName, char *buff, int bufferSize);

//open, write to file using fprintf, and close with safety
#define file_write(fileName, ...)\
do {\
    FILE *_pFile = fopen(fileName, "w");\
    if (_pFile == NULL) {\
        printf("ERROR OPENING %s.\n", fileName);\
    }\
    else if (fprintf(_pFile, __VA_ARGS__) <= 0) {\
        printf("ERROR WRITING DATA\n");\
    }\
    else {\
        fclose(_pFile);\
    }\
} while(0)

long long get_time_ms();
void sleep_ms(long long delayInMs);

void run_command(char *command);

void swap(void *a, void *b, int size);

#endif