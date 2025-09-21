#ifndef __BEEPER_H
#define __BEEPER_H

typedef enum BEEPER_ALARM_t {
    ALARM_OK,
    ALARM_MEMMORY_ERROR,
    ALARM_SENSORS_INIT,
}BEEPER_ALARM;

void Beeper_Service();
void Set_ALARM(BEEPER_ALARM);
#endif