/* TODO:  Include other files here if needed. */

#include "magnetometer.h"
#include <definitions.h>

// Burst mode - continuously making measurements 
// In this mode only RM and EX commands are allowed 
// Mode is started with the SB command and can be ended with the EX command
// SB command
// EX command 

// Measurements are read by the RM command
// Status byte indicates the freshness of the data
// INT pin will go high each time a measurment is finished
// Reading out the data will clear the INT pin



void magn_init()
{
    uint8_t tx = MAGN_CMD_RT; // reset
    uint8_t rx = 0;              // status
    uint8_t null = 0;
    // MAGN_SLAVE_SELECT_Clear(); // Select the slave device (active low)
    // SERCOM5_SPI_WriteRead(&tx, 1, NULL, 1);
    // MAGN_SLAVE_SELECT_Set(); // Deselect the slave device (active low)
    tx = MAGN_CMD_EX; // exit burst mode
    MAGN_SLAVE_SELECT_Clear(); // Select the slave device (active low)
    SERCOM5_SPI_WriteRead(&tx, 1, NULL, 1);
    SERCOM5_SPI_WriteRead(&null, 1, &rx, 1);
    MAGN_SLAVE_SELECT_Set(); // Deselect the slave device (active low)
}


void magn_idle()
{
    uint8_t tx = MAGN_CMD_EX; // reset
    uint8_t rx = 0;              // status
    uint8_t null = 0;
    MAGN_SLAVE_SELECT_Clear(); // Select the slave device (active low)
    SERCOM5_SPI_WriteRead(&tx, 1, NULL, 1);
    SERCOM5_SPI_WriteRead(&null, 1, &rx, 1);
    MAGN_SLAVE_SELECT_Set(); // Deselect the slave device (active low)
}


int magn_read_measurement(Vec3* data) 
{
    uint8_t tx = MAGN_CMD_RM_XYZ; // read measurement
    uint8_t null = 0;
    uint8_t rx[8] = {0}; 
    MAGN_SLAVE_SELECT_Clear(); // Select the slave device (active low)
    SERCOM5_SPI_WriteRead(&tx, 1, NULL, 1); // send command
    SERCOM5_SPI_WriteRead(&null, 8, rx, 8); // read data
    MAGN_SLAVE_SELECT_Set(); // Deselect the slave device (active low)
    // uint8_t status = rx[0];
    data->x = (rx[1] << 8) | rx[2];
    data->y = (rx[3] << 8) | rx[4];
    data->z = (rx[5] << 8) | rx[6];
    // uint8_t crc = rx[7];
    // if (status & MAGN_STATUS_DRDY_BM) 
    // {
        // return 0;
    // }
    return 1;

}


int magn_run()
{
    uint8_t tx = MAGN_CMD_SB_XYZ; // start burst mode
    uint8_t rx = 0;              // status
    uint8_t null = 0;
    MAGN_SLAVE_SELECT_Clear(); // Select the slave device (active low)
    SERCOM5_SPI_WriteRead(&tx, 1, NULL, 1);
    SERCOM5_SPI_WriteRead(&null, 1, &rx, 1);
    MAGN_SLAVE_SELECT_Set(); // Deselect the slave device (active low)
    if (rx & MAGN_STATUS_B_MODE_BM)
    {
        return 0;
    }
    return -1;
}
