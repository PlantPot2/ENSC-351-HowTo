// this file contains the functions needed to set up imu_utils

#include <stdlib.h> // standard library
#include <stdint.h> // for exact width integer
#include <unistd.h> // unix standard
#include <math.h>   // for math functions

#include <sys/ioctl.h>      // for input/output
#include <linux/i2c-dev.h>  // for i2c

#include "imu_setup.h"
#include "imu.h"

//taken from register list online
#define PWR_MGMT_1 0x6B
#define PWR_MGMT_1_DISABLE (1 << 3)
#define ACCEL_CONFIG 0x1C
#define ACCEL_XOUT_H 0x3B
#define GYRO_CONFIG 0x1B
#define GRYO_XOUT_H 0x43

typedef struct{
    int fd;
    uint8_t address;
    double gyro_setup;
    double acc_setup;
} obj_Setup;

error *imu_Setup_initialize(imu_Setup **pthd_Obj, int fd, uint8_t address, IMU_ACC_RESOLUTION acc_res, IMU_GYRO_RESOLUTION gyro_res){
    obj_Setup *obj_1 = malloc(sizeof(obj_Setup));

    obj_1->fd = fd;
    obj_1->address = address;

    uint8_t accel_config = 0;

    switch (acc_res){
        case ACC_RES_2G:
            accel_config = 0;
            obj_1->acc_setup = 32768.0f / 2.0f;
            break;

        case ACC_RES_4G:
            accel_config = (1 << 3);
            obj_1->acc_setup = 32768.0f / 4.0f;
            break;

        case ACC_RES_8G:
            accel_config = (2 << 3);
            obj_1->acc_setup = 32768.0f / 8.0f;
            break;

        case ACC_RES_16G:
            accel_config = (3 << 3);
            obj_1->acc_setup = 32768.0f / 16.0f;
            break;
    }

    uint8_t gyro_config = 0;

    switch (gyro_res){
        case GYRO_RES_250DPS:
            gyro_config = 0;
            obj_1->gyro_setup = 32768.0f / 250.0f;
            break;
        
        case GYRO_RES_500DPS:
            gyro_config = 0;
            obj_1->gyro_setup = 32768.0f / 500.0f;
            break;

        case GYRO_RES_1000DPS:
            gyro_config = 0;
            obj_1->gyro_setup = 32768.0f / 1000.0f;
            break;
        
        case GYRO_RES_2000DPS:
            gyro_config = 0;
            obj_1->gyro_setup = 32768.0f / 2000.0f;
            break;
    }

    int ioControl = ioctl(obj_1->fd, I2C_SLAVE, obj_1->address);
    if (ioControl != 0){
        free (obj_1);
        return "ioctl failed";
    }

    uint8_t cmd[2];

    cmd[0] = PWR_MGMT_1;
    cmd[1] = PWR_MGMT_1_DISABLE;
    ioControl = write(obj_1->fd, cmd, 2);
    if (ioControl != 2){
        free(obj_1);
        return "write failed";
    }

    cmd[0] = ACCEL_CONFIG;
    cmd[1] = accel_config;
    ioControl = write(obj_1->fd, cmd, 2);
    if (ioControl != 2) {
        free(obj_1);
        return "write failed";
    }

    *pthd_Obj = obj_1;
    return NULL;
}

void imu_Setup_free(imu_Setup *Obj) { free(Obj); }

static inline int16_t make_int16(uint8_t *data) {
    return data[0] << 8 | data [1];
}

error *imu_Setup_read(void *Obj, IMU_OUTPUT *output) {
    obj_Setup *obj_2 = (obj_Setup *)Obj;

    uint8_t out_raw[6];

    uint8_t cmd = ACCEL_XOUT_H;
    int ioControl = write(obj_2->fd, &cmd, 1);
    if (ioControl != 1) {
        return "write failed";;
    }

    ioControl = read(obj_2->fd, out_raw, 6);
    if (ioControl != 6) {
        return "read failed";
    }

    output-> accel.x = (double)make_int16(&out_raw[0]) / obj_2->acc_setup;
    output-> accel.y = (double)make_int16(&out_raw[2]) / obj_2->acc_setup;
    output-> accel.z = (double)make_int16(&out_raw[4]) / obj_2->acc_setup;

    cmd = GRYO_XOUT_H;
    ioControl = write(obj_2->fd, &cmd, 1);
    if (ioControl != 1) {
        return "write failed";
    }

    ioControl = read(obj_2->fd, out_raw, 6);
    if (ioControl != 6) {
        return "read failed";
    }

    output->gyro.x = (double)make_int16(&out_raw[0]) / obj_2->gyro_setup;
    output->gyro.y = (double)make_int16(&out_raw[2]) / obj_2->gyro_setup;
    output->gyro.z = (double)make_int16(&out_raw[4]) / obj_2->gyro_setup;
}