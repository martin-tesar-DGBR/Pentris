#ifndef TUNE_THREAD_H
#define TUNE_THREAD_H

//this will be the main loop of the thread
void *tuneLoop(void);

//sends signal to end the loop
void endLoop(void);

//starts the thread
void startThread(void);

#endif