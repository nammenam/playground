#include "ps.h"

#include <definitions.h>
// #include "testing/debug.hpp"

#define MS5611_SKIP_COMP 0
PT_C cValues;
int32_t DT;
int32_t TEMP;

#define PRESSURE_OFFSET 1300.0
#define PRESSURE_SCALE 0.0
#define TEMP_OFFSET 0

void ps_init() {
    ps_reset();
    ps_readProm();
}

void _ps_convert_temp(uint32_t raw, float* res_temp) {
    DT = raw - (cValues.T_REF << 8);
    int32_t temp = 2000 + ((DT * cValues.T_SENS) >> 23);
    TEMP = temp;
    *res_temp = (temp / 100.0) + TEMP_OFFSET;
}

void _ps_convert_pressure(uint32_t raw, float* res_pressure, float* res_temp) {
    int64_t OFF = (cValues.P_OFFSET << 16) + ((cValues.TCO * DT) >> 7);
    int64_t SENS = (cValues.P_SENS << 15) + ((cValues.TCS * DT) >> 8);
    int32_t T2 = 0;
    int64_t OFF2 = 0;
    int64_t SENS2 = 0;

    if (TEMP < 2000) {
        T2 = (DT * DT) >> 31;
        OFF2 = 5 * (((TEMP - 2000) * (TEMP - 2000)) >> 1);
        SENS2 = 5 * (((TEMP - 2000) * (TEMP - 2000)) >> 2);
    }
    if (TEMP < -1500) {
        OFF2 = OFF2 + 7 * ((TEMP + 1500) * (TEMP + 1500));
        SENS2 = SENS2 + 11 * (((TEMP + 1500) * (TEMP + 1500)) >> 1);
    }

    OFF = OFF - OFF2;
    SENS = SENS - SENS2;
    TEMP = TEMP - T2;

    int32_t pressure = (((raw * SENS) >> 21) - OFF) >> 15;
    *res_pressure =
        (pressure / 100.0) -
        PRESSURE_OFFSET;  // TODO linear formula to match vacuum chamber
    *res_temp = ((TEMP - T2) / 100.0) + TEMP_OFFSET;
}

int ps_reset() {
    PT_SLAVE_SELECT_Clear();
    uint8_t buf = 0x0E;
    SERCOM5_SPI_WriteRead(&buf, 1, NULL, 0);
    PT_SLAVE_SELECT_Set();
    return 1;
}

void _ps_start_pressure_conversion() {
    PT_SLAVE_SELECT_Clear();
    uint8_t buf = CONVERT_D1 | osr_256;
    SERCOM5_SPI_WriteRead(&buf, 1, NULL, 0);
    PT_SLAVE_SELECT_Set();
}

void _ps_start_temp_conversion() {
    PT_SLAVE_SELECT_Clear();
    uint8_t buf = CONVERT_D2 | osr_256;
    SERCOM5_SPI_WriteRead(&buf, 1, NULL, 0);
    PT_SLAVE_SELECT_Set();
}

int ps_readProm() {
    PT_SLAVE_SELECT_Clear();
    uint8_t data[3];
    uint16_t results[6];
    uint8_t reg = 0xA2;

    for (int i = 0; i < 6; i++) {
        PT_SLAVE_SELECT_Toggle();
        PT_SLAVE_SELECT_Toggle();
        SERCOM5_SPI_WriteRead(&reg, 1, &data, 3);
        results[i] = data[1] << 8 | data[2];
        reg += 2;
    }
    cValues.P_SENS = results[0];
    cValues.P_OFFSET = results[1];
    cValues.TCS = results[2];
    cValues.TCO = results[3];
    cValues.T_REF = results[4];
    cValues.T_SENS = results[5];
    // uart_printf("P_SENS: %d\n\r", cValues.P_SENS);
    // uart_printf("P_OFFSET: %d\n\r", cValues.P_OFFSET);
    // uart_printf("TCS: %d\n\r", cValues.TCS);
    // uart_printf("TCO: %d\n\r", cValues.TCO);
    // uart_printf("T_REF: %d\n\r", cValues.T_REF);
    // uart_printf("T_SENS: %d\n\r", cValues.T_SENS);

    PT_SLAVE_SELECT_Set();
    return 1;
}

uint8_t ps_get_measurement(float* pressure, float* temp) {
    static uint8_t last_measurement_type = 1;
    uint8_t m_type = last_measurement_type;
    uint8_t next_measurement_type = !last_measurement_type;
    float last_known_temp = *temp;

    uint8_t buf[4];
    uint8_t reg = 0x00;

    PT_SLAVE_SELECT_Clear();
    SERCOM5_SPI_WriteRead(&reg, 1, buf, 4);
    PT_SLAVE_SELECT_Set();

    uint32_t raw = (buf[1] << 16) | (buf[2] << 8) | buf[3];

    if (raw == 0 || raw == 16777215) {
    } else if (last_measurement_type == 0) {
        _ps_convert_temp(raw, temp);
    } else {
        _ps_convert_pressure(raw, pressure, temp);
    }

    if (next_measurement_type == 0) {
        _ps_start_temp_conversion();
        last_measurement_type = 0;
        return m_type;
    } else {
        _ps_start_pressure_conversion();
        last_measurement_type = 1;
        return m_type;
    }
}
