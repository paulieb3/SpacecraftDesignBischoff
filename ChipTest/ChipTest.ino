// Code for this file was pulled from github - the arduno examples
#include <xSL01.h>
#include <si1133_config.h>
#include <SI01_CONFIG.h>
#include <xSI01.h>
#include <xSN01.h>
#include <xSW01.h>


#include <SSD1306init.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>
#include <xOD01.h>
#include <xCore.h>
#include <xVersion.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "eduroam";               // your network SSID (name)
const char* pass = "0118 Paulbischoff";             // your network password
WiFiClient client;

xSW01 SW01;
xSN01 SN01;
xSI01 SI01;
#define PRINT_SPEED 250
static unsigned long lastPrint = 0;
xSL01 SL01;


void setup() {
  Serial.begin(9600);
  Serial.println("Attempting to connect to WiFi");
  WiFi.begin(ssid, pass);
  while ( WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("...");
  }

  Serial.println("Connected to wifi");
  Serial.println("\nStarting connection with server...");

  // Set the I2C Pins for CW01
  #ifdef ESP8266
    Wire.pins(2, 14);
    Wire.setClockStretchLimit(15000);
  #endif
  Wire.begin();

  testSW01();
  testSN01();
  millis();
  testSI01();
  testSL01();
  Serial.println("DONE TESTING!");
}

void testSL01(){
  Serial.println("Testing SL01 :)");
  // begin sl01
  SL01.begin();
  delay(5000);
  // test sl01 and print output
  float lux;
  lux = 0;
  float uv;
  uv = 0;

  SL01.poll();

  lux = SL01.getLUX();
  Serial.print("Ambient Light Level: ");
  Serial.print(lux);
  Serial.println(" LUX");

  uv = SL01.getUVA();
  Serial.print("UVA Intersity: ");
  Serial.print(uv);
  Serial.println(" uW/m^2");

  uv = SL01.getUVB();
  Serial.print("UVB Intensity: ");
  Serial.print(uv);
  Serial.println(" uW/m^2");

  uv = SL01.getUVIndex();
  Serial.print("UVB Index: ");
  Serial.println(uv);
  Serial.println();
}

void testSI01(){
  Serial.println("Testing SI01 :)");
  // begin si01
  SI01.begin();
  delay(5000);
  // test si01 and print output
  if ( (lastPrint + PRINT_SPEED) < millis()) {
    printGyro();  // Print "G: gx, gy, gz"
    printAccel(); // Print "A: ax, ay, az"
    printMag();   // Print "M: mx, my, mz"
    printAttitude(); // Print Roll, Pitch and G-Force
    Serial.println();
    lastPrint = millis(); // Update lastPrint time
  }
}
void printGyro(void) {
  Serial.print("G: ");
  Serial.print(SI01.getGX(), 2);
  Serial.print(", ");
  Serial.print(SI01.getGY(), 2);
  Serial.print(", ");
  Serial.println(SI01.getGZ(), 2);
}
void printAccel(void) {
  Serial.print("A: ");
  Serial.print(SI01.getAX(), 2);
  Serial.print(", ");
  Serial.print(SI01.getAY(), 2);
  Serial.print(", ");
  Serial.println(SI01.getAZ(), 2);
}
void printMag(void) {
  Serial.print("M: ");
  Serial.print(SI01.getMX(), 2);
  Serial.print(", ");
  Serial.print(SI01.getMY(), 2);
  Serial.print(", ");
  Serial.println(SI01.getMZ(), 2);
}
void printAttitude(void) {
  Serial.print("Roll: ");
  Serial.println(SI01.getRoll(), 2);
  Serial.print("Pitch :");
  Serial.println(SI01.getPitch(), 2);
  Serial.print("GForce :");
  Serial.println(SI01.getGForce(), 2);
}

void testSN01(){
  Serial.println("Testing SN01 :)");
  // begin sn01
  SN01.begin();
  delay(5000);
  // Test sn01
  long tick_Print = 0;
  String time;
  long latitude = 0;
  long longitude = 0;
  String date;

  SN01.poll();

  if((millis() - tick_Print) > 1000){
    date = SN01.getDate();
    time = SN01.getTime();
    latitude = SN01.getLatitude();
    longitude = SN01.getLongitude();

    // print output
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


void testSW01(){
  Serial.println("Testing SW01 :)");
  // begin sw01
  SW01.begin();
  delay(5000);
  // Test sw01
  float alt;
  alt = 0;
  float humidity;
  humidity = 0;
  float pressure;
  pressure = 0;
  float temperature;
  temperature = 0;
  String sw01_output = "";

  SW01.poll();
  alt = SW01.getAltitude(101325);
  humidity = SW01.getHumidity();
  pressure = SW01.getPressure();
  temperature = SW01.getTempF();

  // print output
  Serial.println("SW01:");
  Serial.print("Altitude: ");
  Serial.print(alt);
  Serial.println(" m");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
  Serial.print("Pressure: ");
  Serial.print(pressure);
  Serial.println(" Pa");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" F");
  Serial.println();
}



void loop() {

}
