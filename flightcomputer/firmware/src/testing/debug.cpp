// author: Brage Wiseth, Felix Strand
/*********************************************************************
    This file contains functions that are used for debugging purposes.
    These functions are not used in the final product.
*********************************************************************/

#include "debug.hpp"
#include <cstdio>
#include <cstdarg>
#include <stdint.h>
#include "peripheral/sercom/usart/plib_sercom4_usart.h"

/*
*   Function that prints a string to the UART
*   similar signature to printf / sprintf
*   @param str: string to print
*   @param ...: arguments to print
*   @return: 0 if failed, 1 if success
*/
int uart_printf(const char* str, ...)
{
    va_list args;
    va_start(args, str);
    char buffer[100];
    int size = vsnprintf(buffer, 100, str, args);
    va_end(args);
    if (size < 0) { return 0; }
    if (size > 100) { return 0; }
    uint8_t write_status = 0;
    while (SERCOM4_USART_TransmitterIsReady() == false) {}
    write_status = SERCOM4_USART_Write(buffer, size);
    return write_status;
}

void uart_setup()
{
    SERCOM4_USART_ReceiverDisable();
    SERCOM4_USART_Enable();
    SERCOM4_USART_TransmitterEnable();
}



            // if (IMU_INT2_Get() && IMU_INT1_Get())
            // {
            //     imu_get_gyro_rps(&appData.sensorData.gyro);
            //     imu_get_accel_ms2(&appData.sensorData.accel);
            //     Vec3 gyro = appData.sensorData.gyro;
            //     Vec3 accel = appData.sensorData.accel;
            //     // float alpha = 0.000001;
            //     // reinsiirforaccel(&filteredgyro, &gyro, alpha);
            //     // reinsiirforaccel(&filteredAccel, &accel, alpha);
            //
            //     // float dt = 0.003;
            //     float dt = getCurrentTimeMS() - appData.lastTime;
            //     dt = dt / 1000.0;
            //     appData.lastTime = getCurrentTimeMS();
            //
            //     Quaternion omega = {0, gyro.x * dt, gyro.y * dt, gyro.z * dt};
            //     Quaternion q_dot = {0, 0, 0, 0};
            //     Quaternion* q = &appData.kf.state.orientation;
            //     quaternionmul(q, &omega, &q_dot);
            //     q_dot->a *= 0.5;
            //     q_dot->b *= 0.5;
            //     q_dot->c *= 0.5;
            //     q_dot->d *= 0.5;
            //
            //     q->a += q_dot.a;
            //     q->b += q_dot.b;
            //     q->c += q_dot.c;
            //     q->d += q_dot.d;
            //
            //     quaternion_normalize(q);
            //
            //
            //     Quaternion q_conj = {q->a, -q->b, -q->c, -q->d};
            //     Quaternion a_ref = {0, accel.x, accel.y, accel.z};
            //     Quaternion predicted_accel = {0, 0, 0, 0};
            //     quaternionmul(&q, &a_ref, &predicted_accel);
            //     quaternionmul(&predicted_accel, &q_conj, &predicted_accel);
            //     predicted_accel.d -= 9.81;
            //
            //     // uart_printf("gyro: %.3f, %.3f, %.3f   \r", gyro.x, gyro.y, gyro.z);
            //     // uart_printf("accel: %.3f, %.3f, %.3f   \r", accel.x, accel.y, accel.z);
            //     // uart_printf("predicted accel: %.3f, %.3f, %.3f   \r", predicted_accel.b, predicted_accel.c, predicted_accel.d);
            //
            //     char buf[30];
            //     buf[0] = ((char*) &q.a)[0];
            //     buf[1] = ((char*) &q.a)[1];
            //     buf[2] = ((char*) &q.a)[2];
            //     buf[3] = ((char*) &q.a)[3];
            //     buf[4] = ((char*) &q.b)[0];
            //     buf[5] = ((char*) &q.b)[1];
            //     buf[6] = ((char*) &q.b)[2];
            //     buf[7] = ((char*) &q.b)[3];
            //     buf[8] = ((char*) &q.c)[0];
            //     buf[9] = ((char*) &q.c)[1];
            //     buf[10] = ((char*) &q.c)[2];
            //     buf[11] = ((char*) &q.c)[3];
            //     buf[12] = ((char*) &q.d)[0];
            //     buf[13] = ((char*) &q.d)[1];
            //     buf[14] = ((char*) &q.d)[2];
            //     buf[15] = ((char*) &q.d)[3];
            //     buf[16] = ((char*) &predicted_accel.b)[0];
            //     buf[17] = ((char*) &predicted_accel.b)[1];
            //     buf[18] = ((char*) &predicted_accel.b)[2];
            //     buf[19] = ((char*) &predicted_accel.b)[3];
            //     buf[20] = ((char*) &predicted_accel.c)[0];
            //     buf[21] = ((char*) &predicted_accel.c)[1];
            //     buf[22] = ((char*) &predicted_accel.c)[2];
            //     buf[23] = ((char*) &predicted_accel.c)[3];
            //     buf[24] = ((char*) &predicted_accel.d)[0];
            //     buf[25] = ((char*) &predicted_accel.d)[1];
            //     buf[26] = ((char*) &predicted_accel.d)[2];
            //     buf[27] = ((char*) &predicted_accel.d)[3];
            //     buf[28] = '\n';
            //     SERCOM4_USART_Write(buf, 29);
