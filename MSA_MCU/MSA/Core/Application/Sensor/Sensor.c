
#include "Sensor.h"
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


errcode CreateSensor(Sensor *S_this, Sensor_init *Init_data)
{
    S_this = (Sensor *)pvPortMalloc(sizeof(Sensor));
    if (S_this == NULL)
    {
        return ERROR_ERROR;
    }
    if (Init_data == NULL)
    {
        return ERROR_ERROR;
    }
    if (Init_data->Configs_Handler == NULL || Init_data->Service_Handler == NULL)
    {
        return ERROR_ERROR;
    }
    S_this->Sensor_Index = Init_data->SensorIndex;
    S_this->bufSize = Init_data->buflen;
    S_this->new_buf = (void *)pvPortMalloc(Init_data->buflen);
    S_this->current_buf = (void *)pvPortMalloc(Init_data->buflen);
    S_this->Config_new_Sensor = Init_data->Configs_Handler;
    S_this->Service = Init_data->Service_Handler;
    return ERROR_OK;
}

errcode InitSensor(Sensor *S_this)
{
    if (S_this == NULL)
    {
        return ERROR_ERROR;
    }
    if (S_this->Config_new_Sensor == NULL)
    {
        return ERROR_ERROR;
    }
    if (S_this->Config_new_Sensor() == ERROR_ERROR)
    {
        return ERROR_ERROR;
    }
    else
    {
        return ERROR_OK;
    }
}

errcode ServiceSensor(Sensor *S_this)
{
    if (S_this == NULL)
    {
        return ERROR_ERROR;
    }
    if (S_this->Service == NULL)
    {
        return ERROR_ERROR;
    }
    if (S_this->Service(S_this->new_buf, S_this->bufSize) == ERROR_ERROR)
    {
        return ERROR_ERROR;
    }
    else
    {
        /*Тут надо подумать, что делать. У 
        всех датчиков информация может быть разного рода
        и инфомрация скорее всего с каждым прочтением будет разной 
        наверное в структуре Sensor есть необходимость хранить
        некий тип датчика, чтобы понимать, с какого рода данными мы работаем
        и в каких случаех мы решаем, что данные поменялись и подписчика 
        надо уведомить. 
        #TODO сделать качественное приняте решения по датчикам.
        пока пустышка.
        */
        if (memcmp(S_this->new_buf, S_this->current_buf, S_this->bufSize) != 0) {
            memcpy(S_this->current_buf, S_this->new_buf, S_this->bufSize);
            for(uint8_t i = 0; i < OBSERVERS_MAX; i++) {
                if (S_this->Observes_handlers[i] !=NULL) {
                    ObserveEvent local_event = {.SensorIndex = S_this->Sensor_Index, .buf = S_this->current_buf, .size = S_this->bufSize};
                    (errcode *)(S_this->Observes_handlers[i])(&local_event);
                }
            }
        } 
    }
    return ERROR_OK;
}