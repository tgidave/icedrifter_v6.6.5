/*                                                                              
 *  gmessage.h                                                  
 *                                                                               
 *  Header file for implementing the gmessage processing.                     
 */                                                                              

#ifndef _GMESSAGE_H
  #define _GMESSAGE_H

//********************************************************
// If you change the value of this define you must also
// change the value of the same define in gtrackeri2c.h!!!
//********************************************************
#define GTRACKER_PERIPHERAL_I2C_ADDR 0x08
//********************************************************

#define GMSG_DONE         0
#define GMSG_NOT_READY    1
#define GMSG_BAD_SEND     2
#define GMSG_BAD_CHK_CHAR 3
#define GMSG_MSG_TOO_LONG 4

typedef struct i2cData {
  float maxX;
  float maxY;
  float maxZ;
  float maxMag;
  float maxMagX;
  float maxMagY;
  float maxMagZ;
}i2cData;

#define i2cDataSize 28

int gMsgProc(void);

#endif //_GMESSAGE_H
