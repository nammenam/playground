#pragma once
#ifndef SD_CARD_OPERATIONS_H
#define SD_CARD_OPERATIONS_H


#include "system/fs/sys_fs.h"
#include "kalmanfilter/KalmanFilter.hpp"




#ifdef __cplusplus
extern "C" {
#endif


// Function to mount the SD card
int sdMount();

// Function to open a file on the SD card
int sdFileOpen(SYS_FS_HANDLE* fileHandle, const char* name, int read, int write);

// Function to close a file on the SD card
int sdFileClose(SYS_FS_HANDLE* fileHandle);

// Function to write data to a file on the SD card
int sdWrite(SYS_FS_HANDLE* fileHandle, const char* name, void* data, int sizeOfData);

// Function to flush the file system's buffers to the SD card
int sdFlush(SYS_FS_HANDLE* fileHandle);

// Function to read sensor data from a file on the SD card
int sdReadSensorDataFromCard(SYS_FS_HANDLE* fileHandle, const char* name, SensorData* data);


int writePlainText(SYS_FS_HANDLE* fileHandle, const char* name, KalmanFilter_t* data);

// Function to unmount the SD card
int sdUnmount();

#ifdef __cplusplus
}
#endif


#endif // SD_CARD_OPERATIONS_H
