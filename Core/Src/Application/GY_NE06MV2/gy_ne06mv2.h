#ifndef __GY_NE06MV2_H
#define __GY_NE06MV2_H
#include "../Sensor/Sensor.h"
#include "stdint.h"

typedef struct gy_ne06mv2 gy_ne06mv2;
typedef struct gy_queue_piece gy_queue_piece;

errcode gy_init();
errcode gy_service(void * buf, size_t data_size);
errcode gy_close();

#endif