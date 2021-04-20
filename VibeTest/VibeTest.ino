#include <xCore.h>
#include <xSI01.h>


xSI01 SI01;

#define PRINT_SPEED 250
static unsigned long lastPrint = 0;

void setup() {
  // Start the Serial Monitor at 115200 BAUD
  Serial.begin(115200);
  
  // Set the I2C Pins for CW01
  #ifdef ESP8266
    Wire.pins(2, 14);
    Wire.setClockStretchLimit(15000);
  #endif

  Wire.begin();
    
  if (!SI01.begin()) {
    Serial.println("Failed to communicate with SI01.");
    Serial.println("Check the Connector");
  } else {
    Serial.println("start successful");
  }
  millis();
}

void loop() {
  // Read and calculate data from SI01 sensor
  SI01.poll();
  float vib=(sqrt(sq(SI01.getAX())+sq(SI01.getAY())+sq(SI01.getAZ()))-1)*10.0;

  if ( (lastPrint + PRINT_SPEED) < millis()) {
    printAccel(); // Print "A: ax, ay, az"
    Serial.println();
    //client.add("vibrations", vib);
    //client.sendAll(true);
    lastPrint = millis(); // Update lastPrint time
  }
}

void printAccel(void) {
  Serial.println((sqrt(sq(SI01.getAX())+sq(SI01.getAY())+sq(SI01.getAZ()))-1)*10.0);
}
