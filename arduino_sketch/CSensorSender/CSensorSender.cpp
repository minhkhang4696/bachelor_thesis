#include "CSensorSender.h"

#define DEBUG_PRINT 1

static inline void debugPrint(String inputString)
{
#if DEBUG_PRINT
    Serial.println(inputString);
#endif
}

void clientConnectedHandler(void* arg, AsyncClient* aClient)
{
    debugPrint("Connected");
    String sentString = ((CSensorSender*)arg)->getSentString();
    aClient->write(sentString.c_str());
    ((CSensorSender*)arg)->setIndicatorLed(LOW);
}

void clientDisconnectedHandler(void* arg, AsyncClient* aClient)
{
    debugPrint("Disconnected");
    senderState_t senderState = ((CSensorSender*)arg)->getSenderState();
    if ((senderState == SENDING) || (senderState == SENDING_ERROR)) {
        ((CSensorSender*)arg)->setSenderState(READY);
    }
    ((CSensorSender*)arg)->setIndicatorLed(HIGH);
}

void clientErrorHandler(void* arg, AsyncClient* aClient, unsigned char error)
{
    debugPrint("Error");
    senderState_t senderState = ((CSensorSender*)arg)->getSenderState();
    if (senderState == SENDING) {
        ((CSensorSender*)arg)->setSenderState(SENDING_ERROR);
    }
    ((CSensorSender*)arg)->setIndicatorLed(HIGH);
}

void CSensorSender::populateSentString()
{
    mSentString = "";
    for (int i = 0; i < mNoOfSamples; i++) {
        mSentString += String(mSensorData[i].normAccel) + "," + String(mSensorData[i].normGyro);
        if (i < mNoOfSamples - 1) {
            mSentString += ",";
        }
        else {
            mSentString += "\n";
        }
    }
}

// Constructor
CSensorSender::CSensorSender(String IPString, int16_t port, uint16_t noOfSamplesPerPacket, int ledIndicatorPin)
{
    // Load the arguments onto the attributes
    mIPString = IPString;
    mPort = port;
    mNoOfSamplesPerPacket = noOfSamplesPerPacket;
    // Allocate the exact number of elements of sensorData_t
    // this is going to be the array to store queued data
    mSensorData = new sensorData_t[mNoOfSamplesPerPacket];
    mSentString = "";
    mNoOfSamples = 0;
    // Initialize the state to READY
    mSenderState = READY;
    mLedIndicatorPin = ledIndicatorPin;
    pinMode(ledIndicatorPin, OUTPUT);
    digitalWrite(ledIndicatorPin, HIGH);
    // Initialize the ASyncClient
    mClient.onConnect(clientConnectedHandler, this);
    mClient.onError(clientErrorHandler, this);
    mClient.onDisconnect(clientDisconnectedHandler, this);
}

senderErrorCode_t CSensorSender::queueSensorData(sensorData_t& sensorDataInput)
{
    senderErrorCode_t returnSenderErrorCode = SENDER_ERROR_NOT_READY;
    // If there is still space left in the buffer
    if (mNoOfSamples < mNoOfSamplesPerPacket) {
        mSensorData[mNoOfSamples] = sensorDataInput;
        mNoOfSamples++;
        returnSenderErrorCode = SENDER_ERROR_OK;
    }
    else if (mSenderState == READY) {
        // Update the state
        mSenderState = SENDING;
        // Since this is a normal behavior
        returnSenderErrorCode = SENDER_ERROR_OK;
        populateSentString();
        mNoOfSamples = 0;
        debugPrint(mSentString);
        //Do the sending here
        mClient.connect(mIPString.c_str(), mPort);
    }
    else {
        returnSenderErrorCode = SENDER_ERROR_NOT_READY;
    }
    return returnSenderErrorCode;
}

// Destructor, clean up the allocated memory here
CSensorSender::~CSensorSender()
{
    // Deallocated allocated memory
    delete[] mSensorData;
}

String CSensorSender::getSentString()
{
    return mSentString;
}

senderState_t CSensorSender::getSenderState()
{
    return mSenderState;
}

void CSensorSender::setSenderState(senderState_t senderState)
{
    mSenderState = senderState;
}

void CSensorSender::setIndicatorLed(uint8_t ledState)
{
    digitalWrite(mLedIndicatorPin, ledState);
}