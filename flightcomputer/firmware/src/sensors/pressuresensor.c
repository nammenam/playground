/*



OLD FILE - ONLY FOR REFERENCE
 

#include "pressuresensor.h"
#include <time.h>






void resetPS()
{
    PT_SLAVE_SELECT_Clear();
    uint8_t buf = RESET;
    SERCOM5_SPI_WriteRead(&buf, 1, NULL, 0);
    // delayms(3);
    PT_SLAVE_SELECT_Set();
}

void read_prom(ms561101ba03_t *dev)
{
    uint8_t tmp[3] = { 0, 0, 0 };
    uint8_t reg = 0xA2; // 1010<001>0
    
    PT_SLAVE_SELECT_Clear();

    SERCOM5_SPI_WriteRead(&reg, 1, tmp, 3);
    dev->config_data.sens = tmp[1] << 8 | tmp[2];
    
    PT_SLAVE_SELECT_Toggle();
    PT_SLAVE_SELECT_Toggle();
    
    reg = 0xA4; //1010<010>0
    //SERCOM5_SPI_WriteRead(&reg, 1, NULL, 0);
    //SERCOM5_SPI_WriteRead(NULL, 0, tmp, 2);
    //dev->config_data.off = tmp[0] << 8 | tmp[1];
    SERCOM5_SPI_WriteRead(&reg, 1, tmp, 3);
    dev->config_data.off = tmp[1] << 8 | tmp[2];
    
    PT_SLAVE_SELECT_Toggle();
    PT_SLAVE_SELECT_Toggle();

    reg = 0xA6; //1010<011>0
    SERCOM5_SPI_WriteRead(&reg, 1, tmp, 3);
    dev->config_data.tcs = tmp[1] << 8 | tmp[2];
    
    PT_SLAVE_SELECT_Toggle();
    PT_SLAVE_SELECT_Toggle();
    
    reg = 0xA8; //1010<100>0
    SERCOM5_SPI_WriteRead(&reg, 1, tmp, 3);
    dev->config_data.tco = tmp[1] << 8 | tmp[2];
    
    PT_SLAVE_SELECT_Toggle();
    PT_SLAVE_SELECT_Toggle();
    
    reg = 0xAA; //1010<101>0
    SERCOM5_SPI_WriteRead(&reg, 1, tmp, 3);
    dev->config_data.t_ref = tmp[1] << 8 | tmp[2];
    
    PT_SLAVE_SELECT_Toggle();
    PT_SLAVE_SELECT_Toggle();
    
    reg = 0xAC; //1010<110>0
    SERCOM5_SPI_WriteRead(&reg, 1, tmp, 3);
    dev->config_data.tempsens = tmp[1] << 8 | tmp[2];
    
    PT_SLAVE_SELECT_Set();
}

void start_pressure_conversion(ms561101ba03_t *dev) //D1
{
    PT_SLAVE_SELECT_Clear();
    uint8_t buf = CONVERT_D1 + dev->osr;
    SERCOM5_SPI_WriteRead(&buf, 1, NULL, 0);
    PT_SLAVE_SELECT_Set();
}

void start_temperature_conversion(ms561101ba03_t *dev) //D2
{
    PT_SLAVE_SELECT_Clear();
    uint8_t buf = CONVERT_D2 + dev->osr;
    SERCOM5_SPI_WriteRead(&buf, 1, NULL, 0);
    PT_SLAVE_SELECT_Set();
}

bool read_adc(uint32_t *result)
{
    *result = 0;
    uint8_t tmp[4];
    uint8_t reg = 0x00;
    
    PT_SLAVE_SELECT_Clear();
    SERCOM5_SPI_WriteRead(&reg, 1, tmp, 4);
    PT_SLAVE_SELECT_Set();

    *result = (tmp[1] << 16) | (tmp[2] << 8) | tmp[3];

    // If we are to fast the ADC will return 0 instead of the actual result
    if (*result == 0)
        return false;

    return true;
}


void calc_dt(ms561101ba03_t *dev, uint32_t digital_temperature)
{
    // Difference between actual and reference digital_temperature
    // dT = D2 - T_ref = D2 - C5 *2^8
    dev->dT = digital_temperature - ((int32_t)dev->config_data.t_ref << 8);
    //dev->dT = digital_temperature - ((int32_t)dev->config_data.t_ref * 256);
}

int32_t calc_temp(ms561101ba03_t *dev)
{
    // Actual temerature (-40...85C with 0.01 resulution)
    // TEMP = 20C +dT * TEMPSENSE = 2000 + dT * C6 / 2^23
    
    //int32_t temp = (2000 + (int64_t)dev->dT * dev->config_data.tempsens / 8388608); 
    int32_t temp = 2000 + ((int64_t)dev->dT * dev->config_data.tempsens >> 23); 
    return temp;
}

int64_t calc_offset(ms561101ba03_t *dev)
{
    // Offset at actual temperature
    // OFF=OFF_t1 + TCO * dT = OFF_t1(C2) * 2^16 + (C4*dT)/2^7
    int64_t temp_offset = ((int64_t)dev->config_data.off * (int64_t)65536)
        + (((int64_t)dev->config_data.tco * (int64_t)dev->dT) / (int64_t)128);
    int64_t temp_offset = (((int64_t)dev->config_data.off) << 16)
        + (((int64_t)dev->config_data.tco * (int64_t)dev->dT) >> 7);
    
    return temp_offset;
}

int64_t calc_sens(ms561101ba03_t *dev)
{
    // Senisitivity at actual temperature
    // SENS=SENS_t1 + TCS *dT = SENS_t1(C1) *2^15 + (TCS(C3) *dT)/2^8
    int64_t sens = (((int64_t)dev->config_data.sens) * (int64_t)32768)
        + (((int64_t)dev->config_data.tcs * (int64_t)dev->dT) / (int64_t)256);
    int64_t sens = (((int64_t)dev->config_data.sens) << 15)
        + (((int64_t)dev->config_data.tcs * (int64_t)dev->dT) >> 8);
    
    return sens;
}

int32_t calc_p(uint32_t digital_pressure, int64_t sens, int64_t off)
{
    // Temperature compensated pressure (10...1200mbar with 0.01mbar resolution
    // P = digital pressure value  * SENS - OFF = (D1 * SENS/2^21 -OFF)/2^15
    int32_t pressure = (((int64_t)digital_pressure
        * (int64_t)((int64_t)sens / (int64_t)2097152) - (int64_t)off)
        / (int64_t)32768); 
    int32_t pressure = (((int64_t)digital_pressure * sens >> 21) - off) >> 15;
    
    return pressure;
}

int get_raw_temperature(ms561101ba03_t *dev, uint32_t *result)
{
    start_temperature_conversion(dev);
    // TODO remove delay?
    // delayms(9);

    if (!read_adc(result))
    {
        return false;
    }
    return true;
}


double PSgetPressure(ms561101ba03_t *dev) 
{       
    int64_t offset = (((int64_t)dev->config_data.off) << 16)
        + (((int64_t)dev->config_data.tco * (int64_t)dev->dT) >> 7);
    
    int64_t sens = (((int64_t)dev->config_data.sens) << 15)
        + (((int64_t)dev->config_data.tcs * (int64_t)dev->dT) >> 8);
    
    dev->result.pressure = (((int64_t)dev->sensor_data.raw_pressure * sens >> 21) - offset) >> 15;
    //dev->result.pressure = calc_p(raw_pressure, sens, offset);
    
    double pressure = ((double)dev->result.pressure) ; // pressure in pascal 
    
    return pressure;
}

double temperature_get(ms561101ba03_t *dev) 
{
    dev->dT = dev->sensor_data.raw_temperature - ((int32_t)dev->config_data.t_ref << 8);
    
    dev->result.temperature = 2000 + ((int64_t)dev->dT * dev->config_data.tempsens >> 23);
        
    double temperature = ((double)dev->result.temperature) * 0.01f; // temperature in celsius 
    
    return temperature; 
}

int ms561101ba03_get_sensor_data(ms561101ba03_t *dev)
{
    uint32_t raw_pressure = 0;
    if (!get_raw_pressure(dev, &raw_pressure)) 
    {
        return false;
    }
    
    dev->sensor_data.raw_pressure = raw_pressure;

    uint32_t raw_temperature = 0;
    if (!get_raw_temperature(dev, &raw_temperature))
    {
    	return false;
    }
    
    dev->sensor_data.raw_temperature = raw_temperature;
    calc_dt(dev, raw_temperature);
    int64_t temp = calc_temp(dev);
    int64_t off = calc_offset(dev);
    int64_t sens = calc_sens(dev);
    
    dev->result.pressure = calc_p(raw_pressure, sens, off);
    dev->result.temperature = (int32_t)temp;
    return true;
}

bool ms561101ba03_get_sensor_data_2(ms561101ba03_t *dev)
{
    // Second order temperature compensation see datasheet p8
    uint32_t raw_pressure = 0;
    if (!get_raw_pressure(dev, &raw_pressure)) 
    {
        return false;
    }

    uint32_t raw_temperature = 0;
    if (!get_raw_temperature(dev, &raw_temperature))
    {
    	return false;
    }
    
    calc_dt(dev, raw_temperature);
    int64_t temp = calc_temp(dev);
    int64_t off = calc_offset(dev);
    int64_t sens = calc_sens(dev);

    //Set defaults for temp >= 2000
    int64_t t_2 = 0;
    int64_t off_2 = 0;
    int64_t sens_2 = 0;
    int64_t a = 0;
    
    if (temp < 2000)
    {
        //Low temperature
        t_2 = ((dev->dT * dev->dT) >> 31);        // T2 = dT^2/2^31
        a = (temp - 2000);
        off_2 = (5 * (a * a)) >> 1;                // OFF_2 = 5 * (TEMP - 2000)^2/2^1
        sens_2 = a >> 2;                          // SENS_2 = 5 * (TEMP - 2000)^2/2^2
        
        if (temp < -1500)
        {
            // Very low temperature
            a = (temp + 1500);
            off_2 = off_2 + 7 * (a * a);             // OFF_2 = OFF_2 + 7 * (TEMP + 1500)^2
            sens_2 = sens_2 + ((11 * (a * a)) >> 1); // SENS_2 = SENS_2 + 7 * (TEMP + 1500)^2/2^1
        }
    }

    temp = temp - t_2;
    off = off - off_2;
    sens = sens - sens_2;

    dev->result.pressure = calc_p(raw_pressure, sens, off);
    dev->result.temperature = (int32_t)temp;
    return true;
}

void ms561101ba03_init(ms561101ba03_t *dev) //ms561101ba03_t *dev
{
  
    // First  we need to reset the chip
    resetPS();
    
    // Get the config coeffs.
    read_prom(dev);
}

void PSRun(ms561101ba03_t* device)
{
    PT_SLAVE_SELECT_Set();
    
    device->osr = OSR_256;
    
    ms561101ba03_init(device);
}

//Commenting out to test returning device

// void pt_run()
// {
//     PT_SLAVE_SELECT_Set();
//     
//     ms561101ba03_t device = {
//         .osr  = OSR_4096,
//     };
//     
//     ms561101ba03_init(&device);
//     
//     ms561101ba03_get_sensor_data(&device);
//     
//     // Commenting this out to call them in main loop of command center.
//     //temperature_get(&device);
//     //pressure_get(&device);
//     
// }
//


*/
