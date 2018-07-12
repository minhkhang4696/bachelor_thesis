#include "CSensorSender.h"

void clientConnectedHandler(void * arg, AsyncClient * aClient)
{
	
}

void clientDisconnectedHandler(void * arg, AsyncClient * aClient)
{
	
}

void clientErrorHandler(void * arg, AsyncClient * aClient, unsigned char error)
{
	
}

void CSensorSender::populateSentString()
{
    mSentString = "";
    for (int i = 0; i < mNoOfSamples; i++) 
	{
        mSentString += String(mSensorData[i].normAccel) + "," + String(mSensorData[i].normGyro);
        if (i < mNoOfSamples - 1) 
		{
            mSentString += ",";
        }
		else
		{
			mSentString += "\n";
		}
    }
}

// Constructor
CSensorSender::CSensorSender(String IPString, int16_t port, uint16_t noOfSamplesPerPacket)
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
        Serial.print(mSentString);
        //Do the sending here

        //Hack
        mSenderState = READY;
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