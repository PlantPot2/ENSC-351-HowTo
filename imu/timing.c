#include <stdlib.h>
#include <time.h>

#include "timing.h"

void sleepForMs(const long long delayInMs)
{
    long long delayNs = delayInMs * NS_PER_MS; // Conert delay to nanoseconds
    int seconds = delayNs/NS_PER_SECOND; // Convert delay to seconds
    int nanoSeconds = delayNs % NS_PER_SECOND; // Get remaining nanoseconds of delay

    struct timespec reqDelay = {seconds, nanoSeconds}; // create the struct to pass to the sleep function
    nanosleep(&reqDelay, (struct timespec *) NULL); // Sleep function
}

double getTimeInNs(void)
{
    struct timespec spec; // Create timespec struct named spec
    clock_gettime(CLOCK_REALTIME, &spec); // Get the time of the computer clock and store it in spec
    long long seconds = spec.tv_sec; // Get the seconds of current time
    long long nanoSeconds = spec.tv_nsec; // Get the nanoseconds of current time
    long long timeInNS = seconds*NS_PER_SECOND + nanoSeconds; // Combine and store the current time in milliseconds

    return timeInNS;
}
