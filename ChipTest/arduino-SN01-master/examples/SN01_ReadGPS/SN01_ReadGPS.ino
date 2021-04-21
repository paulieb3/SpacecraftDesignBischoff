/*************************************************************
  This is an examples for the SN01
  GPS Module
  
  This examples read data from a NEO 6M GPS
    
  The sensor communicates over the I2C Bus.
  
*************************************************************/

#include "xSN01.h"

xSN01 SN01;

long tick_Print = 0;

void setup(){
	// Start the Serial comms 
	Serial.begin(115200);
  
	// Display what is being tested on serial monitor
	Serial.println("====================================");
	Serial.println("       XINABOX SN01 GPS Data        ");
	Serial.println("====================================");

#ifdef ESP8266
	// Start the I2C Communication
  Wire.begin(2, 14); // set I2C Pins for CW01
  Wire.setClockStretchLimit(15000);
#else
	// Start the I2C Communication
  Wire.begin();
#endif


	// Start the I2C Communication
	Wire.begin(); 
  
	// Start the Sensor
	SN01.begin();
}

void loop(){
	// Create a variable to store the data read from SN01
	String time;
	long latitude = 0;
	long longitude = 0;
	String date;
  
	// Poll the sensor to read all available data
	SN01.poll();
  
  	// Use a timer to print data once a second
  	if((millis() - tick_Print) > 1000){

  		// Get the date from GPS
		date = SN01.getDate();
		
		// Get the time from the GPS 
		time = SN01.getTime();

		// Get the latitude from GPS
		latitude = SN01.getLatitude();

		// Get the longitude from GPS
		longitude = SN01.getLongitude();
	  
		// Display the recorded data over the serial monitor
		Serial.print("GPS Time: ");
		Serial.println(time);
		Serial.print("GPS Date: ");
		Serial.println(date);
		Serial.print("GPS Latitude: ");
		Serial.println(latitude);
		Serial.print("GPS longitude: ");
		Serial.println(longitude);

		Serial.println();
		
		tick_Print = millis();
	}
}
