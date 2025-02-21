/*                                                                              
 *  rockblockqueue.cpp                                                 
 *                                                                               
 *  Queue processing for sending messages through the rockblock to the
 *  user.                     
 *                                                                               
 *  Introduction                                              
 *  
 *  This code manages a queue of messages to be sent through the rockblock
 *  and on to the user through the iridium messaging system.
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
 *  v0.0   - First release
 */

#include <Arduino.h>
#include <IridiumSBD.h>
#include <string.h>

#include "icedrifter.h"
#include "rockblock.h"
#include "rockblockqueue.h"

rbqEntry rbqQueue[RBQ_SIZE];

dataChunk dcChunk;

int rbqProcessDataHumanReadable(icedrifterData *msgPtr);
int rbqProcessChunks(icedrifterData *msgPtr, uint16_t msgLen, uint8_t msgType);

char rbhexchars[] = "0123456789ABCDEF";

void rbqPrintHexChar(uint8_t x) {
  Serial.print(rbhexchars[(x >> 4)]);
  Serial.print(rbhexchars[(x & 0x0f)]);
}

int rbqFindEmptyEntry(void) {

  int i;

  for (i = 0; i < RBQ_SIZE; i++) {
    if (rbqQueue[i].msgType == RBQ_MSG_TYPE_FREE) {
      return (i);
    }
  }
#ifdef SERIAL_DEBUG_RB_QUEUE
  DEBUG_SERIAL.print(F("rbqFindEmptyEntry - Queue is full!!!\n"));
  delay(1000);
#endif // SERIAL_DEBUG
  return (-1);
}

int rbqTransmitQueued(void) {

  int i;
  int j;
  int rc;

  for (i = 0; i < RBQ_SIZE; i++) {
    if (rbqQueue[i].msgType != RBQ_MSG_TYPE_FREE) {

#ifdef SERIAL_DEBUG_RB_QUEUE
      DEBUG_SERIAL.print(F("Sending message:\n"));
      if (rbqQueue[i].msgType == RBQ_MSG_TYPE_CHAR) {
        DEBUG_SERIAL.print((char *)(rbqQueue[i].msg));
      } 
      for (j = 0; j < rbqQueue[i].msgLen; ++j) {
        rbqPrintHexChar(rbqQueue[i].msg[j]);
        DEBUG_SERIAL.print("\n");
      }
      delay(1000);
#endif // SERIAL_DEBUG_RB_QUEUE

      if ((rc = rbTransmit(&(rbqQueue[i].msg[0]), rbqQueue[i].msgLen, rbqQueue[i].msgType)) != ISBD_SUCCESS) {

#ifdef SERIAL_DEBUG_RB_QUEUE
        DEBUG_SERIAL.print(F("rbqTransmitQueued - rbTransmit returned error = "));
        DEBUG_SERIAL.print(rc);
        DEBUG_SERIAL.print(" returning.\n");
        delay(1000);
#endif // SERIAL_DEBUG

        return (rc);
      }

      rbqQueue[i].msgType = RBQ_MSG_TYPE_FREE;
    }
  }
  return (RBQ_GOOD);
}

int rbqInit(void) {

  int i;

#ifdef SERIAL_DEBUG_RB_QUEUE
  DEBUG_SERIAL.print(F("Initializing rockblockqueue\n"));
  delay(1000);
#endif // SERIAL_DEBUG

  for (i = 0; i < RBQ_SIZE; i++) {
    rbqQueue[i].msgType = RBQ_MSG_TYPE_FREE;
    rbqQueue[i].msgLen = 0;
    rbqQueue[i].msg[0] = 0;
  }

  return (RBQ_GOOD);
}
/*
int rbqReserveEntry(void) { 
  for (i = 0; i < RBQ_SIZE; i++) {

    if (rbqQueue[i].msg[0] == 0) {
      rbqQueue[i].msg = (void *)0xffffffff;
      rbqQueue[i].msgLen = 0;
      rbqQueue[i].msgType = MSG_TYPE_RESERVED;
      return(i);
    }
  }
  return(RBQ_QUEUE_FULL);
}

void rbqReleaseEntry(int entNbr) {
  if (rbqQueue[entNbr].msgPtr != (void*)0xffffffff) {
    return (RBQ_ENTRY_NOT_RSVD);
  }

  rbqQueue[entNbr].msgPtr = (void *)NULL;
  rbqQueue[entNbr].msgLen = 0;
  rbqQueue[i].msgType = MSG_TYPE_RESERVED;
    
*/
int rbqProcessMessage(uint8_t *msgPtr, uint16_t msgLen, uint8_t msgType) {

  int i;
  int j;
  int temp;

#ifdef SERIAL_DEBUG_RB_QUEUE
  DEBUG_SERIAL.print("msgPtr=");
  DEBUG_SERIAL.print((int)msgPtr, HEX);
  DEBUG_SERIAL.print("\n");
  delay(1000);
#endif // SERIAL_DEBUG_RB_QUEUE

  if (msgPtr == NULL) {

#ifdef SERIAL_DEBUG_RB_QUEUE
    DEBUG_SERIAL.print(F("msgPtr == NULL - returning\n"));
    delay(1000);
#endif // SERIAL_DEBUG

    return (RBQ_NO_MSG_ADDR);
  }

  if (msgType == RBQ_MSG_TYPE_CHAR) {
    if ((temp = strlen(msgPtr)) > MESSAGE_BUFF_SIZE) {

#ifdef SERIAL_DEBUG_RB_QUEUE  
    DEBUG_SERIAL.print(F("RBQ_MSG_TYPE_CHAR too long = "));
    DEBUG_SERIAL.print(temp);
    DEBUG_SERIAL.print(F(" - returning\n"));
    delay(1000);
#endif // SERIAL_DEBUG

      return (RBQ_CHAR_TOO_LONG);
    }

//    if (strchr(msgPtr, '\r') == NULL) {
//      return (RBQ_NO_CR_FOUND);
//    }

    if (i = rbqFindEmptyEntry() == -1) {
      return (RBQ_QUEUE_FULL);
    }

    rbqQueue[i].msgType = msgType;
    rbqQueue[i].msgLen = msgLen;
    rbqQueue[i].msg[0] = 0;
    strcat(rbqQueue[i].msg, msgPtr);

#ifdef SERIAL_DEBUG_RB_QUEUE
    DEBUG_SERIAL.print(F("Queueing message:\n"));
    if (rbqQueue[i].msgType == RBQ_MSG_TYPE_CHAR) {
      DEBUG_SERIAL.print((char *)(rbqQueue[i].msg));
    } 
    for (j = 0; j < rbqQueue[i].msgLen; ++j) {
      rbqPrintHexChar(rbqQueue[i].msg[j]);
      DEBUG_SERIAL.print("\n");
    }
    delay(1000);
#endif // SERIAL_DEBUG_RB_QUEUE

    return (RBQ_GOOD);

#ifdef HUMAN_READABLE_DISPLAY

  } else if ((msgType == RBQ_MSG_TYPE_SENSOR_BIN) && (msgLen == BASE_RECORD_LENGTH)) {

#ifdef SERIAL_DEBUG_RB_QUEUE
    DEBUG_SERIAL.print(F("Calling rbqProcessDataHumanReadable.\n"));
    delay(1000);
#endif // SERIAL_DEBUG

    return (rbqProcessDataHumanReadable((icedrifterData *)msgPtr));
#endif // HUMAN_READABLE_DISPLAY

  } else if (msgType == RBQ_MSG_TYPE_SENSOR_BIN || msgType == RBQ_MSG_TYPE_BIN) {

    if (msgLen > MESSAGE_BUFF_SIZE) {
      return (rbqProcessChunks((icedrifterData *)msgPtr, msgLen, msgType));
    }

    if (i = rbqFindEmptyEntry() == -1) {
      return (RBQ_QUEUE_FULL);
    }

    rbqQueue[i].msgType = msgType;
    rbqQueue[i].msgLen = msgLen;
    memmove(rbqQueue[i].msg, msgPtr, msgLen);
    return (RBQ_GOOD);

  } else {

    return (RBQ_INVALID_TYPE);

  }
}

int rbqTransmitQueueData(void) {

  int i;
  int j;
  int msgCount;

  msgCount = 0;

  rbInit();

  for (i = 0; i < RBQ_SIZE; i++) {
    if (rbqQueue[i].msgType != RBQ_MSG_TYPE_FREE) {

// Fix this code when text transmits works...
      if (rbqQueue[i].msgType == RBQ_MSG_TYPE_CHAR) {

#ifdef SERIAL_DEBUG
        DEBUG_SERIAL.print((char *)rbqQueue[i].msg);
        delay(1000);
#endif // SERIAL_DEBUG

        rbqQueue[i].msgLen = strlen(rbqQueue[i].msg + 1);
        rbqQueue[i].msgType = RBQ_MSG_TYPE_BIN;
      }
//////////////////////////////////////////////// 

#ifdef SERIAL_DEBUG
      if (rbqQueue[i].msgType == RBQ_MSG_TYPE_CHAR) {
        rbqQueue[i].msgLen = strlen(rbqQueue[i].msg) + 1;
      }
      for (j = 0; j < rbqQueue[i].msgLen; j++) {
        rbqPrintHexChar(rbqQueue[i].msg[j]);
      }
      delay(1000);
#endif // SERIAL_DEBUG

      rbTransmit(rbqQueue[i].msg, rbqQueue[i].msgLen, rbqQueue[i].msgType);
      rbqQueue[i].msgType = RBQ_MSG_TYPE_FREE;
      rbqQueue[i].msgLen = 0;
      rbqQueue[i].msg[0] = 0;
      ++msgCount;
    }
  }

  rbShutdown();
  return (msgCount);
}

int rbqProcessDataHumanReadable(icedrifterData *idPtr) {

  struct tm *timeInfo;
  int i;

  uint8_t oBuff[MESSAGE_BUFF_SIZE + 1];
  uint8_t *buffPtr;
  uint8_t buff[128];
  uint8_t rc;

#ifdef SERIAL_DEBUG_RB_QUEUE
  DEBUG_SERIAL.print("idPtr=");
  DEBUG_SERIAL.print((int)idPtr, HEX);
  DEBUG_SERIAL.print("\n");
  delay(1000);
#endif // SERIAL_DEBUG_RB_QUEUE

#define SERIAL_DEBUG_GMT

#ifdef SERIAL_DEBUG_GMT
  DEBUG_SERIAL.print("\nGMT debug=");
  DEBUG_SERIAL.print((int)idPtr->idGPSTime);
  DEBUG_SERIAL.print("\n");
#endif // SERIAL_DEBUG_ROCKBLOCK

  oBuff[0] = 0;
  strcat(oBuff, "\nGMT=");
  timeInfo = gmtime(idPtr->idGPSTime);
  buffPtr = asctime(timeInfo);
  strcat(oBuff, buffPtr);


#ifdef SERIAL_DEBUG_GMT
  DEBUG_SERIAL.print("\nGMT debug=");
  DEBUG_SERIAL.print((int)buffPtr);
  DEBUG_SERIAL.print("\n");
#endif // SERIAL_DEBUG_ROCKBLOCK

  strcat(oBuff, "\nLBT=");
  timeInfo = gmtime(idPtr->idLastBootTime);
  buffPtr = asctime(timeInfo);
  strcat(oBuff, buffPtr);
  strcat(oBuff, "\nLat=");
  buffPtr = dtostrf(idPtr->idLatitude, 4, 6, buff);
  strcat(oBuff, buffPtr);
  strcat(oBuff, "\nLon=");
  buffPtr = dtostrf(idPtr->idLongitude, 4, 6, buff);
  strcat(oBuff, buffPtr);
//    strcat(oBuff, "\nTmp=");
//    buffPtr = dtostrf(idPtr->idTemperature, 4, 2, buff);
//    strcat(oBuff, buffPtr);
  strcat(oBuff, "\nBP=");
  buffPtr = dtostrf(idPtr->idPressure, 6, 2, buff);
  strcat(oBuff, buffPtr);
  strcat(oBuff, " hPa\nTs=");

#ifdef SERIAL_DEBUG_RB_QUEUE
  DEBUG_SERIAL.print("idPtr->idRemoteTemp=");
  DEBUG_SERIAL.print((int)idPtr->idRemoteTemp, HEX);
  DEBUG_SERIAL.print("\n");
  delay(1000);
#endif // SERIAL_DEBUG_RB_QUEUE

  buffPtr = dtostrf(idPtr->idRemoteTemp, 4, 2, buff);
  strcat(oBuff, buffPtr);
  strcat(oBuff, " C = ");
  buffPtr = dtostrf(((idPtr->idRemoteTemp * 1.8) + 32), 4, 2, buff);
  strcat(oBuff, buffPtr);
  strcat(oBuff, " F\n");
  strcat(oBuff, "\nIcedrifter H/V ");
  strcat(oBuff, HARDWARE_VERSION);
  strcat(oBuff, " S/V ");
  strcat(oBuff, SOFTWARE_VERSION);

#ifdef SERIAL_DEBUG
  strcat(oBuff, "\n*** Debug is ON ***\n");
#endif // SERIAL_DEBUG

//  strcat(oBuff, "\r");

#ifdef SERIAL_DEBUG
  for (i = 0; oBuff[i] != 0x00; i++) {
    rbqPrintHexChar(oBuff[i]);
  }

  DEBUG_SERIAL.print("00\n");
  DEBUG_SERIAL.print((char *)&oBuff);
  delay(1000);
#endif // SERIAL_DEBUG


  if ((rc = rbInit()) != ISBD_SUCCESS) {
#ifdef SERIAL_DEBUG_RB_QUEUE
    DEBUG_SERIAL.print(F("Bad return code from rbInit = "));
    DEBUG_SERIAL.print(rc);
    DEBUG_SERIAL.print(" - returning!\n");
    delay(1000);
#endif // SERIAL_DEBUG  
    return (rc);
  }
// fix this!!!
//    if ((rc = rbTransmit(oBuff, strlen(oBuff) + 1, RBQ_MSG_TYPE_SENSOR_CHAR) != ISBD_SUCCESS)) {
  if ((rc = rbTransmit(oBuff, strlen(oBuff) + 1, RBQ_MSG_TYPE_SENSOR_BIN) != ISBD_SUCCESS)) {

#ifdef SERIAL_DEBUG_RB_QUEUE
    DEBUG_SERIAL.print(F("Bad return code from rbTransmit = "));
    DEBUG_SERIAL.print(rc);
    DEBUG_SERIAL.print(F(" - Shutting down and returning!\n"));
    delay(1000);
#endif // SERIAL_DEBUG  

    rbShutdown();
    return (rc);
  }

  rc = rbqTransmitQueued();
  rbShutdown();
  return (rc);
}

int rbqProcessChunks(icedrifterData *idPtr, uint16_t msgLen, uint8_t msgType) {

  int i;
  int recCount;
  uint8_t *dataPtr;
  uint8_t *chunkPtr;
  uint8_t *wkPtr;
  uint16_t chunkLen;
  uint16_t wkLen;
  int rc;

  recCount = 0;
  dataPtr = (uint8_t *)idPtr;
  chunkPtr = (uint8_t *)&dcChunk.dcBuffer;
  chunkLen = msgLen;

//#ifdef HUMAN_READABLE_DISPLAY
//  if (dataLen == BASE_RECORD_LENGTH) {
//    rbqProcessDataHumanReadable(idPtr, dataLen, dataType);
//#ifdef
//  if (dataLen <= MESSAGE_BUFF_SIZE) {
//    rc = rbqProcessMessage(idPtr, dataLen, dataType);
//#endif
//  } else {


  if ((rc = rbInit()) != ISBD_SUCCESS) {

    while (chunkLen > 0) {

      if (rbqFindEmptyEntry() == -1) {
        if (recCount == 0) {
          return (RBQ_QUEUE_FULL_0);
        } else if (recCount == 1) {
          return (RBQ_QUEUE_FULL_1);
        } else {
          return (RBQ_QUEUE_FULL_2);
        }
      }

      dcChunk.dcSendTime = idPtr->idGPSTime;
      dcChunk.dcRecordType[0] = 'I';
      dcChunk.dcRecordType[1] = 'D';
      dcChunk.dcRecordNumber = recCount;

      if (chunkLen > MAX_CHUNK_DATA_LENGTH) {
        wkLen = MAX_CHUNK_LENGTH;
        chunkLen -= MAX_CHUNK_DATA_LENGTH;
      } else {
        wkLen = (wkLen + CHUNK_HEADER_SIZE);
        chunkLen = 0;
      }

      memmove(chunkPtr, dataPtr, chunkLen);
      dataPtr += MAX_CHUNK_DATA_LENGTH;
      ++recCount;

#ifdef SERIAL_DEBUG
      DEBUG_SERIAL.flush();
      DEBUG_SERIAL.print(F("Chunk address="));
      DEBUG_SERIAL.print((long)chunkPtr, HEX);
      DEBUG_SERIAL.print(F(" Chunk length="));
      DEBUG_SERIAL.print(chunkLen);
      DEBUG_SERIAL.print(F("\n"));
      wkPtr = (uint8_t *)&dcChunk;

      for (i = 0; i < chunkLen; i++) {
        rbqPrintHexChar(wkPtr);
        ++wkPtr;
      }

      DEBUG_SERIAL.print(F("\n"));
#endif // SERIAL_DEBUG

      rc = rbqFindEmptyEntry();

      if (rc != RBQ_GOOD) {
        return (rc);
      }

      rbqQueue[i].msgLen = msgLen;
      rbqQueue[i].msgType = msgType;
      memmove(rbqQueue[i].msg, chunkPtr, chunkLen);
    }
    return (RBQ_GOOD);
  }

  return(rc);
}



  