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
#include <CSensorSender.h>


ESP8266WiFiMulti WiFiMulti ;
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif
#define OUTPUT_READABLE_ACCELGYRO
//#define SERIAL_DEBUG
#define BUZZER 13
#define CONN_INTERVAL     (1000)
#define NUMB_ELEMS_PACKET (100)
#define SAMPL_INTV        (CONN_INTERVAL/NUMB_ELEMS_PACKET)
#define dataRate 9
#define lowAcc 0.35
#define highAcc 2.75
#define highGyro 254

Timer t;
const uint16_t port = 8000;
String host = "192.168.137.1"; // ip or dns

CSensorSender mySender(host, port, NUMB_ELEMS_PACKET, 2);

// Use WiFiClient class to create TCP connections
AsyncClient client;   
MPU6050 accelgyro;

//MPU6050 accelgyro(0x69); // <-- use for AD0 high
int16_t ax, ay, az;
int16_t gx, gy, gz;
float t_time = 0,send_time;
float rotX, rotY, rotZ,normAcc;
float gyroX, gyroY, gyroZ, normGyro;
String packet = "",packetSend = "";
int8_t i=0;
bool blinkState = false;
bool firstCond = false, secCond = false, thirdCond = false;

void Print_IMU()
{
  #ifdef SERIAL_DEBUG
    Serial.println("Accelerometer:");
    Serial.print(ax);
    Serial.print("\t");
    Serial.print(ay);
    Serial.print("\t");
    Serial.println(az);
    Serial.println("Gyroscope: ");
    Serial.print(gx);
    Serial.print("\t");
    Serial.print(gy);
    Serial.print("\t");
    Serial.println(gz);
  #endif
  Serial.print(normAcc);
  Serial.print(" ");
  Serial.println(normGyro);
}

void alert(){
  if (firstCond == true && secCond == true && thirdCond == true)
    {
      digitalWrite(BUZZER, HIGH);
    }
    firstCond = false;
    secCond = false;
    thirdCond = false;
}

void init_IMU() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();
    Serial.begin(115200);

    // initialize device
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
    Serial.print("\n");
    accelgyro.setDLPFMode(1); //set the Frequency to 1kHz
    accelgyro.setRate(dataRate); //Set data Rate: Rate = (Freq)/(dataRate+1)
    Serial.print("DLPF Mode: ");
    Serial.println(accelgyro.getDLPFMode());
    Serial.print("Data rate of sensor: ");
    Serial.println(accelgyro.getRate());
    // use the code below to change accel/gyro offset values
    Serial.println("Updating internal sensor offsets...");
    accelgyro.setXGyroOffset(55);
    accelgyro.setYGyroOffset(-28);
    accelgyro.setZGyroOffset(-2);
    accelgyro.setXAccelOffset(-2581);
    accelgyro.setYAccelOffset(-3937);
    accelgyro.setZAccelOffset(1199);
}


void Init_Wifi()
{
  // Initialize Wifi_connection
    WiFiMulti.addAP("MinhKhang", "qwertyuiop");
    Serial.println();
    Serial.println();
    Serial.print("Wait for WiFi... ");
      while(WiFiMulti.run() != WL_CONNECTED) {
            Serial.print(".");
            delay(500);
            ESP.wdtFeed();
        }
     Serial.println("");
     Serial.println("WiFi connected");
     Serial.println("IP address: ");
     Serial.println(WiFi.localIP());
     digitalWrite(2, LOW);
      delay(500);
}

void setup(){
  //initialize pin mode
  pinMode(BUZZER, OUTPUT);
  Serial.begin(115200);
  Wire.begin();
  init_IMU();
  Init_Wifi();
}

void loop() {
    sensorData_t currentSensorData;
    ESP.wdtFeed();
    if ((millis()-t_time)>SAMPL_INTV)
    { 
        //get data from sensor
        accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
        rotX = ax / 16384.0;
        rotY = ay / 16384.0;
        rotZ = az /16384.0;
        normAcc = sqrt(rotX*rotX + rotY*rotY + rotZ*rotZ);
        gyroX = gx /131.0;
        gyroY = gy /131.0;
        gyroZ = gz /131.0;
        normGyro = sqrt(gyroX*gyroX + gyroY*gyroY + gyroZ*gyroZ);
        //Start the comparator
        if (normAcc < lowAcc)
        {
          firstCond = true;
          t.after(500,alert);
        }
        if (firstCond == true && normAcc > highAcc)
        {
          secCond = true;
        }
        if (firstCond == true && normGyro > highGyro)
        {
          thirdCond =true;
        }
        t_time=millis();
        currentSensorData.normAccel = normAcc;
        currentSensorData.normGyro = normGyro;
        mySender.queueSensorData(currentSensorData);
    }
    t.update();
}

