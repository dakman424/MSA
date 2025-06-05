#ifndef __MPU6050_H
#define __MPU6050_H
#include "../Sensor/Sensor.h"
#include "stdint.h"

typedef struct mpu6050 mpu6050;

errcode mpu_init();
errcode mpu_service(I2C_HandleTypeDef *I2Cx);
errcode mpu_close();

#endif