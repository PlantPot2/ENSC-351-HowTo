#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <math.h>

#include "imu.h"
#include "imu_utils.h"
#include "timing.h"

#define P9_20_CONFIG "config-pin P9_19 i2c"
#define P9_19_CONFIG "config-pin P9_20 i2c"
#define POLLING_RATE 50

static void updateOldGyro();
static void getNewGyro();
static void updateNewAvg_GyroVal();
static void runCommand(char *command);
static void initializeI2Cpins();

static imu_Util *imu_Util_obj;
static error *err;
static IMU_OUTPUT output;

static struct Gyro gyroVal;
static struct Gyro avg_gyroVal;
static struct Gyro pre_gyroVal = {0.0, 0.0, 0.0};

static pthread_t tid;
static pthread_attr_t attr;
static bool isThreadRunning = false;
pthread_mutex_t imuMutex = PTHREAD_MUTEX_INITIALIZER;

void imu_Initialization() {
    initializeI2Cpins();

    err = imu_Util_initialization(&imu_Util_obj, "/dev/i2c-2", ACC_RES_2G, GYRO_RES_250DPS);
    if (err != NULL) {
        return;
    }
    return;
}

static void initializeI2Cpins(){
    runCommand(P9_20_CONFIG);
    runCommand(P9_19_CONFIG);
}

struct Gyro getGyro(){
    return gyroVal;
}

double getGyroMagnitude(){
    double gyroXX = (double)(getGyroAvg().x * getGyroAvg().x);
    double gyroYY = (double)(getGyroAvg().y * getGyroAvg().y);
    double gyroZZ = (double)(getGyroAvg().z * getGyroAvg().z);

    double gyroMag = sqrt(gyroXX + gyroYY + gyroZZ);
    return gyroMag;
}

static void setGyro() {
    pthread_mutex_lock(&imuMutex);
    err = imu_read(imu_Util_obj, &output);
    if (err != NULL) {

    }

    updateOldGyro();
    getNewGyro();
    updateNewAvg_GyroVal();
    pthread_mutex_unlock(&imuMutex);
    return;
}

static void updateOldGyro() {
    pre_gyroVal.x = avg_gyroVal.x;
    pre_gyroVal.y = avg_gyroVal.y;
    pre_gyroVal.z = avg_gyroVal.z;
}

static void getNewGyro() {
    gyroVal.x = output.gyro.x;
    gyroVal.y = output.gyro.y;
    gyroVal.z = output.gyro.z;   
}

static void updateNewAvg_GyroVal() {
    avg_gyroVal.x = (0.8 * gyroVal.x) + (0.2 * pre_gyroVal.x);
    avg_gyroVal.y = (0.8 * gyroVal.y) + (0.2 * pre_gyroVal.y);
    avg_gyroVal.z = (0.8 * gyroVal.z) + (0.2 * pre_gyroVal.z);
}

struct Gyro getGyroAvg() {
    return avg_gyroVal;
}

static void *imuSmaplingThread(void *arg) {
    while (isThreadRunning) {
        setGyro();

        sleepForMs(POLLING_RATE);
    }
    return NULL;
}

void imuThreadStart() {
    isThreadRunning = true;
    pthread_create(&tid, &attr, imuSmaplingThread, NULL);
}

void imuThreadEnd() {
    imu_free(imu_Util_obj);
    isThreadRunning = false;
    pthread_join(tid, NULL);
}

static void runCommand(char* command)
{
    // Execute the shell command (output into pipe)
    FILE* pipe = popen(command, "r");

    // Ignore output of the command; but consume it
    // so we don't get an error when closing the pipe.
    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe)) {
        if (fgets(buffer, sizeof(buffer), pipe) == NULL)
            break;
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