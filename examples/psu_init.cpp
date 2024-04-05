//====================================================================================================================//
// File:          psu_init.cpp                                                                                        //
// Description:   Example how to init an use DPM86xx PSU                                                              //
// Author:        Tiderko                                                                                             //
//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//====================================================================================================================//

/*--------------------------------------------------------------------------------------------------------------------*\
** include files                                                                                                      **
**                                                                                                                    **
\*--------------------------------------------------------------------------------------------------------------------*/
#include <DPM86xx.h>

/**
 * @brief global declaration of PSU
 *
 */
DPM86xx clPsuG;

//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void setup()
{
  //---------------------------------------------------------------------------------------------------
  // Setup UART Baud of PSU
  //
  Serial2.begin(9600);

  //---------------------------------------------------------------------------------------------------
  // init serial interface and PSU
  //
  clPsuG.init(Serial2);

  //---------------------------------------------------------------------------------------------------
  // Read maximal supported current of the PSU, this value corresponds also to the model number
  //
  int32_t slMaxCurrentT = clPsuG.readFunction(DPM86xx::eFUNC_MAX_VOLTAGE);
  if (slMaxCurrentT >= 0)
  {
    Serial.print("Maximal current of PSU is: ");
    Serial.println(slMaxCurrentT);
  }
  else
  {
    Serial.print("Read fail with error: ");
    Serial.println(slMaxCurrentT);
  }
}

//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//                                                                                                                    //
//--------------------------------------------------------------------------------------------------------------------//
void loop()
{

  //---------------------------------------------------------------------------------------------------
  // switch the PSU ON
  //
  int32_t slStatusT = clPsuG.writeFunction(DPM86xx::eFUNC_OUTPUT_STATUS, 1);
  if (slStatusT != DPM86xx::eFUNC_WRITE_OK)
  {
    Serial.print("Write fail with error: ");
    Serial.println(slStatusT);
  }

  //---------------------------------------------------------------------------------------------------
  // wait for 2 sec
  //
  delay(2000);

  //---------------------------------------------------------------------------------------------------
  // read measured voltage value from PSU
  //
  int32_t slMeasuredVoltageT = clPsuG.readFunction(DPM86xx::eFUNC_MAX_VOLTAGE);
  if (slMeasuredVoltageT >= 0)
  {
    Serial.print("Measured voltage value: ");
    Serial.println(slMeasuredVoltageT);
  }
  else
  {
    Serial.print("Read fail with error: ");
    Serial.println(slMeasuredVoltageT);
  }

  //---------------------------------------------------------------------------------------------------
  // switch the PSU OFF
  //
  slStatusT = clPsuG.writeFunction(DPM86xx::eFUNC_OUTPUT_STATUS, 0);
  if (slStatusT != DPM86xx::eFUNC_WRITE_OK)
  {
    Serial.print("Write fail with error: ");
    Serial.println(slStatusT);
  }

  //---------------------------------------------------------------------------------------------------
  // wait for 2 sec
  //
  delay(2000);
}
