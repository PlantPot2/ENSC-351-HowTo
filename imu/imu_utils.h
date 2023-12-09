#pragma once

typedef const char error;

typedef enum {
    ACC_RES_2G,
    ACC_RES_4G,
    ACC_RES_8G,
    ACC_RES_16G
} IMU_ACC_RESOLUTION;

typedef enum {
    GYRO_RES_250DPS,
    GYRO_RES_500DPS,
    GYRO_RES_1000DPS,
    GYRO_RES_2000DPS
} IMU_GYRO_RESOLUTION;

typedef struct {
    union {
        struct {
            double x;
            double y;
            double z;
        }   accel;
        double accel_array[3];
    };
    union {
        struct {
            double x;
            double y;
            double z;
        } gyro;
        double gyro_arry[3];
    };
} IMU_OUTPUT;

typedef void imu_Util;

error *imu_Util_initialization(imu_Util **pthd_Obj, const char *path, IMU_ACC_RESOLUTION acc_res, IMU_GYRO_RESOLUTION gyro_res);

void imu_free(imu_Util *ojb);

error *imu_read(imu_Util *Obj, IMU_OUTPUT *output);