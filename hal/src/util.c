#include "hal/util.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

int file_read(char *fileName, char *buff, int bufferSize) {
    FILE *_pFile = fopen(fileName, "r");
    if (_pFile == NULL) {
        printf("ERROR: Unable to open file (%s) for read\n", fileName);
        return -1;
    }
    fgets(buff, bufferSize, _pFile);
    fclose(_pFile);
    return 0;
}

long long get_time_ms() {
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    long long seconds = spec.tv_sec;
    long long nanoSeconds = spec.tv_nsec;
    long long milliSeconds = seconds * 1000
        + nanoSeconds / 1000000;
    return milliSeconds;
}

void sleep_ms(long long delayInMs) {
    const long long NS_PER_MS = 1000 * 1000;
    const long long NS_PER_SECOND = 1000000000;
    long long delayNs = delayInMs * NS_PER_MS;
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND;
    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *) NULL);
}


void run_command(char *command) {
    // Execute the shell command (output into pipe)
    FILE *pipe = popen(command, "r");
    // Ignore output of the command; but consume it
    // so we don't get an error when closing the pipe.
    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe)) {
        if (fgets(buffer, sizeof(buffer), pipe) == NULL) {
            break;
        }
        // printf("--> %s", buffer); // Uncomment for debugging
    }
    // Get the exit code from the pipe; non-zero is an error:
    int exitCode = WEXITSTATUS(pclose(pipe));
    if (exitCode != 0) {
        perror("Unable to execute command:");
        printf(" command: %s\n", command);
        printf(" exit code: %d\n", exitCode);
    }
}

void swap(void *a, void *b, int size) {
    uint8_t temp[size];

    memcpy(temp, (uint8_t *) a, size);
    memcpy((uint8_t *) a, (uint8_t *) b, size);
    memcpy((uint8_t *) b, temp, size);
}