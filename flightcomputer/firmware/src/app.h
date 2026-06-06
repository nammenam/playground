#pragma once
#ifndef _APP_H
#define _APP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "datastructures.h"
#include "kalmanfilter/KalmanFilter.hpp"
#include "system/fs/sys_fs.h"

typedef enum {
    EVENT_NONE = 0,
    EVENT_SAVE_STATE = 1,
    EVENT_MOUNTED_SD = 1 << 1,
    EVENT_PRESSURE_READY = 1 << 2,
    EVENT_GYRO_READY = 1 << 3,
    EVENT_ACCEL_READY = 1 << 4,
    EVENT_MAGN_READY = 1 << 5,
    EVENT_ERROR = 1 << 6,
    EVENT_PRESSURE_READ = 1 << 7,
} EVENT;

typedef enum { APP_INIT = 0, APP_SAVE_STATE, APP_LOAD_STATE, APP_COMPUTE, APP_READ_SENSORS, APP_IDLE, APP_ERROR, APP_SHUTDOWN, APP_TEST } APP_STATES;

typedef struct {
    APP_STATES state;
    volatile uint8_t events;
    SYS_FS_HANDLE fileHandle;
    KalmanFilter_t kf;
    MatrixVariables_t mv;
    SensorData sensorData;
    uint64_t lastTime;
} APP_DATA;

void APP_Initialize();

void APP_Tasks();

#ifdef __cplusplus
}
#endif
#endif
