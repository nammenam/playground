#include "app.h"

#include <cstdio>

#include "datastructures.h"
#include "definitions.h"
#include "kalmanfilter/KalmanFilter.hpp"
#include "peripheral/port/plib_port.h"
#include "sdcard/sd_card.h"
#include "sensors/imu.h"
#include "sensors/ps.h"
#include "system/time/sys_time.h"
#include "testing/debug.hpp"
#include "time/time.h"
#include "utility/utility.hpp"

#define SDCARD_MOUNT_NAME SYS_FS_MEDIA_IDX0_MOUNT_NAME_VOLUME_IDX0
#define SDCARD_DEV_NAME SYS_FS_MEDIA_IDX0_DEVICE_NAME_VOLUME_IDX0
#define SDCARD_FILE_NAME "save_state"
#define SDCARD_FILE_NAME2 "save_state2"

APP_DATA appData;

/*******************************************************************************
 Callback Functions
    this is called when the sd card is mounted or unmounted
*******************************************************************************/

static void APP_SysFSEventHandler(SYS_FS_EVENT event, void *eventData, uintptr_t context) {
    switch (event) {
        case SYS_FS_EVENT_MOUNT:
            if (strcmp((const char *)eventData, SDCARD_MOUNT_NAME) == 0) {
                appData.events |= EVENT_MOUNTED_SD;
            }
            break;

        case SYS_FS_EVENT_UNMOUNT:
            if (strcmp((const char *)eventData, SDCARD_MOUNT_NAME) == 0) {
                appData.events &= ~EVENT_MOUNTED_SD;
            }

            break;

        case SYS_FS_EVENT_ERROR:
        default:
            break;
    }
}

void APP_WriteToSDCard(uintptr_t context) { appData.events |= EVENT_SAVE_STATE; }

void APP_ReadPressure(uintptr_t context) { appData.events |= EVENT_PRESSURE_READ; }

void APP_Initialize(void) {
    // register the event handlers
    SYS_FS_EventHandlerSet((void const *)APP_SysFSEventHandler, (uintptr_t)NULL);
    SYS_TIME_HANDLE timerHandle = SYS_TIME_CallbackRegisterMS(APP_WriteToSDCard, (uintptr_t)NULL, 1000, SYS_TIME_PERIODIC);
    if (timerHandle == SYS_TIME_HANDLE_INVALID) {
        appData.state = APP_ERROR;
    }
    timerHandle = SYS_TIME_CallbackRegisterMS(APP_ReadPressure, (uintptr_t)NULL, 100, SYS_TIME_PERIODIC);
    if (timerHandle == SYS_TIME_HANDLE_INVALID) {
        appData.state = APP_ERROR;
    }

    kalman_init(&appData.kf, &appData.mv);
    appData.state = APP_INIT;
    appData.events = 0;
    appData.lastTime = getCurrentTimeMS();
    // only for testing
    uart_setup();
}

void APP_Tasks(void) {
    switch (appData.state) {
        case APP_INIT: {
            uint8_t who_am_i;
            if (imu_WhoAmI(&who_am_i) == 0) {
                break;
            }
            if (appData.events & EVENT_MOUNTED_SD) {
                if (SYS_FS_CurrentDriveSet(SDCARD_MOUNT_NAME) == SYS_FS_RES_FAILURE) {
                    appData.state = APP_ERROR;
                    break;
                }
                if (sdFileOpen(&appData.fileHandle, SDCARD_FILE_NAME, 0, 1) == -1) {
                    appData.state = APP_ERROR;
                    break;
                }
            } else {
                break;
            }
            imu_init();
            ps_init();
            appData.state = APP_READ_SENSORS;
            break;
        }

        case APP_LOAD_STATE: {
            // TODO temporary testing plz remove
            // Reading from file in case of system reboot. Consider moving to
            // own APP_CASE
            // TODO Logic bby
            //  SensorData data2;
            //
            //  if (sdReadSensorDataFromCard(&appData.fileHandle,
            //  SDCARD_FILE_NAME, &data2) != 1)
            //  {
            //      appData.state = APP_ERROR;
            //      break;
            //  }
            //
            //  sdWrite(&appData.fileHandle, SDCARD_FILE_NAME2, &data2,
            //  sizeof(data2));
            //  //Flushing and closing file to avoid data loss. Should only
            //  occur when we know
            //  //the data we have written so far is complete
            //  //TODO
            //  if (sdFlush(&appData.fileHandle) != 1)
            //  {
            //      appData.state = APP_ERROR;
            //      break;
            //  }
            //  appData.state = APP_IDLE;
            break;
        }

        case APP_SAVE_STATE: {
            char buffer[100];
            sprintf(buffer, "q1: %f, q2: %f, q3: %f, q4: %f, vx: %f, vy: %f, vz: %f, alt: %f\n", appData.kf.state.orientation.a, appData.kf.state.orientation.b, appData.kf.state.orientation.c,
                    appData.kf.state.orientation.d, appData.kf.state.velocity.x, appData.kf.state.velocity.y, appData.kf.state.velocity.z, appData.kf.state.altitude);
            if (sdWrite(&appData.fileHandle, SDCARD_FILE_NAME, buffer, sizeof(buffer)) == -1) {
                int error = SYS_FS_Error();
                uart_printf("fs error: %d\r\n", error);
                appData.state = APP_ERROR;
                break;
            }

            if (sdFlush(&appData.fileHandle) == -1) {
                appData.state = APP_ERROR;
                break;
            }
            appData.events &= ~EVENT_SAVE_STATE;
            appData.state = APP_READ_SENSORS;
            break;
        }

        case APP_READ_SENSORS: {
            if (IMU_INT1_Get()) {
                imu_get_accel_ms2(&appData.sensorData.accel);
                appData.events |= EVENT_ACCEL_READY;
            }
            if (IMU_INT2_Get()) {
                imu_get_gyro_rps(&appData.sensorData.gyro);
                appData.events |= EVENT_GYRO_READY;
            }
            if (false) {
                appData.sensorData.magnet.x = 0;
                appData.sensorData.magnet.y = 0;
                appData.sensorData.magnet.z = 0;
                appData.events |= EVENT_MAGN_READY;
            }
            if (appData.events & EVENT_PRESSURE_READ) {
                ps_get_measurement(&appData.sensorData.pressure, &appData.sensorData.temp);
                appData.events &= ~EVENT_PRESSURE_READ;
                appData.events |= EVENT_PRESSURE_READY;
            }
            if (appData.events & EVENT_SAVE_STATE) {
                appData.state = APP_SAVE_STATE;
                break;
            }
            if (appData.events & (EVENT_ACCEL_READY | EVENT_GYRO_READY | EVENT_PRESSURE_READY)) {
                appData.state = APP_COMPUTE;
                appData.events &= ~(EVENT_ACCEL_READY | EVENT_GYRO_READY | EVENT_PRESSURE_READY);
                break;
            }
            break;
        }

        case APP_COMPUTE: {
            float dt = getCurrentTimeUS() - appData.lastTime;
            dt = dt / 1000000.0;
            if (dt < 0) {
                dt = 0.0007;  // it is usually around 0.0007
            }
            appData.lastTime = getCurrentTimeUS();
            kalman_nextState(&appData.kf, &appData.mv, &appData.sensorData, dt);
            if (appData.events & EVENT_SAVE_STATE) {
                appData.state = APP_SAVE_STATE;
                break;
            }
            appData.state = APP_READ_SENSORS;
            break;
        }

        case APP_IDLE: {
            if (getCurrentTimeMS() - appData.lastTime > 1000) {
                BATT_LED_Toggle();
                appData.lastTime = getCurrentTimeMS();
            }
            break;
        }

        case APP_TEST: {
            uint16_t delta_time = getCurrentTimeMS() - appData.lastTime;
            if (delta_time > 100) {
                ps_get_measurement(&appData.sensorData.pressure, &appData.sensorData.temp);
                uart_printf("Pressure: %f, Temp: %f |\r", appData.sensorData.pressure, appData.sensorData.temp);
                appData.lastTime = getCurrentTimeMS();
            }

            break;
        }

        case APP_ERROR:
        default: {
            ERR_LED_Set();
            appData.state = APP_INIT;
            break;
        }
    }
}
