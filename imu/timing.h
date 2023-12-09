// timing.h
// Module to use functions relating to the CPU clock time
// Provides functions to sleep and get the current clock time

#ifndef _TIMING_H_
#define _TIMING_H_

// Define Conversion Factors
#define NS_PER_SECOND 1000000000
#define NS_PER_MS 1000000

// Define function prototypes
void sleepForMs(const long long delayInMs); // Function to sleep a thread for a time in milliseconds
double getTimeInNs(void); // Get the time from the REALTIME_CLOCK in nanoseconds

#endif