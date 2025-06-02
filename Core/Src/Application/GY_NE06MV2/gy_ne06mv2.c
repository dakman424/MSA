#include "gy_ne06mv2.h"
#include "stdlib.h"
#include <string.h>
#include "stm32f4xx_hal.h"

extern UART_HandleTypeDef huart1;
/*

Пример сообщения

$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
$GPGGA,104534 .000,7791.0381,N,06727.4434,E,1,08,0.9,510.4,M,43.9,M,,*47

$GPGGA,HHMMSS.SSS,latitude,N,longitude,E,FQ,NOS,HDP,altitude,M,height,M,,checksum data
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

const char line_label[6][6] = {{"GPGGA"},
                               {"GPGSA"},
                               {"GPGSV"},
                               {"GPGLL"},
                               {"GPRMC"},
                               {"GPVTG"}};
typedef struct latitude
{
  char Side;
  float degrees;
  float minutes;
} latitude_t;

typedef struct longitude
{
  char Side;
  float degrees;
  float minutes;
} longitude_t;

typedef enum IsVisble {
  SPUTNIK_NOT_DETECTED = 0,
  SPUTNIK_DETECTED = 1
}IsVisble_t;

struct gy_ne06mv2
{
  IsVisble_t Sputnik;
  uint16_t hours;
  uint16_t minutes;
  longitude_t longitude;
  latitude_t latitude;
};

extern struct gy_queue_piece
{
  gy_queue_piece *PreviusFrame;
  char buffer[400];
  uint8_t isfull;
  gy_queue_piece *NextFrame;
};

typedef struct gy_ring_buf {
  char *start;
  char *current;
  char *end;
  char buffer[500];
}gy_ring_buf_t;

gy_ne06mv2 *sensor_gy_ne06mv2 = NULL;

char InputBuf[70];
char preamble[6];
uint16_t Offset = 0;

gy_queue_piece *StartQue = NULL;
unsigned int Quebegin = 0;
gy_ring_buf_t *Ring_Buf = NULL;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart == &huart1)
  {
    HAL_UART_Receive_IT(&huart1, Ring_Buf->current, 1);
  }
  ++Ring_Buf->current;
  if(Ring_Buf->current == Ring_Buf->end) {
    Ring_Buf->current = Ring_Buf->start;
  }
}

errcode gy_init()
{
  if (sensor_gy_ne06mv2 != NULL)
  {
    return ERROR_ERROR;
  }
  sensor_gy_ne06mv2 = (gy_ne06mv2 *)pvPortMalloc(sizeof(gy_ne06mv2));
  memset(sensor_gy_ne06mv2, 0, sizeof(gy_ne06mv2));
  // StartQue = (gy_queue_piece *)pvPortMalloc(sizeof(gy_queue_piece));
  // memset(StartQue, 0, sizeof(gy_queue_piece));
  // Quebegin = StartQue;
  Ring_Buf = (gy_ring_buf_t *)pvPortMalloc(sizeof(gy_ring_buf_t));
  Ring_Buf->start = Ring_Buf->buffer;
  Ring_Buf->end = &Ring_Buf->buffer[500];
  Ring_Buf->current = Ring_Buf->start; 
  if (HAL_UART_Receive_IT(&huart1, Ring_Buf->current, 1) != HAL_OK)
  {
    return ERROR_ERROR;
  }

  return ERROR_OK;
}
int x;
errcode gy_service(void *buf, size_t data_size)
{
  static uint16_t offset = 0;
  static char buf_t[100];
  static gy_ne06mv2_export data_export;
    if ((memcmp((Ring_Buf->start + offset), line_label, 5))){
      ++offset;
      if (offset > 500) {
        offset = 0;
      }
      return;
    } 
    if ((offset + 100) > 500){
      uint16_t x = (offset + 100) - 500;
      uint16_t y =  500 - offset;
      memcpy( buf_t, (Ring_Buf->start + offset), y);
      memcpy((buf_t + y), (Ring_Buf->start),  x);
      offset = 0;
    }else {
      memcpy( buf_t, (Ring_Buf->start + offset), 100);
      // memset(Ring_Buf->start, 0, 500);
      offset = 0;
    }
    if(sensor_gy_ne06mv2 == NULL) {
      return ERROR_ERROR;
    } else {
      uint16_t Offset_parser = 0;
      while(*(buf_t + Offset_parser) != '\r' && *(buf_t + Offset_parser + 1) != '\n') {
       ++Offset_parser;
      }
      if (Offset_parser <= 40) {
        memset(sensor_gy_ne06mv2, 0, sizeof(gy_ne06mv2));
        memset(buf, 0, sizeof(gy_ne06mv2));
        memset(&data_export, 0, sizeof(gy_ne06mv2_export));
        data_export.Sputnik = sensor_gy_ne06mv2->Sputnik;
        memcpy (buf, &data_export, data_size);
      } else {
        memset(buf, 0, sizeof(gy_ne06mv2));
        memset(&data_export, 0, sizeof(gy_ne06mv2_export));
        sensor_gy_ne06mv2->Sputnik = SPUTNIK_DETECTED;
        sensor_gy_ne06mv2->hours = (*(buf_t + 6) - '0') * 10 + ((*(buf_t + 7) - '0'));
        sensor_gy_ne06mv2->minutes = (*(buf_t + 8) - '0') * 10 + ((*(buf_t + 9) - '0'));
        sensor_gy_ne06mv2->latitude.degrees = (*(buf_t + 16) - '0') * 10 + ((*(buf_t + 17) - '0'));
        sensor_gy_ne06mv2->latitude.minutes = (*(buf_t + 18) - '0') * 10 + ((*(buf_t + 19) - '0')) + ((float)((*(buf_t + 21) - '0')))/10 + ((float)((*(buf_t + 22) - '0')))/100  + ((float)((*(buf_t + 23) - '0')))/1000 + ((float)((*(buf_t + 24) - '0')))/10000 + ((float)((*(buf_t + 25) - '0')))/100000;
        sensor_gy_ne06mv2->latitude.Side = *(buf_t + 27);
        sensor_gy_ne06mv2->longitude.degrees = (*(buf_t + 30) - '0') * 10 + ((*(buf_t + 31) - '0'));
        sensor_gy_ne06mv2->longitude.minutes = (*(buf_t + 32) - '0') * 10 + ((*(buf_t + 33) - '0')) + ((float)((*(buf_t + 35) - '0')))/10 + ((float)((*(buf_t + 36) - '0')))/100  + ((float)((*(buf_t + 37) - '0')))/1000 + ((float)((*(buf_t + 38) - '0')))/10000  + ((float)((*(buf_t + 39) - '0')))/100000;
        sensor_gy_ne06mv2->longitude.Side = *(buf_t + 41);

        data_export.Sputnik = sensor_gy_ne06mv2->Sputnik;
        data_export.hours = sensor_gy_ne06mv2->hours;
        data_export.minutes = sensor_gy_ne06mv2->minutes;
        data_export.degrees_lat = sensor_gy_ne06mv2->latitude.degrees;
        data_export.minutes_lat = sensor_gy_ne06mv2->latitude.minutes;
        data_export.Side_lat = sensor_gy_ne06mv2->latitude.Side;
        data_export.degrees_long = sensor_gy_ne06mv2->longitude.degrees;
        data_export.minutes_long = sensor_gy_ne06mv2->longitude.minutes;
        data_export.Side_long = sensor_gy_ne06mv2->longitude.Side;
        memcpy (buf, &data_export, 32);
        return ERROR_OK;
      }
    } 
}
errcode gy_close();