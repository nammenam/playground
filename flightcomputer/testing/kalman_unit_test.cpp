/*
    kalman_unit_test.c

    Created on: 09.02.2024
    author: 	Brage Wiseth
    description: 	Unit test for the kalman filter made to be compiled for personal computer not the microcontroller

*/


#include "../firmware/src/datastructures.h"
#include "../firmware/src/kalmanfilter/KalmanFilter.hpp"
#include "../firmware/src/utility/utility.hpp"
#include <time.h>
#include <random>
#include <iostream>


void generate_data(SensorData* data, float rng[])
{
    data->accel.x = 2   ;//+ rng[0];
    data->accel.y = 2   ;//+ rng[1];
    data->accel.z = 9   ;//+ rng[2];
    data->gyro.x = 0.3    ;//+ rng[3];
    data->gyro.y = 0.3    ;//+ rng[4];
    data->gyro.z = 0.3    ;//+ rng[5];
    data->magnet.x = 0  ;//+ rng[6];
    data->magnet.y = 0  ;//+ rng[7];
    data->magnet.z = 0  ;//+ rng[8];
    data->pressure= 900 ;//+ rng[9];
}





void print_state(KalmanFilter_t* kf)
{
    float q[4] = {kf->state.orientation.a, kf->state.orientation.b, kf->state.orientation.c, kf->state.orientation.d};
    printf("orientation: %f, %f, %f, %f\n", kf->state.orientation.a, kf->state.orientation.b, kf->state.orientation.c, kf->state.orientation.d);
    printf("velocity: %f, %f, %f\n", kf->state.velocity.x, kf->state.velocity.y, kf->state.velocity.z);
    printf("altitude: %f\n\n", kf->state.altitude);
}


void print_matrix8x8(float matrix[8][8])
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            printf("%f ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void print_matrix4x4(float matrix[4][4])
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            printf("%f ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void print_matrix4x8(float matrix[4][8])
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            printf("%f ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void print_matrix8x4(float matrix[8][4])
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            printf("%f ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}



void print_innovation(float innovation[4])
{
    printf("innovation: %f, %f, %f, %f\n\n", innovation[0], innovation[1], innovation[2], innovation[3]);
}


//Made as global variables to be able to debug with GDB easier
KalmanFilter_t kf;
MatrixVariables_t mvG;

int main(int argc, char **argv) 
{
    SensorData data;

    std::random_device rd;
    std::mt19937 gen(rd());


    std::normal_distribution<double> d{0, 10.0};

    float rng[10];
    double dt = 0.001;


   


    kalman_init(&kf, &mvG);

    float innovation[4];

    for (int i = 0; i < 20; i++)
    {
        for (int i = 0; i < 10; i++)
        {
            rng[i] = d(gen);
        }
        generate_data(&data, rng);
        kalman_predictState(&kf, &data, dt );
        kalman_computeStateTransitionJacobian(&kf,&mvG, &data, dt);
        printf("F\n");
        print_matrix8x8(mvG.F);
        kalman_predictCovariance(&kf,&mvG);
        printf("P\n");
        print_matrix8x8(kf.P);
        kalman_innovation(&kf, &data, dt, innovation);
        kalman_computeMeasurementJacobian(&kf,&mvG,dt);
        printf("H\n");
        print_matrix4x8(mvG.H);
        kalman_innovationCovariance(&kf,&mvG);
        printf("S\n");
        print_matrix4x4(mvG.S);
        kalman_computeKalmanGain(&kf,&mvG);
        printf("K\n");
        print_matrix8x4(mvG.K);
        printf("innovation\n");
        print_innovation(innovation);
        kalman_updateStateEstimate(&kf,&mvG, innovation);
        kalman_updateCovarianceEstimate(&kf,&mvG);
        printf("State:\n");
        print_state(&kf);
        if (kf.state.altitude < 0)
        {
            printf("Altitude is negative\n");
            kf.state.altitude = 0;
        }
        if (std::isnan(kf.state.altitude))
        {
            printf("Altitude is nan\n");
            kf.state.altitude = 0;
            break;
        }
    }



}
