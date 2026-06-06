#pragma once
#ifndef KALMANFILTER_H
#define KALMANFILTER_H

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

#define PI 3.14159265358979323846

/* atmospheric model constants */
#define P0 1013.25f    // atmospheric pressure at sea level
#define T0 288.15f     // temperature at sea level
#define L0 0.0065f     // temperature lapse rate
#define R0 8.31447f    // universal gas constant
#define M0 0.0289644f  // molar mass of dry air
#define G0 9.80665f    // gravitational acceleration at sea level
#define H0 0.0f        // altitude at sea level

/*  magnetic field model reference
    TODO: update with actual values *
    (simply record the values from the magnetometer when the rocket is on the pad/stationary)
*/
#define MAGREF_X 1.0f
#define MAGREF_Y 0.0f
#define MAGREF_Z 0.0f

/********************************************************************
    Data Structures
********************************************************************/

typedef struct {
    Quaternion orientation;
    Vec3 velocity;
    float altitude;
} StateVector_t;

// This struct is used to store the sensor data, this data need to be rememebered for the next iteration
typedef struct {
    StateVector_t state;                            // existing state vector (orientation, velocity, altitude)
    float P[STATE_VECTOR_SIZE][STATE_VECTOR_SIZE];  // State covariance matrix
} KalmanFilter_t;

// This struct contains intermediate variables used in the Kalman filter
typedef struct {
    float F[STATE_VECTOR_SIZE][STATE_VECTOR_SIZE];  // Jacobian of the state transition function  f()
    float H[MEASURE_SIZE][STATE_VECTOR_SIZE];       // Jacobian of the measurement function       h()
    float K[STATE_VECTOR_SIZE][MEASURE_SIZE];       // Kalman gain
    float S[MEASURE_SIZE][MEASURE_SIZE];            // Innovation covariance
} MatrixVariables_t;

/********************************************************************
    Function Prototypes
********************************************************************/

void kalman_nextState(KalmanFilter_t* kf, MatrixVariables_t* mv, const SensorData* sensorData, const float dt);

void kalman_init(KalmanFilter_t* kf, MatrixVariables_t* mv);

void kalman_predictState(KalmanFilter_t* kf, const SensorData* sensorData, const float dt);

void kalman_predictCovariance(KalmanFilter_t* kf, MatrixVariables_t* mv);

void kalman_innovation(const KalmanFilter_t* state, const SensorData* sensorData, const float dt, float result[MEASURE_SIZE]);

void kalman_innovationCovariance(KalmanFilter_t* kf, MatrixVariables_t* mv);

void kalman_computeMeasurementJacobian(KalmanFilter_t* kf, MatrixVariables_t* mv, const float dt);

void kalman_computeStateTransitionJacobian(KalmanFilter_t* kf, MatrixVariables_t* mv, const SensorData* sensorData, const float dt);

void kalman_computeKalmanGain(KalmanFilter_t* kf, MatrixVariables_t* mv);

void kalman_updateStateEstimate(KalmanFilter_t* kf, MatrixVariables_t* mv, const float innovation[MEASURE_SIZE]);

void kalman_updateCovarianceEstimate(KalmanFilter_t* kf, MatrixVariables_t* mv);

void kalman_preLiftoffCalibration(KalmanFilter_t* kf);

#ifdef __cplusplus
}
#endif

#endif  // KALMANFILTER_H
