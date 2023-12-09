// this file contains all functions need for execution of imu.c

#include <stdlib.h> //standard library
#include <stdio.h>  // standard input/output
#include <stdint.h> // for exact width integers
#include <unistd.h> // unix standard
#include <fcntl.h>  // file control

#include <linux/i2c-dev.h>  // used for i2c
#include <sys/ioctl.h>      // used for input/output

#include "imu_utils.h"
#include "imu_setup.h"

typedef error *(*sensor_read_func)(void *, IMU_OUTPUT *);

typedef struct {
    int fd;
    void *sensor;
    sensor_read_func sensor_read;
} obj_Utils;

static error *find_and_initialize_sensor(obj_Utils *Obj, IMU_ACC_RESOLUTION acc_res, IMU_GYRO_RESOLUTION gyro_res) {
    for (uint8_t address = 0x68; address <= 0x69; address++) {
        int ioControl = ioctl(Obj->fd, I2C_SLAVE, address);
        if (ioControl != 0) {
            continue;
        }

        uint8_t cmd = 0x75;
        ioControl = write(Obj->fd, &cmd, 1);
        if (ioControl != 1) {
            continue;
        }

        uint8_t who_am_i;
        ioControl = read(Obj->fd, &who_am_i, 1);
        if (ioControl != 1) {
            continue;
        }

        switch (who_am_i){ //this was for different IMU's really only interested in 0x68
            case 0x68:
                printf("found MPU6000/MPU6050 with address 0x%.2x\n", address);
                break;

            case 0x70:
                printf("found MPU6500 with address 0x%.2x\n", address);
                break;

            case 0x11:
                printf("found ICM20600 with address 0x%.2x\n", address);
                break;

            case 0xAC:
                printf("found ICM20601 with address 0x%.2x\n", address);
                break;

            case 0x12:
                printf("found ICM20602 with address 0x%.2x\n", address);
                break;

            default:
                continue;
        }

        error *err = imu_Setup_initialize(&Obj->sensor, Obj->fd, address, acc_res, gyro_res);
        if (err != NULL) {
            return err;
        }

        Obj->sensor_read = imu_Setup_read;
        return NULL;
    }

    return "no IMU sensor found";
}

error *imu_Util_initialization(imu_Util **pthd_Obj, const char *path, IMU_ACC_RESOLUTION acc_res, IMU_GYRO_RESOLUTION gyro_res){
    obj_Utils *obj1 = malloc(sizeof(obj_Utils));

    obj1->fd = open(path, O_RDWR);
    if (obj1->fd < 0) {
        return "unable to open the device";
    }

    error *err = find_and_initialize_sensor(obj1, acc_res, gyro_res);
    if (err != NULL) {
        close(obj1->fd);
        free(obj1);
        return err;
    }

    *pthd_Obj = obj1;
    return NULL;
}

void imu_free(imu_Util *Obj) {
    obj_Utils *obj2 = (obj_Utils *)Obj;
    close(obj2->fd);
    free(Obj);
}

error *imu_read(imu_Util *Obj, IMU_OUTPUT *output) {
    obj_Utils *obj2 = (obj_Utils *)Obj;
    return obj2->sensor_read(obj2->sensor, output);
}