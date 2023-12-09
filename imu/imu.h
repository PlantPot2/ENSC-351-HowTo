#ifndef IMU_H
#define IMU_H

struct Gyro {
    float x;
    float y;
    float z;
};

struct Gyro getGyro();

void imu_Initialization();

void imuThreadStart();

void imuThreadEnd();

struct Gyro getGyroAvg();
double getGyroMagnitude();

void *imuSamplingThread();

#endif