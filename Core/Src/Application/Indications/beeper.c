#include "beeper.h"
#include "stdint.h"
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"

BEEPER_ALARM Beeper_state = ALARM_OK;

void Set_ALARM(BEEPER_ALARM alarm) {
    Beeper_state = alarm;
}


void Normal_work(){
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
      osDelay(100);
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
      osDelay(1000);
}

void Alarm_SD_card(){
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
      osDelay(500);
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
      osDelay(100);
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
      osDelay(50);
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
      osDelay(500);
}

void Beeper_Service() {
    while (1)
    {
        switch (Beeper_state)
        {
        case ALARM_OK:
            Normal_work();
            break;
        case ALARM_SENSORS_INIT:
        case ALARM_MEMMORY_ERROR:
            Alarm_SD_card();
            break;
        default:
            break;
        }
    }
}