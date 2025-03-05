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

  if (rbType == RBQ_MSG_TYPE_BIN || rbType == RBQ_MSG_TYPE_SENSOR_BIN) {
    rc = isbd.sendSBDBinary(rbPtr, rbLen);
  } else {
    rc = isbd.sendSBDText(rbPtr);
  }

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
