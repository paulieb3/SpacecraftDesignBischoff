/*************************************************************
	This is an examples for the SW01
	Digital Temperature, Pressure and Humidity Sensor
	
	You can buy one on our store!
	-----> https://xinabox.cc/products/SW01
	
	This example request a pressure meausrement to be 
	made by the SW01 and display the data over the Serial
	bus.
	
	Supported on the all ☒CHIPs
	
	The sensor communicates over the I2C Bus.
	
	------------------------TIPS--------------------------
	Comment out this line ----->Wire.pins(2, 14); for support
	on other devices
	
*************************************************************/

#include <xCore.h>
#include <xSW01.h>

xSW01 SW01;

const int DELAY_TIME = 1000;

void setup(){
	// Start the Serial Monitor
	Serial.begin(115200);

	// Set the I2C Pins for CW01
	#ifdef ESP8266
	  Wire.pins(2, 14);
	  Wire.setClockStretchLimit(15000);
	#endif

	// Start the I2C Comunication
	Wire.begin();
	
	// Start the  SW01 Sensor
	SW01.begin();
	
	//Delay for sensor to normalise
	delay(5000);	
	
	Serial.println("================================");
	Serial.println("     XINABOX SW01 Pressure      ");
	Serial.println("================================");	
}

void loop(){
	// Create a variable to store the data read from SW01
	float pressure;
	pressure = 0;
 
  	// Read and calculate data from SW01 sensor
  	SW01.poll();
  
	// Request SW01 to get the pressure measurement and store in
	// the pressure	variable		
	pressure = SW01.getPressure();
	
	// Display the recoreded data over the Serial Monitor	
	Serial.print("Pressure: ");
	Serial.print(pressure);
	Serial.println(" Pa");

  	// Small delay between sensor reads  
	delay(DELAY_TIME);
}
