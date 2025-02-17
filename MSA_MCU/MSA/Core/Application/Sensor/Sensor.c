
#include "Sensor.h"
#include "stdint.h"
#include "observer.h"
#include "stdio.h"
#include "portable.h"
#define SENSOR_MAGICNUMBER 0x18C529F2
#define OBSERVERS_MAX 3


extern struct Sensor
{
    uint16_t MagicNumber;
    uint16_t Sensor_Index;
    size_t bufSize;
    void *current_buf;
    void *new_buf;
    Config_Sensor Config_new_Sensor;
    Service_Sensor Service;
    ObserveEvent *Event;
    Event_Subscribe Observes_handlers[OBSERVERS_MAX];
};

errcode CreateSensor(Sensor *S_this, Sensor_init *Init_data) {
    S_this = (Sensor *)pvPortMalloc(sizeof(Sensor));
    if (S_this == NULL) {
        return ERROR;
    }
    if (Init_data == NULL) {
        return ERROR;
    }
    if (Init_data->Configs_Handler == NULL || Init_data->Service_Handler == NULL) {
        return ERROR;
    }
    S_this->Sensor_Index  = Init_data->SensorIndex;
    S_this->bufSize = Init_data->buflen;
    S_this->new_buf = (void *)pvPortMalloc(Init_data->buflen);
    S_this->current_buf = (void *)pvPortMalloc(Init_data->buflen);
    S_this->Config_new_Sensor = Init_data->Configs_Handler;
    S_this->Service = Init_data->Service_Handler;
}