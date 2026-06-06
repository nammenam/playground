/******************************************************************************
    Helper functions
******************************************************************************/
#include "utility.hpp"
#include <math.h>



void iirfilter_vec3(const Vec3* data, Vec3* result, float alpha)
{
    result->x = result->x * alpha + data->x * (1-alpha);
    result->y = result->y * alpha + data->y * (1-alpha);
    result->z = result->z * alpha + data->z * (1-alpha);
}

void quaternionmul(const Quaternion* q1, const Quaternion* q2, Quaternion* result) 
{
    // Calculate potential components of the resulting quaternion
    float a = q1->a * q2->a - q1->b * q2->b - q1->c * q2->c - q1->d * q2->d;
    float b = q1->a * q2->b + q1->b * q2->a + q1->c * q2->d - q1->d * q2->c;
    float c = q1->a * q2->c - q1->b * q2->d + q1->c * q2->a + q1->d * q2->b;
    float d = q1->a * q2->d + q1->b * q2->c - q1->c * q2->b + q1->d * q2->a;
    // Assign the calculated values to the result quaternion
    result->a = a;
    result->b = b;
    result->c = c;
    result->d = d;
}

void quaternion_normalize(Quaternion* q)
{
    float norm = sqrt(q->a * q->a + q->b * q->b + q->c * q->c + q->d * q->d);
    q->a /= norm;
    q->b /= norm;
    q->c /= norm;
    q->d /= norm;
}

void matmul8x8x8(
    const float A[STATE_VECTOR_SIZE][STATE_VECTOR_SIZE], 
    const float B[STATE_VECTOR_SIZE][STATE_VECTOR_SIZE], 
    float       result[STATE_VECTOR_SIZE][STATE_VECTOR_SIZE])
{
    for (int i = 0; i < STATE_VECTOR_SIZE; ++i) {
        for (int j = 0; j < STATE_VECTOR_SIZE; ++j) {
            result[i][j] = 0;
            for (int k = 0; k < STATE_VECTOR_SIZE; ++k) 
            {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void matmul8x8x4(
    const float A[STATE_VECTOR_SIZE][STATE_VECTOR_SIZE],
    const float B[STATE_VECTOR_SIZE][MEASURE_SIZE],
    float       out[STATE_VECTOR_SIZE][MEASURE_SIZE])
{
    for (int i = 0; i < STATE_VECTOR_SIZE; ++i) {
        for (int j = 0; j < MEASURE_SIZE; ++j) {
            out[i][j] = 0;
            for (int k = 0; k < STATE_VECTOR_SIZE; ++k) 
            {
                out[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void matmul8x4x4(
    const float A[STATE_VECTOR_SIZE][MEASURE_SIZE],
    const float B[MEASURE_SIZE][MEASURE_SIZE],
    float       out[STATE_VECTOR_SIZE][MEASURE_SIZE])
{
    for (int i = 0; i < STATE_VECTOR_SIZE; ++i) {
        for (int j = 0; j < MEASURE_SIZE; ++j) {
            out[i][j] = 0;
            for (int k = 0; k < MEASURE_SIZE; ++k) 
            {
                out[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void matmul8x4x1(
    const float A[STATE_VECTOR_SIZE][MEASURE_SIZE],
    const float B[MEASURE_SIZE],
    float out[STATE_VECTOR_SIZE])
{
    for (int i = 0; i < STATE_VECTOR_SIZE; ++i) {
        out[i] = 0;
        for (int j = 0; j < MEASURE_SIZE; ++j) 
        {
            out[i] += A[i][j] * B[j];
        }
    }
}

void matmul8x4x8(
    const float A[][MEASURE_SIZE],
    const float B[][STATE_VECTOR_SIZE],
    float out[][STATE_VECTOR_SIZE])
{
    for (int i = 0; i < STATE_VECTOR_SIZE; ++i) {
        for (int j = 0; j < STATE_VECTOR_SIZE; ++j) {
            out[i][j] = 0;
            for (int k = 0; k < MEASURE_SIZE; ++k) 
            {
                out[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void matmul4x8x8(
    const float A[][STATE_VECTOR_SIZE],
    const float B[][STATE_VECTOR_SIZE],
    float out[][STATE_VECTOR_SIZE])
{
    for (int i = 0; i < MEASURE_SIZE; ++i) {
        for (int j = 0; j < STATE_VECTOR_SIZE; ++j) {
            out[i][j] = 0;
            for (int k = 0; k < STATE_VECTOR_SIZE; ++k) 
            {
                out[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void matmul4x8x4(
    const float A[][STATE_VECTOR_SIZE],
    const float B[][MEASURE_SIZE],
    float out[][MEASURE_SIZE])
{
    for (int i = 0; i < MEASURE_SIZE; ++i) {
        for (int j = 0; j < MEASURE_SIZE; ++j) {
            out[i][j] = 0;
            for (int k = 0; k < STATE_VECTOR_SIZE; ++k) 
            {
                out[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}
 
void mattranspose8x8(const float A[][STATE_VECTOR_SIZE], float result[][STATE_VECTOR_SIZE])
{
    for (int i = 0; i < STATE_VECTOR_SIZE; ++i) {
        for (int j = 0; j < STATE_VECTOR_SIZE; ++j) 
        {
            result[i][j] = A[j][i];
        }
    }
}

void matinv4x4(const float src[4][4], float dst[4][4]) 
{
    float det, invDet;

    // Calculate the determinant of the matrix
    det = src[0][0] * (src[1][1] * (src[2][2] * src[3][3] - src[3][2] * src[2][3]) -
                       src[2][1] * (src[1][2] * src[3][3] - src[1][3] * src[3][2]) +
                       src[3][1] * (src[1][2] * src[2][3] - src[1][3] * src[2][2])) -
          src[1][0] * (src[0][1] * (src[2][2] * src[3][3] - src[3][2] * src[2][3]) -
                       src[2][1] * (src[0][2] * src[3][3] - src[0][3] * src[3][2]) +
                       src[3][1] * (src[0][2] * src[2][3] - src[0][3] * src[2][2])) +
          src[2][0] * (src[0][1] * (src[1][2] * src[3][3] - src[1][3] * src[3][2]) -
                       src[1][1] * (src[0][2] * src[3][3] - src[0][3] * src[3][2]) +
                       src[3][1] * (src[0][2] * src[1][3] - src[0][3] * src[1][2])) -
          src[3][0] * (src[0][1] * (src[1][2] * src[2][3] - src[1][3] * src[2][2]) -
                       src[1][1] * (src[0][2] * src[2][3] - src[0][3] * src[2][2]) +
                       src[2][1] * (src[0][2] * src[1][3] - src[0][3] * src[1][2]));

    if (det == 0)
    {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                dst[i][j] = 0;
            }
        }
        return;
    }
    // if (std::isnan(det) || std::isinf(det))
    // {
    //     // return zero matrix since determinant is large
    //     for (int i = 0; i < 4; i++) {
    //         for (int j = 0; j < 4; j++) {
    //             dst[i][j] = 0;
    //         }
    //     }
    //     return;
    // }


    invDet = 1.0 / det;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) 
        {
            float minor[3][3]; // To hold the minor matrix for (i,j)

            // Build the minor matrix for (i,j)
            for (int mi = 0, origi = 0; origi < 4; origi++)
            {
                if (origi == i) continue; // Skip row i
                
                for (int mj = 0, origj = 0; origj < 4; origj++) 
                {
                    if (origj == j) continue; // Skip column j
                    
                    minor[mi][mj] = src[origi][origj];
                    mj++;
                }
                mi++;
            }
            
            float minorDet = minor[0][0] * (minor[1][1] * minor[2][2] - minor[2][1] * minor[1][2]) -
                             minor[0][1] * (minor[1][0] * minor[2][2] - minor[1][2] * minor[2][0]) +
                             minor[0][2] * (minor[1][0] * minor[2][1] - minor[1][1] * minor[2][0]);
            
            dst[j][i] = ((i+j) % 2 == 0 ? 1 : -1) * minorDet * invDet;
        }
    }
}
