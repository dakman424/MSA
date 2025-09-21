#ifndef _COMMON_H_
#define _COMMON_H_
typedef enum
{
    ERROR_OK,
    ERROR_ERROR,
    ERROR_WARNING
} errcode;

typedef enum Sensor_type{
    SENSOR_GYNE06 = 0,
    SENSOR_MPU = 1,
    SENSOR_MS5611 = 2

} Sensor_type_t;

#endif