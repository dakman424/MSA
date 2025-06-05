#include "stdint.h"
#include "stdio.h"
#include "observer.h"
typedef struct Sensor Sensor;
typedef enum
{
    ERROR_OK,
    ERROR_ERROR,
    ERROR_WARNING
} errcode;

typedef errcode (*Service_Sensor)(void *, size_t);
typedef errcode (*Config_Sensor)(void);
typedef errcode (*Event_Subscribe)(ObserveEvent *);

typedef struct Sensor_init
{
    size_t buflen;
    uint16_t SensorIndex;
    Service_Sensor Service_Handler;
    Config_Sensor Configs_Handler;
}Sensor_init;