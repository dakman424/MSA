typedef struct Sensor Sensor;

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

typedef enum
{
    OK,
    ERROR,
    WARNING
} errcode;