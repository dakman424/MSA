#include "gy_ne06mv2.h"
#include "usart.h"

extern UART_HandleTypeDef huart1;
/*

Пример сообщения

$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
где:
GGA – NMEA Заговолок
123519 –UTC время 12:35:19
4807.038, N – Широта, 48 градусов 7.038 минуты северной широты
01131.000, Е – Долгота, 11 градусов 31.000 минуты восточной долготы
1 - тип решение, StandAlone решение
0 – нет решения,
1 – StandAlone,
2 – DGPS,
3 – PPS,
4 – фиксированный RTK,
5 – не фиксированный RTK,
6 – использование данных инерциальных систем,
7 – ручной режим,
8 – режим симуляции
08 – количество используемых спутников
0.9 – геометрический фактор, HDOP
545.4, М – высота над уровнем моря в метрах
46.9, М – высота геоида над эллипсоидом WGS 84
[пустое поле] – время прошедшее с момента получения последней DGPS
поправки. Заполняется при активизации DGPS режима
[пустое поле] – идентификационный номер базовой станции. Заполняется при
активизации DGPS режима*/

const char line_label[6][6] = {{"$GPGGA"},
                               {"$GPGSA"},
                               {"$GPGSV"},
                               {"$GPGLL"},
                               {"$GPRMC"},
                               {"$GPVTG"}};
typedef struct latitude {
    char Side;
    float degrees;
    float minutes;
} latitude_t;

typedef struct longitude{
    char Side;
    float degrees;
    float minutes;
}longitude_t;

struct gy_ne06mv2
{
    uint16_t hours;
    uint16_t minutes;
    longitude_t longitude;
    latitude_t  latitude;
    char Input;
    char buffer[50];
};

gy_ne06mv2 *sensor_gy_ne06mv2 = NULL;
char InputBuf[60];

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    static uint16_t offset = 0;
    if (sensor_gy_ne06mv2 == NULL){
        return;
    }
    if(huart == &huart1) {
      HAL_UART_Receive_IT (&huart1, &sensor_gy_ne06mv2->Input, 1);
    //   if(sensor_gy_ne06mv2->Input = '')
      // while((char)symbol != '\n'){
        *(sensor_gy_ne06mv2->buffer + offset) = sensor_gy_ne06mv2->Input;
        ++offset;
        if(offset > 49) {
          memset(&sensor_gy_ne06mv2->buffer, 0, 49);
        }

        if(offset > 49) {
          offset = 0;
        }
        // HAL_UART_Receive_IT (&huart1, &symbol, 1);
      // }
    }
}

errcode gy_init() {
    if (sensor_gy_ne06mv2 != NULL) {
        return ERROR_ERROR;
    }
    sensor_gy_ne06mv2 = (gy_ne06mv2 *)pvPortMalloc(sizeof(gy_ne06mv2));
    if (HAL_UART_Receive_IT(&huart1, &sensor_gy_ne06mv2->Input, 1) != HAL_OK) {
        return ERROR_ERROR;
    }
    return ERROR_OK;
}
errcode gy_service(void *buf, size_t data_size) {
    
}
errcode gy_close();