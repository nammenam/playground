#pragma once
#ifndef MAGNETOMETER_H
#define MAGNETOMETER_H


/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif


#include "datastructures.h"

/*******************************************************************************
* STATUS BYTE MASKS
* the status byte is returned by the magnetometer after every command
*******************************************************************************/
enum 
{
    MAGN_STATUS_B_MODE_BM = (0x80),   // Burst mode
    MAGN_STATUS_WOC_MODE_BM = (0x40), // Wake on change mode
    MAGN_STATUS_S_MODE_BM = (0x20),   // Single measurement mode
    MAGN_STATUS_DRDY_BM = (0x01),     // Data ready
    MAGN_STATUS_RESET_BM = (0x02),    // Reset
    MAGN_STATUS_OVF_BM = (0x04),      // Overflow
    MAGN_STATUS_SEC_BM = (0x04),      // Single bit error correction
    MAGN_STATUS_DED_BM = (0x08),      // Double bit error detection
    MAGN_STATUS_CE_BM = (0x08),       // Communication error
};



/*******************************************************************************
* COMMANDS
* by sending a command to the magnetometer, the user can control the device
* one cammand byte is sent to the magnetometer,
* n the commands SB, WOC, SM and RM need to specify the fist 4 bits (zyxt). This is used to select which components need to be
* measured (set corresponding bit to 1). zyxt = 0x0 results in all components being measured according the
* settings of VmeasEn and BurstSel (X, Y, Z, T and V). Else, the components of the zyxt argument are
* measured. With temperature compensation enabled, the temperature will always be measured. However
* depending on the argument zyxt, it can be read out or not (for CRC calculation it will be 0x0). Recommended
* is to do the measurement and the read command with the same zyxt argument.
* Example: SB | 0x0E will start a burst measurement of the ZYX components only.
*******************************************************************************/
enum
{
    MAGN_CMD_SB = (0x10),   /**> Start burst-meas mode. */
    MAGN_CMD_SB_XYZ = (0x1E), /**> Start burst-meas mode with xyz */
    MAGN_CMD_WOC = (0x20),  /**> Start wake-on-change mode. */
    MAGN_CMD_SM = (0x30),   /**> Start single-meas mode. */
    MAGN_CMD_RM = (0x40),   /**> Read measurement.*/
    MAGN_CMD_RM_XYZ = (0x4E), /**> Read measurement with xyz */
    MAGN_CMD_RR = (0x50),   /**> Read register. first four bits is dont't cares */
    MAGN_CMD_WR = (0x60),   /**> Write register. first four bits is dont't cares */
    MAGN_CMD_RV = (0xB0),   /**> Read voltage. */
    MAGN_CMD_HR = (0xC0),   /**> Memory recall. */
    MAGN_CMD_HS = (0xD0),   /**> Memory store. */
    MAGN_CMD_EX = (0x80),   /**> Exit mode. */
    MAGN_CMD_RT = (0xF0),   /**< Reset. */
};



/*******************************************************************************
REGISTERS
used for configuring the magnetometer, each register is 16 bits
*******************************************************************************/

typedef enum 
{
    MAGN_OSR_1,
    MAGN_OSR_2,
    MAGN_OSR_4,
    MAGN_OSR_8
} magn_osr_t;


typedef enum
{
    MAGN_RES_16,
    MAGN_RES_17,  // default
    MAGN_RES_18,
    MAGN_RES_19
} magn_res_t;

// TODO: add more register definitions if needed


static const float gainMultipliers[16] = {
    0.2, 0.25,  0.3333, 0.4, 0.5,  0.6, 0.75,  1,
    0.1, 0.125, 0.1667, 0.2, 0.25, 0.3, 0.375, 0.5
};




/*******************************************************************************
DATA STRUCTURES
*******************************************************************************/






/*******************************************************************************
FUNCTION PROTOTYPES
*******************************************************************************/

void magn_init(void);
int magn_run(void);
int magn_read_measurement(Vec3* data);
void magn_idle(void);



/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* IMU_H */
