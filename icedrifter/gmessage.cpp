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

//float endianSwap(float gForce) {
//
//  uint8_t temp;
//
//  union {
//    float gf;
//    uint8_t gx[4];
//  };
//
//  gf = gForce;
//  temp = gx[0];
//  gx[0] = gx[3];
//  gx[3] = temp;
//  temp = gx[1];
//  gx[1] = gx[2];
//  gx[2] = temp;
//  
//  return(gf); 
//}


int gMsgProc(void) {

  int i;
  int wireErr;
  int gDataLen;
  uint8_t *gDataPtr;
//  char *gMsgPtr;
//  uint16_t gMsgLen;
//  uint16_t gMsgBuffLen;
  int rbqReturnCode;
  bool gMsgDone = false;
  char wkFloat[16];

  if (gMsgDone == true) {
    return (GMSG_DONE);
  }

  Wire.begin();

#ifdef SERIAL_DEBUG
  DEBUG_SERIAL.print("\nTesting for the gtracker module.");
#endif // SERIAL_DEBUG

  Wire.beginTransmission(GTRACKER_PERIPHERAL_I2C_ADDR);
  wireErr = Wire.endTransmission();

#ifdef SERIAL_DEBUG
  DEBUG_SERIAL.print("\ngMsgProc Wire check returned ");
  DEBUG_SERIAL.print(wireErr);
  DEBUG_SERIAL.print("\n");
#endif // SERIAL_DEBUG

  Wire.end();

  if (wireErr != 0) {
    return (GMSG_NOT_READY);
  }

  Wire.begin();

  // Clear the receive buffer...
//  while (Wire.available()) {
//    Wire.read();
//  }

  gDataPtr = (uint8_t *)&gData;
//  gMsgLen = 0;


//    if (gMsgLen >= MESSAGE_BUFF_SIZE) {
//
//#ifdef SERIAL_DEBUG
//      DEBUG_SERIAL.print("\ngMsgProc returning GMSG_MSG_TOO_LONG!");
//#endif // SERIAL_DEBUG
//
//      return(GMSG_MSG_TOO_LONG);
//    }

  Wire.requestFrom(GTRACKER_PERIPHERAL_I2C_ADDR, i2cDataSize);
  while (!Wire.available());

#ifdef SERIAL_DEBUG
  DEBUG_SERIAL.print("\n");
#endif // SERIAL_DEBUG
  i = 0;

  while (i < i2cDataSize) {
    while (Wire.available()) {
      *(gDataPtr + i) = (uint8_t)Wire.read();
#ifdef SERIAL_DEBUG
      printHexChar((char)*(gDataPtr + i));
#endif // SERIAL_DEBUG
      i++;
    }
  }

#ifdef SERIAL_DEBUG
  DEBUG_SERIAL.print("\n");
#endif // SERIAL_DEBUG

  sprintf((char *)&gMsgBuff,
          "maxX = %.2f maxY = %.2f maxZ = %.2f g\nmaxMagX = %.2f maxMagY = %.2f maxMagZ = %.2f maxMag = %.2f g\n\r",
          gData.maxX, gData.maxY, gData.maxZ, gData.maxMagX, gData.maxMagY, gData.maxMagZ, gData.maxMag);

  gMsgBuff[0] = 0;

  strcat(gMsgBuff, "maxX = ");
  dtostrf(gData.maxX, 6, 2, wkFloat);
  strcat(gMsgBuff, wkFloat);
  strcat(gMsgBuff, "g\nmaxY = ");
  dtostrf(gData.maxY, 6, 2, wkFloat);
  strcat(gMsgBuff, wkFloat);
  strcat(gMsgBuff, "g\nmaxZ = ");
  dtostrf(gData.maxX, 6, 2, wkFloat);
  strcat(gMsgBuff, wkFloat);
  strcat(gMsgBuff, "g\nmaxMagX = ");
  dtostrf(gData.maxMagX, 6, 2, wkFloat);
  strcat(gMsgBuff, wkFloat);
  strcat(gMsgBuff, "g\nmaxMagY = ");
  dtostrf(gData.maxMagY, 6, 2, wkFloat);
  strcat(gMsgBuff, wkFloat);
  strcat(gMsgBuff, "g\nmaxMagY = ");
  dtostrf(gData.maxMagZ, 6, 2, wkFloat);
  strcat(gMsgBuff, wkFloat);
  strcat(gMsgBuff, "g\nmaxMag = ");
  dtostrf(gData.maxMag, 6, 2, wkFloat);
  strcat(gMsgBuff, wkFloat);
  strcat(gMsgBuff, "g\n\r");

#ifdef SERIAL_DEBUG
  DEBUG_SERIAL.print("\n");
  DEBUG_SERIAL.print(gMsgBuff);
  DEBUG_SERIAL.print("\n");
#endif // SERIAL_DEBUG

  Wire.end();

  if (rbqReturnCode = rbqProcessMessage(gMsgBuff, strlen(gMsgBuff), RBQ_MSG_TYPE_CHAR) == RBQ_GOOD) {
    gMsgDone == true;
    rbqTransmitQueueData(); //Send the data ASAP...
    return (GMSG_DONE);
  }

  return (rbqReturnCode);
}

