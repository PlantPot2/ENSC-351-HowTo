#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "imu.h"
#include "imu_utils.h"
#include "timing.h"

int main() {

    printf("initialzing pins\n");
    imu_Initialization();

    imuThreadStart();

    double startTime = getTimeInNs();
    double currentTime, gameTime;
    struct Gyro gyro;

    gameTime = 0;

    while (gameTime < 10000000000) {
        currentTime = getTimeInNs();
        gameTime = currentTime - startTime;
        gyro = getGyro();

        printf("gyro x,y,z: %f, %f, %f\n", gyro.x, gyro.y, gyro.z);

        sleepForMs(100);
    }

    imuThreadEnd();

}