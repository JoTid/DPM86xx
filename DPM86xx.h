//====================================================================================================================//
// File:          DPM86xx.h                                                                                           //
// Description:   DPM86xx Class definition                                                                            //
// Author:        Tiderko                                                                                             //
//--------------------------------------------------------------------------------------------------------------------//
//                                                                                                                    //
//====================================================================================================================//

#ifndef DPM86xx_h
#define DPM86xx_h

#include "Arduino.h"

/**
 * @brief Set this define to get Debug output for requests and responses via \c Serial interface.
 *
 */
#ifndef DPM86XX_LOG_REQ_RESP
#define DPM86XX_LOG_REQ_RESP
#endif

/**
 * @brief Maximal number of byte that are reserved for reception of response
 *
 */
#define DPM86XX_RECEIVE_BUFER_MAX 24

class DPM86xx
{
public:
  /**
   * @brief Definition of status values that are returned by readFunction() and writeFunction() methods
   */
  typedef enum Status_e
  {
    /**
     * @brief The response Frame is not the expected one or it cold not be parsed
     */
    eSTATUS_RESP_FRAME = -3,

    /**
     * @brief Number of received data exceeds the capacity of the receive buffer
     */
    eSTATUS_RESP_BUFFER = -2,

    /**
     * @brief  Timeout while waiting for reception
     */
    eSTATUS_RESP_TIMEOUT = -1,

    /**
     * @brief All OK
     */
    eSTATUS_OK = 0

  } Status_te;

  /**
   * @brief Supported function numbers used by readFunction() and writeFunction() methods
   */
  typedef enum Function_e
  {
    eFUNC_MAX_VOLTAGE = 0,
    eFUNC_MAX_CURRENT,

    eFUNC_SET_VOLTAGE = 10,
    eFUNC_SET_CURRENT,
    eFUNC_OUTPUT_STATUS,
    eFUNC_SET_VC = 20,

    eFUNC_MEASURED_VOLTAGE = 30,
    eFUNC_MEASURED_CURRENT,
    eFUNC_CONSTANT_OUTPUT,
    eFUNC_TEMPERATURE,

    eFUNC_WRITE_OK = 254,
    eFUNC_INVALID = 255
  } Function_te;

  /**
   * @brief Construct a new DPM86xx object
   */
  DPM86xx();

  /**
   * @brief Initialisation of object parameters
   *
   * @param[in] clSerialIfR interface, that should be used by this object
   */
  void init(HardwareSerial &clSerialIfR, uint8_t ubAddressV = 1);

  /**
   * @brief Read a value from PSU
   *
   * @param[in] teFunctionV number from \c #Function_e enumeration that should be read.
   * @return On success, a positive read value of the corresponding function is returned. On failure,
   *         a negative value of \c #Status_e is returned.
   *
   * The returned values are provided with fixed number of decimal digits, as define in PSU manual.
   */
  int32_t readFunction(Function_te teFunctionV);

  /**
   * @brief Write value to PSU
   *
   * @param[in] teFunctionV number from \c #Function_e enumeration that should be writte.
   * @param[in] uwValue1V first value that should be written.
   * @param[in] uwValue2V second value that should be written, only for \c #eFUNC_SET_VC function valid.
   * @return On success, \c #eFUNC_WRITE_OK is returned. On failure, a negative value of \c #Status_e is returned.
   *
   */
  int32_t writeFunction(Function_te teFunctionV, uint16_t uwValue1V, uint16_t uwValue2V = 0);

  /**
   * @brief Returns the maximum output voltage supported by the PSU
   * @return float value given in [V]
   *
   * Make sure that the \c #readFunction() has been successfully triggered with function number from \c #Function_e
   * that corresponds to this value.
   */
  float maxVoltage();

  /**
   * @brief Returns the maximum output current supported by the PSU
   * @return float value given in [A]
   *
   * Make sure that the \c #readFunction() has been successfully triggered with function number from \c #Function_e
   * that corresponds to this value.
   */
  float maxCurrent();

  /**
   * @brief Returns the measured output current
   * @return float value given in [A]
   *
   * Make sure that the \c #readFunction() has been successfully triggered with function number from \c #Function_e
   * that corresponds to this value.
   */
  float measuredCurrent();

  /**
   * @brief Returns the measured output voltage
   * @return float value given in [V]
   *
   * Make sure that the \c #readFunction() has been successfully triggered with function number from \c #Function_e
   * that corresponds to this value.
   */
  float measuredVoltage();

  /**
   * @brief Returns the indication the constant current output is pending
   * @return true if current is actually constant
   *
   * Make sure that the \c #readFunction() has been successfully triggered with function number from \c #Function_e
   * that corresponds to this value.
   */
  bool constantOutputIsCurrent();

  /**
   * @brief Returns the indication the constant voltage output is pending
   * @return true if voltage is actually constant
   *
   * Make sure that the \c #readFunction() has been successfully triggered with function number from \c #Function_e
   * that corresponds to this value.
   */
  bool constantOutputIsVoltage();

  /**
   * @brief Returns measured temperature
   * @return true if voltage is actually constant
   *
   * Make sure that the \c #readFunction() has been successfully triggered with function number from \c #Function_e
   * that corresponds to this value.
   */
  float temperature();

private:
  bool isNumber(const std::string &sclStringR);
  int32_t writeAndRead(const String clFrameV);
  Function_te parseResponse(char *pszBufferV, const uint8_t ubLengthV);
  String functionToString(const Function_te teFunctionV);
  uint16_t functionValue(const Function_te teFunctionV);

  HardwareSerial *pclSeralP;
  uint64_t uqResponseTimeP;
  String clAddressP;
  char aszReceiveBufferP[DPM86XX_RECEIVE_BUFER_MAX];

  //---------------------------------------------------------------------------------------------------
  // formatted values
  //
  uint16_t uwMaxVoltageP;
  uint16_t uwMaxCurrentP;
  uint16_t uwMeasuredCurrentP;
  uint16_t uwMeasuredVoltageP;
  uint16_t uwConstantOutputP;
  uint16_t uwTemperatureP;
};

#endif
