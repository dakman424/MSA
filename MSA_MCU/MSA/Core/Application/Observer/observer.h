#include "stdio.h"
extern enum errcode;

typedef struct Observer Observer;

typedef struct ObserveEvent {
    uint16_t SensorIndex;
    void *buf;
    size_t size;
} ObserveEvent;

// errcode observe_notify (ObserveEvent *event);