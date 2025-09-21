#include "observer.h"
#include <string.h>
#include "stdlib.h"
#include "gy_ne06mv2.h"
extern struct Sensor_data
{
    void *data;
    size_t len;
    Sensor_type_t type;
};

extern struct Observer{
    Sensor_data *Gyne06m;
    Sensor_data *MPU;
    Sensor_data *MS5611;
};

Observer * CreateObserve() {
    Observer *this = NULL;
    this = (Observer *)malloc(sizeof(Observer));
    if (this == NULL){
        return NULL;
    }
    this->Gyne06m = (Sensor_data *)malloc(sizeof(Sensor_data));
    this->Gyne06m->data = (gy_ne06mv2_export*)malloc((56));
    if (this->Gyne06m == NULL){
        return NULL;
    }
    this->Gyne06m->len = 56;
    this->MPU = (Sensor_data *)malloc(sizeof(Sensor_data));
    this->MPU->data = malloc((56));
    if (this->MPU == NULL){
        return NULL;
    }
    this->MPU->len = 56;

    this->MS5611 = (Sensor_data *)malloc(sizeof(Sensor_data));
    this->MS5611->data = malloc((56));
    if (this->MS5611 == NULL){
        return NULL;
    }
    this->MS5611->len = 56;

    this->Gyne06m->type = SENSOR_GYNE06;
    this->MPU->type = SENSOR_MPU;
    this->MS5611->type = SENSOR_MS5611;
    return this;
} 

errcode NotifySubs(Observer *Observer, ObserveEvent *event) {
    if (event->SensorType == SENSOR_GYNE06) {
        memcpy(Observer->Gyne06m->data, event->buf, event->size);
    } else if (event->SensorType == SENSOR_MPU) {
        memcpy(Observer->MPU->data, event->buf, event->size);
    } else if (event->SensorType == SENSOR_MS5611) {
        memcpy(Observer->MS5611->data, event->buf, event->size);
    }
    return ERROR_OK;
}

errcode getSensorData(Observer *Observer, void *data, Sensor_type_t type, size_t size) {
    switch (type)
    {
    case SENSOR_GYNE06:
        memcpy(data, Observer->Gyne06m->data, size);
        break;
    case SENSOR_MPU:
        memcpy(data, Observer->MPU->data, size);
        break;
    case SENSOR_MS5611:
        memcpy(data, Observer->MPU->data, size);
        break;
    default:
        return ERROR_ERROR;
    }
    return ERROR_OK;
}