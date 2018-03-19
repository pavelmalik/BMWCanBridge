#include "DueCANLayer.h"

// CAN Layer functions
extern byte canInit(byte cPort, long lBaudRate);
extern byte canTx(byte cPort, long lMsgID, bool bExtendedFormat, byte* cData, byte cDataLen);
extern byte canRx(byte cPort, long* lMsgID, bool* bExtendedFormat, byte* cData, byte* cDataLen);


byte CAR = 0;
byte NEW_MODULE = 1;

//Address of the bmw night vision module
long NVE_MODULE_ADDRESS = 0x6F1;
//Frame to clear
byte NVE_ERROR_CLEAR[] = {0x57, 0x03, 0x14, 0xFF, 0xFF, 0xFF, 0x00, 0x00};



long VIN_FRAME_ADDRESS = 0x380;

// Hex ascii values of the last 7 of the donor vin
// eg vin that ends in ...ABCDEFG would be 0x41 0x42 0x43 0x44 0x45 0x46 0x47
// EDIT THIS ARRAY WITH THE VIN INFO OF THE DONOR CAR
byte DONOR_VIN[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


//Throttle the vin sending to 120 sec
unsigned long SEND_VIN_RATE_MILLIS = 120 * 1000;

/**
   Clears error stored in the night vision module
*/
boolean clearNVECodes()
{
  if (canTx(NEW_MODULE, NVE_MODULE_ADDRESS, false, &NVE_ERROR_CLEAR[0], 8) != CAN_OK)
  {
    return false;
  }
  return true;
}

/**
   Sends the donor vehicle vin number to the new module
*/
boolean sendVin()
{
  if (canTx(NEW_MODULE, VIN_FRAME_ADDRESS, false, &DONOR_VIN[0], 7) != CAN_OK)
  {
    return false;
  }
  return true;
}

void setup()
{

  //Initialize both CAN controllers
  canInit(CAR, CAN_BPS_100K);
  canInit(NEW_MODULE, CAN_BPS_100K);
}


void loop()
{
  long lMsgID;
  bool bExtendedFormat;
  byte cRxData[8];
  byte cDataLen;

  unsigned long lastVinSentMillis = millis();
  unsigned long currentMillis;


  while (true) // Endless loop
  {
    currentMillis = millis();
    
    //Received traffic from the module, fwd to the car
    if (canRx(NEW_MODULE, &lMsgID, &bExtendedFormat, &cRxData[0], &cDataLen) == CAN_OK)
    {
      canTx(CAR, lMsgID, bExtendedFormat, &cRxData[0], cDataLen);

      //in case its been a while, resend the module vin
      if (abs(currentMillis - lastVinSentMillis) > SEND_VIN_RATE_MILLIS)
      {
        lastVinSentMillis = currentMillis;
        sendVin();
      }
    }

    //Received traffic from the car
    if (canRx(CAR, &lMsgID, &bExtendedFormat, &cRxData[0], &cDataLen) == CAN_OK)
    {
      //Non-vin frame, forward onto the new modue
      if (lMsgID != VIN_FRAME_ADDRESS)
      {
        canTx(NEW_MODULE, lMsgID, bExtendedFormat, &cRxData[0], cDataLen);
      }
      //Vin frame, intercept and send donor vin
      else
      {
        lastVinSentMillis = currentMillis;
        sendVin();
      }
    }

  }// end while

}// end loop

