#ifndef __OBSERVER_H__
#define __OBSERVER_H__
#include "stdio.h"
#include "stdint.h"
#include "common.h"

typedef struct Sensor_data Sensor_data;
typedef struct Observer Observer;

typedef struct ObserveEvent {
    Sensor_type_t SensorType;
    void *buf;
    size_t size;
} ObserveEvent;

typedef struct gy_ne06mv2_export
{
  float degrees_lat;
  float minutes_lat;
  float degrees_long;
  float minutes_long;
  uint16_t hours;
  uint16_t minutes;
  uint8_t Sputnik;
  char Side_long;
  char Side_lat;
}gy_ne06mv2_export;

errcode getSensorData(Observer *Observer, void *data, Sensor_type_t type, size_t size);
Observer * CreateObserve();
errcode NotifySubs(Observer *Observer, ObserveEvent *event);
// errcode observe_notify (ObserveEvent *event);
#endif