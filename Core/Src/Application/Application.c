#include "gy_ne06mv2.h"
#include "observer.h"
#include "terminal_adoptation.h"
#include "Sensor.h"
#include "cmsis_os.h"
#include "fatfs.h"
#include "mpu6050.h"

static Observer *Sensor_wiewer = NULL;
static Sensor *Sensor_Gyne06 = NULL;
static Sensor *Sensor_MPU = NULL;
osThreadId SensorService;
osThreadId Terminal;
osThreadId Logger_tusk;

FATFS fs; // Файловая система
FIL fil; // Файл
BYTE work[512]; // Рабочий буфер

errcode getObserveData_GENE06(void *data, size_t size);
errcode getObserveData_MPU(void *data, size_t size);

void Service(void) {
    while (1)
    {
        if (Sensor_Gyne06 != NULL) {
            ServiceSensor(Sensor_Gyne06);
        }
        if (Sensor_MPU != NULL) {
            ServiceSensor(Sensor_MPU);
        }
        osDelay(1);
    }
    
}

void Micro_Terminal_Service(void) {
    uTerminal_init();
    while (1)
    {   
        uTerminal_Service();
        osDelay(1);
    }
    
}

    // if (f_open(&fil, "0:/test.txt", FA_WRITE | FA_CREATE_ALWAYS) == FR_OK) {
    //     UINT bytes_written;
    //     const char* data = "Hello, FatFs!";
    //     f_write(&fil, data, strlen(data), &bytes_written);
    //     f_close(&fil);
    // }

void Data_log_Service(void) {
    void *buf;
    size_t size;
    char chrTmp[2];
    while(1) {
        if (f_open(&fil, "0:/Data", FA_WRITE) == FR_OK) {
            f_lseek(&fil, f_size(&fil));
            if (getObserveData_GENE06(&buf, sizeof(gy_ne06mv2_export)) == ERROR_OK) { 
                f_write(&fil, "GY ", 3, &size);
                f_write(&fil, buf, sizeof(gy_ne06mv2_export), &size);
                f_write(&fil, "\n\r", 2, &size);
            }
            if (getObserveData_MPU(&buf, 32) == ERROR_OK) { 
                f_write(&fil, "MPU ", 4, &size);
                f_write(&fil, buf, 32, &size);
                f_write(&fil, "\n\r", 2, &size);
            }
            f_close(&fil);
        }  else {
            f_close(&fil);
        }

        osDelay(100);
    }
   

}

errcode Application_Init(void) {


    if (f_mount(&fs, "0:", 1) != FR_OK) {
        // f_mkfs("0:", FS_FAT32, 0, work, sizeof(work))
    // Ошибка монтирования
        if(f_mount(&fs, "0:", 0) != FR_OK) {
            return ERROR_ERROR;
        }
    }
    if (f_open(&fil, "0:/Data", FA_CREATE_ALWAYS) == FR_OK) {
        f_close(&fil);
    } else {
        if (f_open(&fil, "0:/Data", FA__WRITTEN) == FR_OK) {
            f_close(&fil);
        } 
    }

    Sensor_wiewer = CreateObserve();
    if (Sensor_wiewer == NULL) {
        return ERROR_ERROR;
    }
    Sensor_init tmpdata = {.Type = SENSOR_GYNE06, .buflen = 32, .Configs_Handler = gy_init, .Service_Handler = gy_service, .Event_Handler =  NotifySubs};
    Sensor_Gyne06 = CreateSensor(&tmpdata);
    if ( Sensor_Gyne06 == NULL) {
        return ERROR_ERROR;
    }

    if (InitSensor(Sensor_Gyne06) != ERROR_OK) {
        return ERROR_ERROR;
    }
    if (AddSubscriber(Sensor_Gyne06, Sensor_wiewer) != ERROR_OK) {
        return ERROR_ERROR;
    }

    tmpdata.buflen = 16;
    tmpdata.Type = SENSOR_MPU;
    tmpdata.Configs_Handler = mpu_init;
    tmpdata.Event_Handler = NotifySubs;
    tmpdata.Service_Handler = mpu_service;

    Sensor_MPU = CreateSensor(&tmpdata);
    if (Sensor_MPU == NULL) {
        return ERROR_ERROR;
    }

    if (InitSensor(Sensor_MPU) != ERROR_OK) {
        return ERROR_ERROR;
    }

    if (AddSubscriber(Sensor_MPU, Sensor_wiewer) != ERROR_OK) {
        return ERROR_ERROR;
    }


    osThreadDef(SensorTusk, Service, osPriorityNormal, 0, 512);
    SensorService = osThreadCreate(osThread(SensorTusk), NULL);

    osThreadDef(MicroTerminal, Micro_Terminal_Service, osPriorityNormal, 0, 512);
    Terminal = osThreadCreate(osThread(MicroTerminal), NULL);

    osThreadDef(Logger, Data_log_Service, osPriorityAboveNormal, 0, 512);
    Logger_tusk = osThreadCreate(osThread(Logger), NULL);

    return ERROR_OK;
}

errcode getObserveData_GENE06(void *data, size_t size) {
    return getSensorData(&Sensor_wiewer, data , SENSOR_GYNE06, size);
}
errcode getObserveData_MPU(void *data, size_t size) {
    return getSensorData(&Sensor_wiewer, data , SENSOR_MPU, size);
}