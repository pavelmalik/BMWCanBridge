// ------------------------------------------------------------------------
// Due CAN Layer - Created for easier interfacing
// ------------------------------------------------------------------------
#include "DueCANLayer.h"

// ------------------------------------------------------------------------
// Initialize the CAN Controller
// ------------------------------------------------------------------------
// cPort = 0/1 - other numbers will be ignored
// lBaudRate according to due_can.h
//
// Returns CAN_OK/CAN_ERROR
//
byte canInit(byte cPort, long lBaudrate)
{
  // Declarations
  byte cRetCode = CAN_ERROR;
  
  // Initialize the desired CAN port
  if(cPort == 0)
  {   
    cRetCode = Can0.begin(lBaudrate, ENABLE_PIN_CAN0);

  }
  else if(cPort == 1)
  {
    cRetCode = Can1.begin(lBaudrate, ENABLE_PIN_CAN1);
  }

  // Allow all filters after Init was successful
  if(cRetCode == CAN_OK)
  {
    if(cPort == 0)
      Can0.watchFor();
    else
      Can1.watchFor();
  }// end if

  return cRetCode;
  
}// end canInit

// ------------------------------------------------------------------------
// Transmit CAN message frame
// ------------------------------------------------------------------------
// cPort = 0/1 - other numbers will be ignored
//
// Returns CAN_OK/CAN_ERROR
//
byte canTx(byte cPort, long lMsgID, bool bExtendedFormat, byte* cData, byte cDataLen)
{
  // Declarations
  byte cRetCode = CAN_ERROR;
  CAN_FRAME frameCANMsg;

  // Port the message
  frameCANMsg.id = lMsgID;
  frameCANMsg.extended = bExtendedFormat;
  frameCANMsg.length = cDataLen;

  // Copy the data, making sure there are 8 data bytes total
  byte cDataCopy[8];
  for(byte cIndex = 0; cIndex < 8; cIndex++)
  {
    if(cIndex < cDataLen)
      cDataCopy[cIndex] = cData[cIndex];
    else
      cDataCopy[cIndex] = 0x00;

  }// end for

  frameCANMsg.data.low = (long)cDataCopy[0] + ((long)cDataCopy[1] << 8) + ((long)cDataCopy[2] << 16) + ((long)cDataCopy[3] << 24);
  frameCANMsg.data.high = (long)cDataCopy[4] + ((long)cDataCopy[5] << 8) + ((long)cDataCopy[6] << 16) + ((long)cDataCopy[7] << 24);
  
  // Send the message
  if(cPort == 0)       
    cRetCode = Can0.sendFrame(frameCANMsg);
  else
    cRetCode = Can1.sendFrame(frameCANMsg);

  return cRetCode;

}// end canTx

// ------------------------------------------------------------------------
// Receive CAN message frame
// ------------------------------------------------------------------------
// cPort = 0/1 - other numbers will be ignored
//
// Returns CAN_OK/CAN_ERROR
//
byte canRx(byte cPort, long* lMsgID, bool* bExtendedFormat, byte* cData, byte* cDataLen)
{
  // Declarations
  byte cLen = 0;
  byte cRetCode = CAN_ERROR;
  CAN_FRAME frameIncoming;
  
  // Check the desired port
  if(cPort == 0)
  {  
    cLen = Can0.available();
    if(cLen > 0)
    {
      Can0.read(frameIncoming);
    }

  }// end if
  else if (cPort == 1)
  {  
    cLen = Can1.available();
    if(cLen > 0)
      Can1.read(frameIncoming);

  }// end else

  *lMsgID = frameIncoming.id;
  *bExtendedFormat = frameIncoming.extended;
  
  if(cLen > 0)
  {
    // Copy the data
    cData[0] = (byte)(frameIncoming.data.low & 0x000000FF);
    cData[1] = (byte)((frameIncoming.data.low & 0x0000FF00) >> 8);
    cData[2] = (byte)((frameIncoming.data.low & 0x00FF0000) >> 16);
    cData[3] = (byte)((frameIncoming.data.low & 0xFF000000) >> 24);
  
    cData[4] = (byte)(frameIncoming.data.high & 0x000000FF);
    cData[5] = (byte)((frameIncoming.data.high & 0x0000FF00) >> 8);
    cData[6] = (byte)((frameIncoming.data.high & 0x00FF0000) >> 16);
    cData[7] = (byte)((frameIncoming.data.high & 0xFF000000) >> 24);

    cRetCode = CAN_OK;
    *cDataLen = frameIncoming.length;

  }// end if

  return cRetCode;

}// end canRx


