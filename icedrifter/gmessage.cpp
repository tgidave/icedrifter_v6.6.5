/*                                                                              
 *  gmessage.cpp                                                  
 *                                                                               
 *  Code to implement the communications between the icedrifter system and the gtracker system.                     
 *                                                                               
 *  Introduction                                              
 *  
 *  This code implements functionality that periodically contacts the gtracker
 *  code and, when the gtracker is ready, receives data accumulated by the
 *  gtracker to be sent through the iridium system to the user.
 *                                                                              
 *  Author                                                       
 *                                                                               
 *  Uncle Dave                                                  
 *                                                                               
 *  License                                                     
 *                                                                               
 *  Unknown (Talk to Cy)                                                        
 *                                                                               
 *  HISTORY                                                             
 *                                                                               
 *  v1.0   - First release
 *  
 */

#include <Wire.h>
#include "icedrifter.h"
#include "rockblockqueue.h"
#include "gmessage.h"

char gMsgBuff[MESSAGE_BUFF_SIZE];

i2cData gData;

int gMsgProc(void) {

  int i;
  int wireErr;
  uint8_t *gDataPtr;
  uint16_t gMsgLen;
  uint16_t gMsgBuffLen;
  int rbqReturnCode;
  bool gMsgDone = false;
  
  if (gMsgDone == true) {
    return(GMSG_DONE);
  }


  Wire.beginTransmission(GTRACKER_PERIPHERAL_I2C_ADDR);
  wireErr = Wire.endTransmission();

#ifdef SERIAL_DEBUG
  DEBUG_SERIAL.print("\ngMsgProc Wire check returned ");
  DEBUG_SERIAL.print(wireErr);
  DEBUG_SERIAL.print("\n");
#endif // SERIAL_DEBUG

  if (wireErr != 0) {
    return (GMSG_NOT_READY);
  }

  i = 0;

  Wire.requestFrom(GTRACKER_PERIPHERAL_I2C_ADDR, i2cDataSize);

  gDataPtr = (uint8_t *)(&gData);

  while ((i < i2cDataSize) && (Wire.available())) {
      *gDataPtr = (uint8_t)Wire.read();
      i++;
      gDataPtr++;
  }

  sprintf(gMsgBuff,
          "maxX = %.2f maxY = %.2f maxZ = %.2f g\nmaxMagX = %.2f maxMagY = %.2f maxMagZ = %.2f maxMag = %.2f g\n\r",
          gData.maxX, gData.maxY, gData.maxZ, gData.maxMagX, gData.maxMagY, gData.maxMagZ, gData.maxMag);

#ifdef SERIAL_DEBUG
  DEBUG_SERIAL.print("\n");
  DEBUG_SERIAL.print(gMsgBuff);
  DEBUG_SERIAL.print("\n");
#endif // SERIAL_DEBUG

  if (rbqReturnCode = rbqProcessMessage(gMsgBuff, strlen(gMsgBuff), RBQ_MSG_TYPE_SENSOR_CHAR) == RBQ_GOOD) {
    gMsgDone == true;
    return(GMSG_DONE);
  }

  return(rbqReturnCode);
}

