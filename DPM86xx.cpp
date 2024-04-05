//====================================================================================================================//
// File:          DPM86xx.cpp                                                                                         //
// Description:   DPM86xx implementation                                                                              //
// Author:        Tiderko                                                                                             //
//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//====================================================================================================================//

#include <Arduino.h>
#include <DPM86xx.h>

//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
DPM86xx::DPM86xx()
{
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void DPM86xx::init(HardwareSerial &clSerialIfR, uint8_t ubAddressV)
{
  //---------------------------------------------------------------------------------------------------
  // store reference to the provided interface
  //
  pclSeralP = &clSerialIfR;

  //---------------------------------------------------------------------------------------------------
  // setup address of PSU
  //
  if ((ubAddressV < 10) && (ubAddressV >= 1))
  {
    clAddressP = "0" + String(ubAddressV);
  }
  else if ((ubAddressV >= 10 && ubAddressV < 100))
  {
    clAddressP = String(ubAddressV);
  }
  else
  {
    clAddressP = "01";
  }

  //---------------------------------------------------------------------------------------------------
  // Calculate the maximal time to wait for a response
  // The time is based on 20 characters per 8 bits at the configured baud rate:
  //
  //  TimeToWait = 1000 * (20 * 8) / Baudrate + TimeOffset
  //
  // For 19200 Baud the wait time should be maximum 8,3ms + TimeOffset
  //
  uqResponseTimeP = (uint64_t)(1000 * 20 * 8);
  uqResponseTimeP /= (uint64_t)pclSeralP->baudRate();
  uqResponseTimeP += (uint64_t)20; // Time Offset
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
float DPM86xx::maxVoltage()
{
  float ftValueT = (float)uwMaxVoltageP;
  ftValueT /= 100;
  return ftValueT;
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
float DPM86xx::maxCurrent()
{
  float ftValueT = (float)uwMaxCurrentP;
  ftValueT /= 1000;
  return ftValueT;
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
float DPM86xx::measuredCurrent()
{
  float ftValueT = (float)uwMeasuredCurrentP;
  ftValueT /= 1000;
  return ftValueT;
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
float DPM86xx::measuredVoltage()
{
  float ftValueT = (float)uwMeasuredVoltageP;
  ftValueT /= 100;
  return ftValueT;
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool DPM86xx::constantOutputIsCurrent()
{
  bool btReturnT = false;
  if (uwConstantOutputP != 0)
  {
    btReturnT = true;
  }
  return btReturnT;
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool DPM86xx::constantOutputIsVoltage()
{
  bool btReturnT = true;
  if (uwConstantOutputP != 0)
  {
    btReturnT = false;
  }
  return btReturnT;
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
float DPM86xx::temperature()
{
  return (float)uwTemperatureP;
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
String DPM86xx::functionToString(Function_te teFunctionV)
{
  String clStringT = "";
  if (teFunctionV < 10)
  {
    clStringT += "0";
  }
  clStringT += String(teFunctionV);

  return clStringT;
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
uint16_t DPM86xx::functionValue(Function_te teFunctionV)
{
  uint16_t uwReturnT = 0xFFFF; // not valid value

  //---------------------------------------------------------------------------------------------------
  // return value, that corresponds to the function
  //
  switch (teFunctionV)
  {
  case eFUNC_MAX_VOLTAGE:
    uwReturnT = uwMaxVoltageP;
    break;
  case eFUNC_MAX_CURRENT:
    uwReturnT = uwMaxCurrentP;
    break;
  case eFUNC_MEASURED_CURRENT:
    uwReturnT = uwMeasuredCurrentP;
    break;
  case eFUNC_MEASURED_VOLTAGE:
    uwReturnT = uwMeasuredVoltageP;
    break;
  case eFUNC_CONSTANT_OUTPUT:
    uwReturnT = uwConstantOutputP;
    break;
  case eFUNC_TEMPERATURE:
    uwReturnT = uwTemperatureP;
    break;

  case eFUNC_SET_VOLTAGE:
  case eFUNC_SET_CURRENT:
  case eFUNC_OUTPUT_STATUS:
  case eFUNC_SET_VC:
  case eFUNC_WRITE_OK:
  case eFUNC_INVALID:
  default:
    break;
  }

  return uwReturnT;
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
bool DPM86xx::isNumber(const std::string &s)
{
  std::string::const_iterator it = s.begin();
  while (it != s.end() && std::isdigit(*it))
    ++it;
  return !s.empty() && it == s.end();
}
//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
DPM86xx::Function_te DPM86xx::parseResponse(char *pszBufferV, uint8_t ubLengthV)
{
  const char *SEP = "=";   // Separator between function member and operand (value)
  const char *BEGIN = ":"; // Separator for the begin of the response frame
  char *pszLeftT;
  char *pszRightT;
  char *pszStartT = nullptr;
  Function_te teReturnT = eFUNC_INVALID;
  String clFunctionT = "";
  uint32_t ulLengthT;

  //---------------------------------------------------------------------------------------------------
  // It can happen that the fault on the bus results in "fault data" before the actual response,
  // this is now removed and the length of the string is adjusted accordingly.
  //
  pszLeftT = strtok(pszBufferV, BEGIN);
  if (pszLeftT != nullptr)
  {
    pszStartT = pszLeftT;
  }
  pszRightT = strtok(NULL, BEGIN);
  if (pszRightT != nullptr)
  {
    pszStartT = pszRightT;
  }
  ulLengthT = String(pszStartT).length();

  //---------------------------------------------------------------------------------------------------
  // check the frame format is valid, define in manual
  // 1. The start symbol is ":".
  // 2. The final symbol is "." followed by '\r\n'
  //
  if (((String(pszStartT).indexOf('.')) != (ulLengthT - 3)) ||
      ((String(pszStartT).indexOf('\r')) != (ulLengthT - 2)) ||
      ((String(pszStartT).indexOf('\n')) != (ulLengthT - 1)))
  {
    //-------------------------------------------------------------------------------------------
    // it may be an write response, check for :01ok
    //
    if (((String(pszStartT).indexOf('o')) != (ulLengthT - 4)) ||
        ((String(pszStartT).indexOf('k')) != (ulLengthT - 3)) ||
        ((String(pszStartT).indexOf('\r')) != (ulLengthT - 2)) ||
        ((String(pszStartT).indexOf('\n')) != (ulLengthT - 1)))
    {
      //-----------------------------------------------------------------------------------
      // Error, Frame format does not match
      //
      teReturnT = eFUNC_INVALID;
    }
    else
    {
      teReturnT = eFUNC_WRITE_OK;
    }
  }

  //---------------------------------------------------------------------------------------------------
  // proceed with parsing
  //
  else if (pszStartT != nullptr)
  {
    //-------------------------------------------------------------------------------------------
    // Get everything to the left of the parameter separator
    //
    pszLeftT = strtok(pszStartT, SEP);

    //-------------------------------------------------------------------------------------------
    // Get what is to the right of the parameter separator
    //
    pszRightT = strtok(NULL, SEP);

    //-------------------------------------------------------------------------------------------
    // make sure both pointers are valid
    //
    if ((pszLeftT != nullptr) && (pszRightT != nullptr))
    {
      //-----------------------------------------------------------------------------------
      // remove dot at the end of the string
      //
      pszRightT[String(pszRightT).indexOf('.')] = '\0';

      //-----------------------------------------------------------------------------------
      // check value is a number
      //
      if (isNumber(pszRightT) != true)
      {
        Serial.print(pszRightT);
        Serial.println("  :::  Value is not a NUMBER!");
        teReturnT = eFUNC_INVALID;
      }
      else
      {
        clFunctionT = String(clAddressP + "r00");
        if (clFunctionT.equals(pszLeftT))
        {

          uwMaxVoltageP = (uint16_t)atoi(pszRightT);
          teReturnT = eFUNC_MAX_VOLTAGE;
        }

        clFunctionT = String(clAddressP + "r01");
        if (clFunctionT.equals(pszLeftT))
        {
          uwMaxCurrentP = (uint16_t)atoi(pszRightT);
          teReturnT = eFUNC_MAX_CURRENT;
        }

        clFunctionT = String(clAddressP + "r30");
        if (clFunctionT.equals(pszLeftT))
        {
          uwMeasuredVoltageP = (uint16_t)atoi(pszRightT);
          teReturnT = eFUNC_MEASURED_VOLTAGE;
        }

        clFunctionT = String(clAddressP + "r31");
        if (clFunctionT.equals(pszLeftT))
        {
          uwMeasuredCurrentP = (uint16_t)atoi(pszRightT);
          teReturnT = eFUNC_MEASURED_CURRENT;
        }

        clFunctionT = String(clAddressP + "r32");
        if (clFunctionT.equals(pszLeftT))
        {
          uwConstantOutputP = (uint16_t)atoi(pszRightT);
          teReturnT = eFUNC_CONSTANT_OUTPUT;
        }

        clFunctionT = String(clAddressP + "r33");
        if (clFunctionT.equals(pszLeftT))
        {
          uwTemperatureP = (uint16_t)atoi(pszRightT);
          teReturnT = eFUNC_TEMPERATURE;
        }
      }
    }
  }

  return teReturnT;
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
int32_t DPM86xx::readFunction(Function_te teFunctionV)
{
  int32_t slReturnT;

  //---------------------------------------------------------------------------------------------------
  // make sure write and read buffers are empty
  //
  pclSeralP->flush();
  delay(5);
  do
  {
    slReturnT = pclSeralP->read();
  } while (slReturnT >= 0);

  //---------------------------------------------------------------------------------------------------
  // Create the request frame
  //
  String clRequestFrameT = String(":" + clAddressP + "r" + functionToString(teFunctionV) + "=0,");

  //---------------------------------------------------------------------------------------------------
  // write request and read response
  //
  slReturnT = writeAndRead(clRequestFrameT);
  if (slReturnT > 0)
  {
    //-------------------------------------------------------------------------------------------
    // parse response frame
    //
    if (parseResponse(aszReceiveBufferP, slReturnT) != teFunctionV)
    {
      //-----------------------------------------------------------------------------------
      // error the response is not the expected one
      //
      slReturnT = eSTATUS_RESP_FRAME;
    }

    else
    {
      //-----------------------------------------------------------------------------------
      // success, get and return read value
      //
      slReturnT = (int32_t)functionValue(teFunctionV);
    }
  }

  return slReturnT;
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
int32_t DPM86xx::writeAndRead(String clFrameV)
{
  int32_t slReturnT = eSTATUS_RESP_TIMEOUT;
  int32_t slCharCounterT = 0;
  uint64_t ulWaitTimeT;

#ifdef DPM86XX_LOG_REQ_RESP
  Serial.print("REQ: ");
  Serial.println(clFrameV);
#endif

  //---------------------------------------------------------------------------------------------------
  // print provided frame to the UART
  //
  pclSeralP->println(clFrameV);

  //---------------------------------------------------------------------------------------------------
  // enter while loop for reception of the response
  //
  ulWaitTimeT = (millis() + uqResponseTimeP);
  while (millis() < ulWaitTimeT)
  {
    //-------------------------------------------------------------------------------------------
    // when a char has been received
    // 1. copy it to the receive buffer
    // 2. increase char counter
    // 3. check for final char and quit while loop
    //
    if (pclSeralP->available())
    {
      aszReceiveBufferP[slCharCounterT] = (char)pclSeralP->read();

      if (slCharCounterT < (DPM86XX_RECEIVE_BUFER_MAX - 1))
      {
        slCharCounterT++;
      }
      else
      {
        slReturnT = eSTATUS_RESP_BUFFER;
        break;
      }

      if (aszReceiveBufferP[slCharCounterT - 1] == '\n')
      {
        slReturnT = (int32_t)slCharCounterT;
        break;
      }
    }
  }

  //---------------------------------------------------------------------------------------------------
  // complete the string
  //
  aszReceiveBufferP[slCharCounterT] = '\0';

#ifdef DPM86XX_LOG_REQ_RESP
  Serial.print("RES(");
  Serial.print(slReturnT);
  Serial.print("):");
  Serial.println(aszReceiveBufferP);
#endif

  return slReturnT;
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
int32_t DPM86xx::writeFunction(Function_te teFunctionV, uint16_t uwValue1V, uint16_t uwValue2V)
{
  int32_t slReturnT;

  //---------------------------------------------------------------------------------------------------
  // make sure write and read buffers are empty
  //
  pclSeralP->flush();
  delay(5);
  do
  {
    slReturnT = pclSeralP->read();
  } while (slReturnT >= 0);

  //---------------------------------------------------------------------------------------------------
  // Create the request frame
  //
  String clRequestFrameT = String(":" + clAddressP + "w" + functionToString(teFunctionV) + "=" + String(uwValue1V) + ",");
  if (teFunctionV == eFUNC_SET_VC)
  {
    clRequestFrameT += String(String(uwValue2V) + ",");
  }

  //---------------------------------------------------------------------------------------------------
  // write request and read response
  //
  slReturnT = writeAndRead(clRequestFrameT);
  if (slReturnT > 0)
  {
    //-------------------------------------------------------------------------------------------
    // parse response frame
    //
    if (parseResponse(aszReceiveBufferP, slReturnT) != eFUNC_WRITE_OK)
    {
      //-----------------------------------------------------------------------------------
      // error the response is not the expected one
      //
      slReturnT = eSTATUS_RESP_FRAME;
    }

    else
    {
      slReturnT = eFUNC_WRITE_OK;
    }
  }

  return slReturnT;
}