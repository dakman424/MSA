
#include "Sensor.h"
#include "string.h"
#define SENSOR_MAGICNUMBER 0x18C529F2
#define OBSERVERS_MAX 3


typedef struct ObserveList{
    Observer *begin;
    Observer *current;
    Observer *next;
}ObserveList;

extern struct Sensor
{
    uint16_t MagicNumber;
    Sensor_type_t type;
    size_t bufSize;
    void *current_buf;
    void *new_buf;
    Config_Sensor Config_new_Sensor;
    Service_Sensor Service;
    ObserveEvent *Event;
    Event_Subscribe Observes_handlers;
    Observer *Subscriber;
};


Sensor * CreateSensor(Sensor_init *Init_data)
{
    Sensor *S_this = NULL;
    S_this = (Sensor *)pvPortMalloc(sizeof(Sensor));
    memset(S_this, 0, sizeof(Sensor));
    if (S_this == NULL)
    {
        return NULL;
    }
    if (Init_data == NULL)
    {
        return NULL;
    }
    if (Init_data->Configs_Handler == NULL || Init_data->Service_Handler == NULL)
    {
        return NULL;
    }
    S_this->type = Init_data->Type;
    S_this->bufSize = Init_data->buflen;
    S_this->new_buf = (void *)pvPortMalloc(Init_data->buflen);
    S_this->current_buf = (void *)pvPortMalloc(Init_data->buflen);
    S_this->Config_new_Sensor = Init_data->Configs_Handler;
    S_this->Service = Init_data->Service_Handler;
    S_this->Observes_handlers = Init_data->Event_Handler;

    return S_this;
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
        #TODO сделать качественное принятbtе решения по датчикам.
        пока пустышка.
        */
        if (memcmp(S_this->new_buf, S_this->current_buf, S_this->bufSize) != 0) {
            memcpy(S_this->current_buf, S_this->new_buf, S_this->bufSize);
            // for(uint8_t i = 0; i < OBSERVERS_MAX; i++) {
                if (S_this->Observes_handlers !=NULL) {
                    ObserveEvent local_event = {.SensorType = S_this->type, .buf = S_this->current_buf, .size = S_this->bufSize};
                    return (S_this->Observes_handlers)(S_this->Subscriber, &local_event);
                }
            // }
        } 
    }
    return ERROR_OK;
}

errcode AddSubscriber(Sensor *this, Observer *observe) {
    if(this == NULL) {
        return ERROR_ERROR;
    }
    if (observe == NULL) {
        return ERROR_ERROR;
    }
    if(this->Subscriber == NULL) {
        this->Subscriber = observe;
        return ERROR_OK;
    } else {
        /*Подписчик значит уже есть*/
       return ERROR_ERROR;
    }
}

errcode RemoveSubscriber(Sensor *this) {
    if(this == NULL) {
        return ERROR_ERROR;
    }
    
    if(this->Subscriber == NULL) {
        return ERROR_ERROR;
    } else {
        this->Subscriber = NULL;
        return ERROR_OK;
    }
    return ERROR_ERROR;
}