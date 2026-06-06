// authors: Brage Wiseth, Felix Strand

#include "KalmanFilter.hpp"

#include <math.h>

#include "../utility/utility.hpp"

const float Q[STATE_VECTOR_SIZE][STATE_VECTOR_SIZE] = {
    {10.0, 0, 0, 0, 0, 0, 0, 0}, {0, 10.0, 0, 0, 0, 0, 0, 0},
    {0, 0, 10.0, 0, 0, 0, 0, 0}, {0, 0, 0, 10.0, 0, 0, 0, 0},
    {0, 0, 0, 0, 10.0, 0, 0, 0}, {0, 0, 0, 0, 0, 10.0, 0, 0},
    {0, 0, 0, 0, 0, 0, 10.0, 0}, {0, 0, 0, 0, 0, 0, 0, 10.0}};

const float R[MEASURE_SIZE][MEASURE_SIZE] = {
    {10.0, 0, 0, 0}, {0, 10.0, 0, 0}, {0, 0, 10.0, 0}, {0, 0, 0.0, 10.0}};

void kalman_nextState(KalmanFilter_t *kf, MatrixVariables_t *mv,
                      const SensorData *sensorData, const float dt) {
    float innovation_result[MEASURE_SIZE];
    kalman_predictState(kf, sensorData, dt);
    kalman_computeStateTransitionJacobian(kf, mv, sensorData, dt);
    kalman_predictCovariance(kf, mv);
    kalman_innovation(kf, sensorData, dt, innovation_result);
    kalman_computeMeasurementJacobian(kf, mv, dt);
    kalman_innovationCovariance(kf, mv);
    kalman_computeKalmanGain(kf, mv);
    kalman_updateStateEstimate(kf, mv, innovation_result);
    kalman_updateCovarianceEstimate(kf, mv);
    if (kf->state.altitude < 0) {
        kf->state.altitude = 0;
    }
}

void kalman_init(KalmanFilter_t *kf, MatrixVariables_t *mv) {
    Quaternion initialOrientation = {1.0, 0.0, 0.0, 0.0};
    Vec3 initialVelocity = {0.0, 0.0, 0.0};
    float initialAltitude = 120.0;

    kf->state.orientation = initialOrientation;
    kf->state.velocity = initialVelocity;
    kf->state.altitude = initialAltitude;

    for (int i = 0; i < STATE_VECTOR_SIZE; ++i) {
        for (int j = 0; j < STATE_VECTOR_SIZE; ++j) {
            kf->P[i][j] = (i == j) ? 0.0 : 0.0;
        }
    }

    for (int i = 0; i < STATE_VECTOR_SIZE; ++i) {
        for (int j = 0; j < STATE_VECTOR_SIZE; ++j) {
            mv->F[i][j] = 0.0;
        }
    }

    for (int i = 0; i < MEASURE_SIZE; ++i) {
        for (int j = 0; j < STATE_VECTOR_SIZE; ++j) {
            mv->H[i][j] = 0.0;
        }
    }

    for (int i = 0; i < MEASURE_SIZE; ++i) {
        for (int j = 0; j < MEASURE_SIZE; ++j) {
            mv->S[i][j] = 0.0;
        }
    }

    for (int i = 0; i < STATE_VECTOR_SIZE; ++i) {
        for (int j = 0; j < MEASURE_SIZE; ++j) {
            mv->K[i][j] = 0.0;
        }
    }
    mv->F[0][0] = 1.0;
    mv->F[1][1] = 1.0;
    mv->F[2][2] = 1.0;
    mv->F[3][3] = 1.0;
    mv->F[4][4] = 1.0;
    mv->F[5][5] = 1.0;
    mv->F[6][6] = 1.0;
    mv->F[7][7] = 1.0;
}

void kalman_predictState(KalmanFilter_t *kf, const SensorData *sensorData,
                         const float dt) {
    Vec3 gyro = sensorData->gyro;
    Vec3 accel = sensorData->accel;
    Quaternion *q = &kf->state.orientation;
    Vec3 *velocity = &kf->state.velocity;
    float half_dt = dt * 0.5;
    Quaternion omega = {0, gyro.x, gyro.y, gyro.z};
    Quaternion q_dot = {0, 0, 0, 0};
    quaternionmul(q, &omega, &q_dot);
    q_dot.a *= half_dt;
    q_dot.b *= half_dt;
    q_dot.c *= half_dt;
    q_dot.d *= half_dt;
    q->a += q_dot.a;
    q->b += q_dot.b;
    q->c += q_dot.c;
    q->d += q_dot.d;
    quaternion_normalize(q);

    kf->state.altitude = kf->state.altitude + velocity->z * dt;

    Quaternion q_conj = {q->a, -q->b, -q->c, -q->d};
    Quaternion a_ref = {0, accel.x, accel.y, accel.z};
    Quaternion predicted_accel;
    quaternionmul(q, &a_ref, &predicted_accel);
    quaternionmul(&predicted_accel, &q_conj, &predicted_accel);

    velocity->x += predicted_accel.b * dt;
    velocity->y += predicted_accel.c * dt;
    velocity->z += (predicted_accel.d - 9.81) * dt;
}

void kalman_predictCovariance(KalmanFilter_t *kf, MatrixVariables_t *mv) {
    float F_T[STATE_VECTOR_SIZE][STATE_VECTOR_SIZE];
    mattranspose8x8(mv->F, F_T);

    float tmp[STATE_VECTOR_SIZE][STATE_VECTOR_SIZE];
    matmul8x8x8(mv->F, kf->P, tmp);
    matmul8x8x8(tmp, F_T, kf->P);
    for (int i = 0; i < STATE_VECTOR_SIZE; ++i) {
        for (int j = 0; j < STATE_VECTOR_SIZE; ++j) {
            kf->P[i][j] += Q[i][j];
            if (std::isnan(kf->P[i][j]) || std::isinf(kf->P[i][j])) {
                kf->P[i][j] = 1.0e+20;
            }
        }
    }
}

void kalman_innovation(const KalmanFilter_t *kf, const SensorData *sensorData,
                       const float dt, float result[MEASURE_SIZE]) {
    Quaternion q = kf->state.orientation;
    Vec3 magnet = sensorData->magnet;
    float pressure = sensorData->pressure;
    float alt = kf->state.altitude;
    float tmp = T0 / (T0 + L0 * (alt - H0));
    float predicted_pressure = P0 * pow(tmp, (G0 * M0) / (R0 * L0));

    Quaternion q_conj = {q.a, -q.b, -q.c, -q.d};
    Quaternion magn_ref = {0, MAGREF_X, MAGREF_Y, MAGREF_Z};
    Quaternion predicted_magn;
    quaternionmul(&q, &magn_ref, &predicted_magn);
    quaternionmul(&predicted_magn, &q_conj, &predicted_magn);
    result[0] = magnet.x - predicted_magn.b;
    result[1] = magnet.y - predicted_magn.c;
    result[2] = magnet.z - predicted_magn.d;
    result[3] = pressure - predicted_pressure;
}

void kalman_innovationCovariance(KalmanFilter_t *kf, MatrixVariables_t *mv) {
    float H_T[STATE_VECTOR_SIZE][MEASURE_SIZE];
    for (int i = 0; i < STATE_VECTOR_SIZE; ++i) {
        for (int j = 0; j < MEASURE_SIZE; ++j) {
            H_T[i][j] = mv->H[j][i];
        }
    }

    float tmp[MEASURE_SIZE][STATE_VECTOR_SIZE];

    matmul4x8x8(mv->H, kf->P, tmp);
    matmul4x8x4(tmp, H_T, mv->S);
    for (int i = 0; i < MEASURE_SIZE; ++i) {
        for (int j = 0; j < MEASURE_SIZE; ++j) {
            mv->S[i][j] += R[i][j];
            if (std::isnan(mv->S[i][j])) {
                mv->S[i][j] = 0.0;
            }
        }
    }
}

void kalman_computeMeasurementJacobian(KalmanFilter_t *kf,
                                       MatrixVariables_t *mv, const float dt) {
    float tmp = T0 / (T0 + L0 * (kf->state.altitude - H0));
    float tmp2 = pow(tmp, (G0 * M0) / (R0 * L0) + 1);
    mv->H[3][7] = G0 * M0 * P0 * pow(tmp, tmp2) / (R0 * L0);
    if (mv->H[3][7] < 0.0) {
        mv->H[3][7] = -mv->H[3][7];
    }

    Vec3 psi = {MAGREF_X, MAGREF_Y, MAGREF_Z};
    Quaternion q = kf->state.orientation;

    mv->H[0][0] = (q.a * psi.x + q.c * psi.z - q.d * psi.y) * 2;
    mv->H[0][1] = (q.b * psi.x + q.c * psi.y + q.d * psi.z) * 2;
    mv->H[0][2] = (q.a * psi.z - q.c * psi.x + q.b * psi.y) * 2;
    mv->H[0][3] = (q.d * psi.x - q.a * psi.y + q.b * psi.z) * 2;
    mv->H[1][0] = (q.a * psi.y + q.d * psi.x - q.b * psi.z) * 2;
    mv->H[1][1] = (q.c * psi.x - q.b * psi.y - q.a * psi.z) * 2;
    mv->H[1][2] = (q.c * psi.y + q.b * psi.x + q.d * psi.z) * 2;
    mv->H[1][3] = (q.c * psi.z - q.d * psi.y + q.a * psi.x) * 2;
    mv->H[2][0] = (q.a * psi.z + q.b * psi.y - q.c * psi.x) * 2;
    mv->H[2][1] = (q.d * psi.x - q.b * psi.z + q.a * psi.y) * 2;
    mv->H[2][2] = (q.d * psi.y - q.c * psi.z - q.a * psi.x) * 2;
    mv->H[2][3] = (q.d * psi.z + q.b * psi.x + q.c * psi.y) * 2;
}

void kalman_computeStateTransitionJacobian(KalmanFilter_t *kf,
                                           MatrixVariables_t *mv,
                                           const SensorData *sensorData,
                                           const float dt) {
    Vec3 a = sensorData->accel;
    Vec3 w = sensorData->gyro;
    Quaternion q = kf->state.orientation;
    double dt_times_2 = 2.0 * dt;
    double dt_over_2 = 0.5 * dt;

    mv->F[0][1] = -w.x * dt_over_2;
    mv->F[0][2] = -w.y * dt_over_2;
    mv->F[0][3] = -w.z * dt_over_2;
    mv->F[1][0] = w.x * dt_over_2;
    mv->F[1][2] = w.z * dt_over_2;
    mv->F[1][3] = -w.y * dt_over_2;
    mv->F[2][0] = w.y * dt_over_2;
    mv->F[2][1] = -w.z * dt_over_2;
    mv->F[2][3] = w.x * dt_over_2;
    mv->F[3][0] = w.z * dt_over_2;
    mv->F[3][1] = w.y * dt_over_2;
    mv->F[3][2] = -w.x * dt_over_2;

    mv->F[4][0] = (q.a * a.x + q.c * a.z - q.d * a.y) * dt_times_2;
    mv->F[4][1] = (q.b * a.x + q.c * a.y + q.d * a.z) * dt_times_2;
    mv->F[4][2] = (q.a * a.z - q.c * a.x + q.b * a.y) * dt_times_2;
    mv->F[4][3] = (q.d * a.x - q.a * a.y + q.b * a.z) * dt_times_2;
    mv->F[5][0] = (q.a * a.y + q.d * a.x - q.b * a.z) * dt_times_2;
    mv->F[5][1] = (q.c * a.x - q.b * a.y - q.a * a.z) * dt_times_2;
    mv->F[5][2] = (q.c * a.y + q.b * a.x + q.d * a.z) * dt_times_2;
    mv->F[5][3] = (q.c * a.z - q.d * a.y + q.a * a.x) * dt_times_2;
    mv->F[6][0] = (q.a * a.z + q.b * a.y - q.c * a.x) * dt_times_2;
    mv->F[6][1] = (q.d * a.x - q.b * a.z + q.a * a.y) * dt_times_2;
    mv->F[6][2] = (q.d * a.y - q.c * a.z - q.a * a.x) * dt_times_2;
    mv->F[6][3] = (q.d * a.z + q.b * a.x + q.c * a.y) * dt_times_2;

    mv->F[7][6] = dt;
}

void kalman_computeKalmanGain(KalmanFilter_t *kf, MatrixVariables_t *mv) {
    float H_T[STATE_VECTOR_SIZE][MEASURE_SIZE];
    for (int i = 0; i < STATE_VECTOR_SIZE; ++i) {
        for (int j = 0; j < MEASURE_SIZE; ++j) {
            H_T[i][j] = mv->H[j][i];
        }
    }

    float S_inv[MEASURE_SIZE][MEASURE_SIZE];
    matinv4x4(mv->S, S_inv);
    float tmp[STATE_VECTOR_SIZE][MEASURE_SIZE];
    matmul8x8x4(kf->P, H_T, tmp);
    matmul8x4x4(tmp, S_inv, mv->K);
}

void kalman_updateStateEstimate(KalmanFilter_t *kf, MatrixVariables_t *mv,
                                const float innovation[MEASURE_SIZE]) {
    float tmp[STATE_VECTOR_SIZE];
    matmul8x4x1(mv->K, innovation, tmp);
    kf->state.orientation.a += tmp[0];
    kf->state.orientation.b += tmp[1];
    kf->state.orientation.c += tmp[2];
    kf->state.orientation.d += tmp[3];
    kf->state.velocity.x += tmp[4];
    kf->state.velocity.y += tmp[5];
    kf->state.velocity.z += tmp[6];
    kf->state.altitude += tmp[7];
    quaternion_normalize(&kf->state.orientation);
}

void kalman_updateCovarianceEstimate(KalmanFilter_t *kf,
                                     MatrixVariables_t *mv) {
    float KH[STATE_VECTOR_SIZE][STATE_VECTOR_SIZE];
    matmul8x4x8(mv->K, mv->H, KH);
    float I[STATE_VECTOR_SIZE][STATE_VECTOR_SIZE] = {
        {1, 0, 0, 0, 0, 0}, {0, 1, 0, 0, 0, 0}, {0, 0, 1, 0, 0, 0},
        {0, 0, 0, 1, 0, 0}, {0, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 0, 1}};
    float tmp[STATE_VECTOR_SIZE][STATE_VECTOR_SIZE];
    for (int i = 0; i < STATE_VECTOR_SIZE; ++i) {
        for (int j = 0; j < STATE_VECTOR_SIZE; ++j) {
            tmp[i][j] = I[i][j] - KH[i][j];
        }
    }
    matmul8x8x8(tmp, kf->P, kf->P);

    for (int i = 0; i < STATE_VECTOR_SIZE; ++i) {
        kf->P[i][i] += 0.0001;
    }
}

void kalman_preLiftoffCalibration(KalmanFilter_t *kf) {}

/* Old code used for numerical differentiation kept for reference if analytical
 * solution is wrong */

//   float epsilon = 0.0001;
//   Quaternion world_frame_accel = {0, a.x, a.y, a.z};
//   Quaternion q_conj = {q.a, -q.b, -q.c, -q.d};
//   Quaternion perturbed_predicted_accel, predicted_accel;
//
//   quaternionmul(&q, &world_frame_accel, &predicted_accel);
//   quaternionmul(&predicted_accel, &q_conj, &predicted_accel);
//   // numerical differentiation
//   for (int i = 0; i < 4; ++i) {
//     Quaternion perturbed_q = q;
//     ((float *)&perturbed_q)[i] += epsilon; // Perturb the i-th component
//
//     Quaternion perturbed_q_conj = {perturbed_q.a, -perturbed_q.b,
//                                    -perturbed_q.c, -perturbed_q.d};
//     // Compute the predicted acceleration for the perturbed orientation
//     quaternionmul(&perturbed_q, &world_frame_accel,
//     &perturbed_predicted_accel); quaternionmul(&perturbed_predicted_accel,
//     &perturbed_q_conj,
//                   &perturbed_predicted_accel);
//     mv->F[4][i] =
//         ((perturbed_predicted_accel.b - predicted_accel.b) / epsilon) * dt;
//     mv->F[5][i] =
//         ((perturbed_predicted_accel.c - predicted_accel.c) / epsilon) * dt;
//     mv->F[6][i] =
//         ((perturbed_predicted_accel.d - predicted_accel.d) / epsilon) * dt;
// }

// Quaternion magn_ref = {0, MAGREF_X, MAGREF_Y, MAGREF_Z};
// Quaternion q_conj = {q.a, -q.b, -q.c, -q.d};
// Quaternion perturbed_predicted_magn, predicted_magn;
// float epsilon = 0.0001;
// quaternionmul(&q, &magn_ref, &predicted_magn);
// quaternionmul(&predicted_magn, &q_conj, &predicted_magn);
//
// for (int i = 0; i < 4; ++i) {
//   Quaternion perturbed_q = q;
//   ((float *)&perturbed_q)[i] += epsilon;
//   Quaternion perturbed_q_conj = {perturbed_q.a, -perturbed_q.b,
//                                  -perturbed_q.c, -perturbed_q.d};
//   quaternionmul(&perturbed_q, &magn_ref, &perturbed_predicted_magn);
//   quaternionmul(&perturbed_predicted_magn, &perturbed_q_conj,
//                 &perturbed_predicted_magn);
//   // Calculate partial derivatives
//   mv->H[0][i] = ((perturbed_predicted_magn.b - predicted_magn.b) /
//   epsilon); mv->H[1][i] = ((perturbed_predicted_magn.c - predicted_magn.c)
//   / epsilon); mv->H[2][i] = ((perturbed_predicted_magn.d -
//   predicted_magn.d) / epsilon);
// }
