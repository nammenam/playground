#pragma once
#ifndef PS_H
#define PS_H

#include <stdint.h>
#include "datastructures.h"


#define CONVERT_D1  0x40
#define CONVERT_D2  0x50
#define ADC_READ    0x00
#define RESET       0x1E

// #define P_SENS      47109
// #define P_OFFSET    45940
// #define TCS         29686
// #define TCO         26510
// #define T_REF       32891
// #define T_SENS      28569


//Taken from datasheet page 8, factory calibrated offsets from PROM
typedef struct
{
    uint16_t P_SENS;
    uint16_t P_OFFSET;
    uint16_t TCS;
    uint16_t TCO;
    uint16_t T_REF;
    uint16_t T_SENS;   // C6 Temperature coefficient of the temperature       | TEMPSENSE
} PT_C;






typedef enum
{
    osr_256  = 0x00, // 256 samples per measurement
    osr_512  = 0x02, // 512 samples per measurement
    osr_1024 = 0x04, // 1024 samples per measurement
    osr_2048 = 0x06, // 2048 samples per measurement
    osr_4096 = 0x08  // 4096 samples per measurement
} PT_OSR;



void ps_init();
int ps_reset();
int ps_readProm();
void _ps_start_pressure_conversion();
void _ps_start_temp_conversion();
void _ps_convert_temp(uint32_t raw, float* res_temp);
void _ps_convert_pressure(uint32_t raw, float* res_pressure, float* res_temp);

/*
 * Reads the measurement from the sensor
 * @param pressure: pointer to the pressure value, will be set by the function if the result is valid and the measurement_type is 1
 * @param temp: pointer to the temperature value, will be set by the function if the result is valid and the measurement_type is 0, is used to calculate pressure
 * @param measurement_type: measurement to be made,eg. start temp conversion or pressure conversion next, 0 for temp, 1 for pressure
 * @return -1 if the result is invalid, 0 if the result is temperture, 1 if the result is pressure
 */
uint8_t ps_get_measurement(float* pressure, float* temp);

#endif //PS_H
