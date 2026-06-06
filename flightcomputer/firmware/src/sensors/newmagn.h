#pragma once

#include <cstdint>
#include <stdint.h>
#define BM1422AGMV_DEVICE_ADDRESS_0E   (0x0E)    // 7bit Addrss
#define BM1422AGMV_DEVICE_ADDRESS_0F   (0x0F)    // 7bit Address
#define BM1422AGMV_WIA_VAL             (0x41)

//14 bit continous mode

/* ---Register map--- */ 


#define MAGN_WHO_AM_I                  (0x0F) //Confirm correct device
#define MAGN_INT                       (0x18) // Bitmask (1 << 6), Only care about second msb

//Data fields are two adresses at 8 bits added together to be 16 bits
//TODO combine 
//Mind that these registers do not take our offset into account

#define MAGN_DATA_X                    (0x10) //Bit [7:0]  of Data X
#define MAGN_DATA_X2                   (0x11) //Bit [15:8] of Data X

#define MAGN_DATA_Y                    (0x12) //Bit [7:0]  of Data Y
#define MAGN_DATA_Y2                   (0x13) //Bit [15:8] of Data Y
                                              
#define MAGN_DATA_Z                    (0x14) //Bit [7:0]  of Data Z
#define MAGN_DATA_Z2                   (0x15) //Bit [15:8] of Data Z
                                              

//Registers for offets (WE SHOULD SET THE OFFSETS IN INIT FUNCTION)
#define MAGN_DATA_X_OFF                (0x6C) //Bit [7:0]  of Data X OFFSET
#define MAGN_DATA_X2_OFF               (0x6D) //Bit [15:8] of Data X OFFSET

#define MAGN_DATA_Y_OFF                (0x72) //Bit [7:0]  of Data Y OFFSET
#define MAGN_DATA_Y2_OFF               (0x73) //Bit [15:8] of Data Y OFFSET
                                              
#define MAGN_DATA_Z_OFF                (0x78) //Bit [7:0]  of Data Z OFFSET
#define MAGN_DATA_Z2_OFF               (0x79) //Bit [15:8] of Data Z OFFSET
 
 
//CNTL Registers 

//#define BAGMV_STA1                (0x18)
#define MAGN_CNTL1               (0x1B)
#define MAGN_CNTL2               (0x1C)
#define MAGN_CNTL3               (0x1D)
#define MAGN_AVE_A               (0x40)
#define MAGN_CNTL4               (0x5C)


/* ---End Register map--- */

/* ---Start Bitmask--- */

#define MAGN_DATARDY_BM                (1 << 6) //Bitmask for DATAREADY

#define BM1422AGMV_CNTL1_FS1           (1 << 1)
#define BM1422AGMV_CNTL1_ODR_10Hz      (0 << 3)
#define BM1422AGMV_CNTL1_RST_LV        (1 << 5)
#define BM1422AGMV_CNTL1_OUT_BIT       (1 << 6)
#define BM1422AGMV_CNTL1_PC1           (1 << 7)

#define BM1422AGMV_CNTL2_DRP           (1 << 2)
#define BM1422AGMV_CNTL2_DREN          (1 << 3)

#define BM1422AGMV_CNTL3_FORCE         (1 << 6)

#define BM1422AGMV_AVE_A_AVE4          (0 << 2)

#define BM1422AGMV_CNTL1_VAL           (BM1422AGMV_CNTL1_FS1 | BM1422AGMV_CNTL1_OUT_BIT | BM1422AGMV_CNTL1_PC1)
#define BM1422AGMV_CNTL2_VAL           (BM1422AGMV_CNTL2_DREN)
#define BM1422AGMV_CNTL3_VAL           (BM1422AGMV_CNTL3_FORCE)
#define BM1422AGMV_CNTL4_VAL           (0x0000)
#define BM1422AGMV_AVE_A_VAL           (BM1422AGMV_AVE_A_AVE4)

#define BM1422AGMV_14BIT_SENS          (24)
#define BM1422AGMV_12BIT_SENS          (6)


/* ---End Bitmask--- */

//TODO SET!
#define OFFSET_X 0x0 
#define OFFSET_Y 0x0 
#define OFFSET_Z 0x0 

void initMagn();
uint8_t magnWhoAmI();
void setOffsets();
void readMagn(); //TODO where to return values
