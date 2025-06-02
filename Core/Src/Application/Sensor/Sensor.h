#ifndef __SENSOR_H__
#define __SENSOR_H__
#include "stdint.h"
#include "stdio.h"
#include "observer.h"
#include "common.h"

typedef struct Sensor Sensor;

typedef errcode (*Service_Sensor)(void *, size_t);
typedef errcode (*Config_Sensor)(void);
typedef errcode (*Event_Subscribe)(Observer *, ObserveEvent *);

typedef struct Sensor_init
{
    size_t buflen;
    Sensor_type_t Type;
    Service_Sensor Service_Handler;
    Config_Sensor Configs_Handler;
    Event_Subscribe Event_Handler;
}Sensor_init;

Sensor * CreateSensor(Sensor_init *Init_data);
errcode AddSubscriber(Sensor *this, Observer *observe);
errcode RemoveSubscriber(Sensor *this);
errcode InitSensor(Sensor *S_this);
errcode ServiceSensor(Sensor *S_this);

#endif