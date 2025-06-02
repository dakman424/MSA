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
    SENSOR_MPU = 1
} Sensor_type_t;

errcode getObserveData_GENE06(void *data, size_t size);

#endif