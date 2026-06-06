#pragma once
#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#include <stdint.h>


#define BUFFER_SIZE 1024



typedef struct 
{
    float x;
    float y;
    float z;
} Vec3;


typedef struct 
{
    Vec3  accel;
    Vec3  magnet;
    Vec3  gyro;
    float pressure;
    float temp;
} SensorData;


typedef struct 
{
    float a;
    float b;
    float c;
    float d;
} Quaternion;


#endif //DATASTRUCTURES_H
