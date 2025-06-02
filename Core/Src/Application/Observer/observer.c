#include "observer.h"
#include <string.h>
#include "stdlib.h"
#include "gy_ne06mv2.h"
extern struct Sensor_data
{
    gy_ne06mv2_export *data;
    size_t len;
    Sensor_type_t type;
};

extern struct Observer{
    Sensor_data *Gyne06m;
    Sensor_data *MPU;
};

Observer * CreateObserve() {
    Observer *this = NULL;
    this = (Observer *)malloc(sizeof(Observer));
    if (this == NULL){
        return NULL;
    }
    this->Gyne06m = (Sensor_data *)malloc(sizeof(Sensor_data));
    if (this->Gyne06m == NULL){
        return NULL;
    }
    this->MPU = (Sensor_data *)malloc(sizeof(Sensor_data));
    if (this->MPU == NULL){
        return NULL;
    }
    this->Gyne06m->type = SENSOR_GYNE06;
    this->Gyne06m->type = SENSOR_MPU;
    return this;
} 

errcode NotifySubs(Observer *Observer, ObserveEvent *event) {
    if (event->SensorType == SENSOR_GYNE06) {
        memcpy(Observer->Gyne06m->data, event->buf, event->size);
    } else if (event->SensorType == SENSOR_MPU) {
        memcpy(Observer->MPU->data, event->buf, event->size);
    }
    return ERROR_OK;
}

errcode getSensorData(Observer *Observer, void *data, Sensor_type_t type, size_t size) {
    switch (type)
    {
    case SENSOR_GYNE06:
        memcpy(data, &Observer->Gyne06m->data, size);
        break;
    
    default:
        return ERROR_ERROR;
    }
    return ERROR_OK;
}