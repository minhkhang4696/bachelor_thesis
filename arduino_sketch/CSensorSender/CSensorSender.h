#ifndef sensorSender_h
#define sensorSender_h

#include <Event.h>
#include <Timer.h>
#include <AsyncPrinter.h>
#include <async_config.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncTCPbuffer.h>
#include "MPU6050.h"
#include "Wire.h"
#include "WiFiClient.h"
#include "ESP8266WiFiMulti.h"
#include "ESP.h"
#include "Time.h"

typedef struct
{
  float normAccel;
  float normGyro;
} sensorData_t;

typedef enum
{
  READY = 0,
  SENDING,
  SENDING_ERROR
} senderState_t;

typedef enum
{
  SENDER_ERROR_OK = 0,
  SENDER_ERROR_NOT_READY
} senderErrorCode_t;

class CSensorSender
{
  private:
    // Dynamically allocated sensor data array
    sensorData_t * mSensorData;
    // Current state of the sensor
    volatile senderState_t mSenderState;
    String mIPString;
    String mSentString;
    int16_t mPort;
    // Number of samples in a packet
    uint16_t mNoOfSamplesPerPacket;
    // Current number of samples in the array
    uint16_t mNoOfSamples;
    // Async TCP client for sending data
    AsyncClient mClient;
    
    void populateSentString();
	void disconnectedHandler(void * arg, AsyncClient * aClient);
	void errorHandler(void * arg, AsyncClient * aClient, uint8_t error);
  public:
    // Constructor
    CSensorSender(String IPString, int16_t port, uint16_t noOfSamplesPerPacket);
    senderErrorCode_t queueSensorData(sensorData_t& sensorDataInput);
	// Get + set function
	String getSentString();
	senderState_t getSenderState();
	void setSenderState(senderState_t senderState);
    // Destructor, clean up the allocated memory here
    ~CSensorSender();
};

#endif
