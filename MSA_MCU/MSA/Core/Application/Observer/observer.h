#include "stdio.h"
typedef struct Observer Observer;

typedef struct ObserveEvent {
    uint16_t SensorIndex;
    void *buf;
    size_t size;
} ObserveEvent;