
#include "newmagn.h"
#include <cstdint>


void initMagn()
{
  //STEP 1
  //Who am i to verify device
  // 
  //CNTL1 0x1B = 0xC0 // PC1 and OUT_BIT to high
  //digitalWrite(CNTL1, 0xC0)

  //CNTL4 0x5C = 0x00 //Reset //doesnt really do anything but follow the datasheet
  //CNTL4 0x5D = 0x00 //Reset //puts reset bit 
  //digitalWrite(0x5C, 0x00);
  //digitalWrite(0x5D, 0x00);

  //STEP 2 - Terminal enable setting
  //digitalWrite(MAGN_CNTL2, 0x0C);

  //STEP 3 set offsets
  setOffsets();


  //STEP 4 Write CNTL3 0x1D 0x40 FORCE REG: 
  /*
    AD start measurement trigger at continuous mode (FS1=0)
    and single mode (FS1=1)
    1: Start measurement
    Register is automatic clear “0” after write data “1”
    Write data “0” is invalid
    If write data “1” on measurement way, restart measurement
   */

  //digitalWrite(MAGN_CNTL3, 0x40);
}

uint8_t magnWhoAmI()
{
  //return digitalRead(MAGN_WHO_AM_I); //I think it should return 0x41 but confused by datasheet

  return 1;
}

//14 Bit output data at continous mode
void readMAGN()
{
  //Reads magn at DATAX-DATAY, both registers for each axis and combines them. Unsure if 
  //we should read from DATAX or FINEOUTPUTX, atm reading DATAX normally
  //DA

  //if (!digitalRead(MAGN_INT & (1 << 6))) //Will probably be abstracted away by 
  //  return;

  //uint8_t xLSB = digitalRead(0x10);
  //uint8_t xMSB = digitalRead(0x11);
  //uint16_t dataX = (xMSB << 8) | xLSB;

  //uint8_t yLSB = digitalRead(0x12);
  //uint8_t yMSB = digitalRead(0x13);
  //uint16_t dataY = (xMSB << 8) | yLSB;

  //uint8_t zLSB = digitalRead(0x14);
  //uint8_t zMSB = digitalRead(0x15);
  //uint16_t dataZ = (xMSB << 8) | zLSB;


  //TODO return somehow, probably in pointer argument

}

//void combineAxis(byte 1, byte2) //TODO

//Sets found offsets to offset registers
void setOffsets()
{
  //Create holders for MSB and LSB and write to register for each axis
  /*
    uint16_t offsetxLSB = OFFSET_X & 0xFF;          // LSB
    uint16_t offsetxMSB = (OFFSET_X >> 8) & 0xFF;   // MSB
    writeRegister(MAGN_DATA_X_OFF, offsetxLSB);     // LSB
    writeRegister(MAGN_DATA_X2_OFF, offsetxMSB);    // MSB

    // Sett offset for Y-aksen
    uint16_t offsetyLSB = OFFSET_Y & 0xFF;          
uint16_t offsetyMSB = (OFFSET_Y >> 8) & 0xFF;   
    writeRegister(MAGN_DATA_Y_OFF, offsetyLSB);     
    writeRegister(MAGN_DATA_Y2_OFF, offsetyMSB);   

    // Sett offset for Z-aksen
    uint16_t offsetzLSB = OFFSET_Z & 0xFFe;
    uint16_t offsetzMSB = (OFFSET_Z >> 8) & 0xFF;   
    writeRegister(MAGN_DATA_Z_OFF, offsetzLSB);    
    writeRegister(MAGN_DATA_Z2_OFF, offsetzMSB);  
  */
}
