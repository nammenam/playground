/* 



OLD FILE - ONLY FOR REFERENCE


#ifndef PRESSURE_SENSOR_H
#define PRESSURE_SENSOR_H


#ifdef __cplusplus
extern "C" {
#endif
    

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>


#define ms561101ba03_t Barometer

#define CONVERT_D1  0x40
#define CONVERT_D2  0x50
#define ADC_READ    0x00
#define RESET       0x1E
    

typedef enum
{
    OSR_256  = 0x00, // 256 samples per measurement
    OSR_512  = 0x02, // 512 samples per measurement
    OSR_1024 = 0x04, // 1024 samples per measurement
    OSR_2048 = 0x06, // 2048 samples per measurement
    OSR_4096 = 0x08  // 4096 samples per measurement
} ms561101ba03_osr_t;



typedef struct
{
    uint16_t sens;       // C1 Pressure sensitivity                             | SENS_t1
    uint16_t off;        // C2 Pressure offset                                  | OFF_t1
    uint16_t tcs;        // C3 Temperature coefficient of pressure sensitivity  | TCS
    uint16_t tco;        // C4 Temperature coefficient of pressuer offset       | TCO
    uint16_t t_ref;      // C5 Reference temperature                            | T_ref
    uint16_t tempsens;   // C6 Temperature coefficient of the temperature       | TEMPSENSE
} ms561101ba03_config_data_t;

typedef struct
{
    int32_t raw_pressure;       // Compensated pressure from 10 mbar to 1200 mbar with 0.01 mbar resolution
    int32_t raw_temperature;    // Temperature from -40 C to 85 C with 0.01 C resulution
} ms561101ba03_sensor_data_t;

typedef struct
{
    int32_t pressure;       // Compensated pressure from 10 mbar to 1200 mbar with 0.01 mbar resolution
    int32_t temperature;    // Temperature from -40 C to 85 C with 0.01 C resulution
} ms561101ba03_result_t;

typedef struct
{
    ms561101ba03_osr_t osr;                 // Oversampling setting
    ms561101ba03_config_data_t config_data; // Device configuration
    ms561101ba03_sensor_data_t sensor_data; // Raw sensor data
    ms561101ba03_result_t result;           // Result
    int32_t dT;                             // delta temperature
}  Barometer;



void PSInit(ms561101ba03_t *dev); //ms561101ba03_t *dev

void read_prom(ms561101ba03_t *dev);

int getBarometerData(ms561101ba03_t *dev);

int get_raw_pressure(ms561101ba03_t *dev, uint32_t *result);

int PSgetRawTemperature(ms561101ba03_t *dev, uint32_t *result);

double PSgetPressure(ms561101ba03_t *dev);

double getTemperature(ms561101ba03_t *dev);

void startPressureConversion(ms561101ba03_t *dev);

void startTemperatureConversion(ms561101ba03_t *dev);

bool readADC(uint32_t *result);

void PSRun(ms561101ba03_t* device);

void resetPS();

#ifdef __cplusplus
}
#endif

#endif 

*/
