#pragma once

#include "imu_utils.h"

typedef void imu_Setup;

//this function will
error *imu_Setup_initialize(imu_Setup **pthd_Obj, int fd, uint8_t address, IMU_ACC_RESOLUTION acc_res, IMU_GYRO_RESOLUTION gyro_res);

void imu_Setup_free(imu_Setup *Obj);

error *imu_Setup_read(void *Obj, IMU_OUTPUT *r);