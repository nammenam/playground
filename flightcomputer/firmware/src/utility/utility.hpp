#pragma once

#ifndef UTILITY_HPP
#define UTILITY_HPP

#ifdef __cplusplus
extern "C" {
#endif


#include "../datastructures.h"

#ifndef STATE_VECTOR_SIZE
#define STATE_VECTOR_SIZE 8
#endif

#ifndef MEASURE_SIZE
#define MEASURE_SIZE 4
#endif


void iirfilter_vec3(const Vec3* data, Vec3* result, float alpha);

void quaternionmul(const Quaternion* q1, const Quaternion* q2, Quaternion* result);

void quaternion_normalize(Quaternion* q);


void matmul8x8x8(
    const float A[][STATE_VECTOR_SIZE],
    const float B[][STATE_VECTOR_SIZE], 
    float       out[][STATE_VECTOR_SIZE]);

void matmul8x8x4(
    const float A[][STATE_VECTOR_SIZE], 
    const float B[][MEASURE_SIZE],
    float       out[][MEASURE_SIZE]);

void matmul8x4x4(
    const float A[][MEASURE_SIZE],
    const float B[][MEASURE_SIZE],
    float       out[][MEASURE_SIZE]);

void matmul8x4x1(
    const float A[][MEASURE_SIZE],
    const float B[],
    float       out[]);

void matmul8x4x8(
    const float A[][MEASURE_SIZE],
    const float B[][STATE_VECTOR_SIZE],
    float       out[][STATE_VECTOR_SIZE]);

void matmul4x8x8(
    const float A[][STATE_VECTOR_SIZE],
    const float B[][STATE_VECTOR_SIZE],
    float       out[][STATE_VECTOR_SIZE]);

void matmul4x8x4(
    const float A[][STATE_VECTOR_SIZE],
    const float B[][MEASURE_SIZE],
    float       out[][MEASURE_SIZE]);


void mattranspose8x8(const float A[][STATE_VECTOR_SIZE], float result[][STATE_VECTOR_SIZE]);

void matinv4x4(const float A[][MEASURE_SIZE], float result[][MEASURE_SIZE]);




#ifdef __cplusplus
}
#endif


#endif //UTILITY_HPP
