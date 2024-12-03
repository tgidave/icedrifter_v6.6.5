#include <Arduino.h>
#include <IridiumSBD.h>
#include <SoftwareSerial.h>

#include "icedrifter.h"
#include "rockblock.h"
#include "rockblockqueue.h"

SoftwareSerial isbdss(ROCKBLOCK_RX_PIN, ROCKBLOCK_TX_PIN);

IridiumSBD isbd(isbdss, ROCKBLOCK_SLEEP_PIN);

//iceDrifterChunk idcChunk;

#ifdef SERIAL_DEBUG_ROCKBLOCK
void ISBDConsoleCallback(IridiumSBD *device, char c) {
  DEBUG_SERIAL.write(c);
}

void ISBDDiagsCallback(IridiumSBD *device, char c) {
  DEBUG_SERIAL.write(c);
}
#endif

int rbInit(void) {

  int rc;
  char buff[128];

  // Setup the RockBLOCK
  isbd.setPowerProfile(IridiumSBD::USB_POWER_PROFILE);

#ifdef SERIAL_DEBUG_ROCKBLOCK
  DEBUG_SERIAL.flush();
  DEBUG_SERIAL.println(F("Powering up RockBLOCK\n"));
  DEBUG_SERIAL.flush();
#endif // SERIAL_DEBUG_ROCKBLOCK

  digitalWrite(ROCKBLOCK_POWER_PIN, HIGH);
  delay(1000);
                                                      
  isbdss.begin(ROCKBLOCK_BAUD);

  // Step 3: Start talking to the RockBLOCK and power it up
#ifdef SERIAL_DEBUG_ROCKBLOCK
  DEBUG_SERIAL.println(F("RockBLOCK begin\n"));
  delay(1000);
#endif // SERIAL_DEBUG_ROCKBLOCK
  isbdss.listen();

  if ((rc = isbd.begin()) != ISBD_SUCCESS) {
#ifdef SERIAL_DEBUG
    buff[0] = 0;
    DEBUG_SERIAL.print("Bad return code from usbd.begin = ");
    DEBUG_SERIAL.print(rc);
    DEBUG_SERIAL.print("\n");
#endif // SERIAL_DEBUG_ROCKBLOCK
    return(rc);
  }

  return(ISBD_SUCCESS);
}

int rbTransmit(uint8_t *rbPtr, uint16_t rbLen, uint8_t rbType) {

  int rc;

//  int recCount;
//  int dataLen;
//  int chunkLen;
//  int i;
//  uint8_t *dataPtr;
//  uint8_t *chunkPtr;
//  uint8_t *wkPtr;
//  uint8_t *msgBuffPtr;
//  struct tm *timeInfo;
//  char *buffPtr;
//  char buff[128];
//  char oBuff[340];

//if (dataLen == 0) {
//    oBuff[0] = 0;
//    strcat(oBuff, "\nGMT=");
//    timeInfo = gmtime(&idPtr->idGPSTime);
//    buffPtr = asctime(timeInfo);
//    strcat(oBuff, buffPtr);

//#define SERIAL_DEBUG_ROCKBLOCK_GMT

//#ifdef SERIAL_DEBUG_ROCKBLOCK_GMT
//    DEBUG_SERIAL.print("\nGMT debug=");
//    DEBUG_SERIAL.print(buffPtr);
//    DEBUG_SERIAL.print("\n");
//#endif // SERIAL_DEBUG_ROCKBLOCK

//    strcat(oBuff, "\nLBT=");
//    timeInfo = gmtime(&idPtr->idLastBootTime);
//    buffPtr = asctime(timeInfo);
//    strcat(oBuff, buffPtr);
//    strcat(oBuff, "\nLat=");
//    buffPtr = dtostrf(idPtr->idLatitude, 4, 6, buff);
//    strcat(oBuff, buffPtr);
//    strcat(oBuff, "\nLon=");
//    buffPtr = dtostrf(idPtr->idLongitude, 4, 6, buff);
//    strcat(oBuff, buffPtr);
//    strcat(oBuff, "\nTmp=");
//    buffPtr = dtostrf(idPtr->idTemperature, 4, 2, buff);
//    strcat(oBuff, buffPtr);
//    strcat(oBuff, "\nBP=");
//    buffPtr = dtostrf(idPtr->idPressure, 6, 2, buff);
//    strcat(oBuff, buffPtr);
//    strcat(oBuff, " hPa\nTs=");
//    buffPtr = dtostrf(idPtr->idRemoteTemp, 4, 2, buff);
//    strcat(oBuff, buffPtr);
//    strcat(oBuff, " C = ");
//    buffPtr = dtostrf(((idPtr->idRemoteTemp * 1.8) + 32), 4, 2, buff);
//    strcat(oBuff, buffPtr);
//    strcat(oBuff, " F\n");
//    strcat(oBuff, "\nIcedrifter H/V ");
//    strcat(oBuff, HARDWARE_VERSION);
//    strcat(oBuff, " S/V ");
//    strcat(oBuff, SOFTWARE_VERSION);

//#ifdef SERIAL_DEBUG_ROCKBLOCK
//    strcat(oBuff, "\n*** Debug is ON ***\n");
//    DEBUG_SERIAL.print((char *)&oBuff);
//    delay(1000);  
//#endif // SERIAL_DEBUG_ROCKBLOCK
//  }

//#ifdef NEVER_TRANSMIT
//  #ifdef SERIAL_DEBUG_ROCKBLOCK
//    DEBUG_SERIAL.print(F("Transmission disabled by NEVER_TRANSMIT switch.\n"));
//  #endif
//#else // NEVER_TRANSMIT

  // Setup the RockBLOCK
//  isbd.setPowerProfile(IridiumSBD::USB_POWER_PROFILE);
//#ifdef SERIAL_DEBUG_ROCKBLOCK
//  DEBUG_SERIAL.flush();
//  DEBUG_SERIAL.println(F("Powering up RockBLOCK\n"));
//  DEBUG_SERIAL.flush();
//#endif // SERIAL_DEBUG_ROCKBLOCK

//  digitalWrite(ROCKBLOCK_POWER_PIN, HIGH);
//  delay(1000);

//  isbdss.begin(ROCKBLOCK_BAUD);

  // Step 3: Start talking to the RockBLOCK and power it up
//#ifdef SERIAL_DEBUG_ROCKBLOCK
//  DEBUG_SERIAL.flush();
//  DEBUG_SERIAL.println(F("RockBLOCK begin\n"));
//  DEBUG_SERIAL.flush();
//  delay(1000);
//#endif // SERIAL_DEBUG_ROCKBLOCK
//  isbdss.listen();

//  if ((rc = isbd.begin()) == ISBD_SUCCESS) {
//#ifdef SERIAL_DEBUG_ROCKBLOCK
//    DEBUG_SERIAL.flush();
//    DEBUG_SERIAL.print(F("Transmitting address="));
//    DEBUG_SERIAL.print((long)idPtr, HEX);
//    DEBUG_SERIAL.print(F(" length="));
//    DEBUG_SERIAL.print(idLen);
//    DEBUG_SERIAL.print(F("\n"));
//    DEBUG_SERIAL.flush();
//    delay(1000);
//#endif // SERIAL_DEBUG_ROCKBLOCK

//    recCount = 0;
//    dataPtr = (uint8_t *)idPtr;
//    chunkPtr = (uint8_t *)&idcChunk.idcBuffer;
//    dataLen = idLen;

//    if (dataLen == 0) {
//      dataLen = strlen(oBuff) + 1;
//#ifdef SERIAL_DEBUG_ROCKBLOCK
//        DEBUG_SERIAL.flush();
//        DEBUG_SERIAL.print(F("Chunk address="));
//        DEBUG_SERIAL.print((long)chunkPtr, HEX);
//        DEBUG_SERIAL.print(F(" Chunk length="));
//        DEBUG_SERIAL.print(chunkLen);
//        DEBUG_SERIAL.print(F("\n"));
//        wkPtr = (uint8_t *)&idcChunk;
//        DEBUG_SERIAL.print(*wkPtr);
//        DEBUG_SERIAL.print(F("\n"));
//        DEBUG_SERIAL.flush();
//        delay(1000);
//#endif // SERIAL_DEBUG_ROCKBLOCK
//      rc = isbd.sendSBDBinary((const uint8_t *)oBuff, dataLen);
//
//    } else {
//
//      while (dataLen > 0) {
//        idcChunk.idcSendTime = idPtr->idGPSTime;
//        idcChunk.idcRecordType[0] = 'I';
//        idcChunk.idcRecordType[1] = 'D';
//        idcChunk.idcRecordNumber = recCount;
//
//        if (dataLen > MAX_CHUNK_DATA_LENGTH) {
//          chunkLen = MAX_CHUNK_LENGTH;
//          dataLen -= MAX_CHUNK_DATA_LENGTH;
//        } else {
//          chunkLen = (dataLen + CHUNK_HEADER_SIZE);
//          dataLen = 0;
//        }
//
//        memmove(chunkPtr, dataPtr, chunkLen);
//        dataPtr += MAX_CHUNK_DATA_LENGTH;
//        ++recCount;
//
//#ifdef SERIAL_DEBUG_ROCKBLOCK
//        DEBUG_SERIAL.flush();
//        DEBUG_SERIAL.print(F("Chunk address="));
//        DEBUG_SERIAL.print((long)chunkPtr, HEX);
//        DEBUG_SERIAL.print(F(" Chunk length="));
//        DEBUG_SERIAL.print(chunkLen);
//        DEBUG_SERIAL.print(F("\n"));
//        wkPtr = (uint8_t *)&idcChunk;
//  #ifdef dHUMAN_READABLE_DISPLAY
//        DEBUG_SERIAL.print(*wkPtr);
//  #else
//        for (i = 0; i < chunkLen; i++) {
//          rbprintHexChar((uint8_t)*wkPtr);
//          ++wkPtr;
//        }
//  #endif

//        DEBUG_SERIAL.print(F("\n"));
//        DEBUG_SERIAL.flush();
//#endif // SERIAL_DEBUG_ROCKBLOCK

  if (rbType == RBQ_MSG_TYPE_BIN || rbType == RBQ_MSG_TYPE_SENSOR_BIN) {
    rc = isbd.sendSBDBinary(rbPtr, rbLen);
  } else {
    rc = isbd.sendSBDText(rbPtr);
  }
//    }
//    }

//    if ((msgBuffPtr = (uint8_t *)checkForMessage()) != NULL) {
//      dataLen = strlen(msgBuffPtr) + 1;
//      rc = isbd.sendSBDBinary((const uint8_t*)msgBuffPtr, (strlen(msgBuffPtr) + 1));
//    }

#ifdef SERIAL_DEBUG_ROCKBLOCK
    if (rc == ISBD_SUCCESS) {
      DEBUG_SERIAL.print(F("Good return code from send!\n"));
    } else {
      DEBUG_SERIAL.print(F("Bad return code from send = "));
      DEBUG_SERIAL.print(rc);
      DEBUG_SERIAL.print(F("\n"));
      DEBUG_SERIAL.flush();
    }

    delay(1000);
#endif // SERIAL_DEBUG_ROCKBLOCK

    return(rc);
}

void rbShutdown(void) {
  isbd.sleep();
  isbdss.end();
  digitalWrite(ROCKBLOCK_POWER_PIN, LOW);
}

int rbTransmitDataNow(uint8_t *rbPtr, int rbLen, int rbType) {

  int rc;

  if((rc = rbInit()) == ISBD_SUCCESS) {
    rc = rbTransmit(rbPtr, rbLen, rbType);
  }
   
  rbShutdown();
  return(rc);
}
