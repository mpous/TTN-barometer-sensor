#include <TheThingsNetwork.h>

#include "Barometer.h"
#include <Wire.h>

/* ARDUINO */
const char *appEui = "xxx";
const char *appKey = "yyy";

#define loraSerial Serial1
#define debugSerial Serial

// Replace REPLACE_ME with TTN_FP_EU868 or TTN_FP_US915
#define freqPlan TTN_FP_EU868

TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);


/* Barometer demo V1.0
* Based largely on code by  Jim Lindblom
* Get pressure, altitude, and temperature from the BMP085.
* By:http://www.seeedstudio.com
*/

float temperature;
float pressure;
float atm;
float altitude;

Barometer myBarometer;

int16_t int16_temperature, int16_atm, int16_pressure, int16_altitude, int16_vbat;

void setup()
{
  Serial.begin(9600);
  loraSerial.begin(57600);
  
  myBarometer.init();
  
  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000)
    ;

  // Set callback for incoming messages
  ttn.onMessage(message);

  debugSerial.println("-- STATUS");
  ttn.showStatus();

  debugSerial.println("-- JOIN");
  ttn.join(appEui, appKey);
  
}

void loop()
{
   temperature = myBarometer.bmp085GetTemperature(myBarometer.bmp085ReadUT()); //Get the temperature, bmp085ReadUT MUST be called first
   pressure = myBarometer.bmp085GetPressure(myBarometer.bmp085ReadUP());//Get the temperature
   altitude = myBarometer.calcAltitude(pressure); //Uncompensated caculation - in Meters 
   atm = pressure / 101325; 
  
  Serial.print("Temperature: ");
  Serial.print(temperature, 2); //display 2 decimal places
  Serial.println("deg C");

  Serial.print("Pressure: ");
  Serial.print(pressure, 0); //whole number only.
  Serial.println(" Pa");

  Serial.print("Ralated Atmosphere: ");
  Serial.println(atm, 4); //display 4 decimal places

  Serial.print("Altitude: ");
  Serial.print(altitude, 2); //display 2 decimal places
  Serial.println(" m");
  
  int16_temperature = temperature*100.0;
  int16_atm = atm*100.0;
  int16_pressure = pressure/10.0;
  int16_altitude = altitude*10.0;

  byte data[8];
 
    data[0] = (byte) (int16_temperature >> 8);
    data[1] = (byte) (int16_temperature & 0x00FF);
    data[2] = (byte) (int16_atm >> 8);
    data[3] = (byte) (int16_atm & 0x00FF);
    data[4] = (byte) (int16_pressure >> 8);
    data[5] = (byte) (int16_pressure & 0x00FF);
    data[6] = (byte) (int16_altitude >> 8);
    data[7] = (byte) (int16_altitude & 0x00FF);
  
  ttn.sendBytes(data, sizeof(data));

  Serial.println();

  delay(100000); //wait a second and get values again.
}


void message(const byte *payload, size_t length, port_t port)
{
  debugSerial.println("-- MESSAGE");

  // Only handle messages of a single byte
  if (length != 1)
  {
    return;
  }

  if (payload[0] == 0)
  {
    debugSerial.println("LED: off");
    digitalWrite(LED_BUILTIN, LOW);
  }
  else if (payload[0] == 1)
  {
    debugSerial.println("LED: on");
    digitalWrite(LED_BUILTIN, HIGH);
  }
}
