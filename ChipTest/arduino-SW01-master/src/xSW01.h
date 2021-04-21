/*
	This is a library for the SW01
	DIGITAL HUMIDITY, PRESSURE AND TEMPERATURE SENSOR

	The board uses I2C for communication.
	
	The board communicates with the following I2C device:
	- 	BME280
	
	Data Sheets:
	BME280 - https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BME280_DS001-11.pdf
*/

#ifndef xSW01_h
#define xSW01_h

// System Includes
#include <inttypes.h>
#include <Arduino.h>
#include "xCore.h"
#include "math.h"

// Register Defines
#define BME280_REG_SOFTRESET   		0xE0

#define BME280_REG_CAL26     		0xE1
#define BME280_REG_CONTROLHUMID 	0xF2
#define BME280_REG_STATUS       	0XF3
#define BME280_REG_CONTROLMEASURE   0xF4
#define BME280_REG_CONFIG         	0xF5
#define BME280_REG_PRESSURE  		0xF7
#define BME280_REG_TEMP_MSB    		0xFA
#define BME280_REG_TEMP_CSB    		0xFB
#define BME280_REG_TEMP_LSB    		0xFC
#define BME280_REG_HUMID    		0xFD

#define BME280_DIG_T1_REG   		0x88
#define BME280_DIG_T2_REG   		0x8A
#define BME280_DIG_T3_REG   		0x8C
#define BME280_DIG_P1_REG   		0x8E
#define BME280_DIG_P2_REG   		0x90
#define BME280_DIG_P3_REG   		0x92
#define BME280_DIG_P4_REG   		0x94
#define BME280_DIG_P5_REG   		0x96
#define BME280_DIG_P6_REG   		0x98
#define BME280_DIG_P7_REG   		0x9A
#define BME280_DIG_P8_REG   		0x9C
#define BME280_DIG_P9_REG   		0x9E
    
#define BME280_DIG_H1_REG   		0xA1
#define BME280_DIG_H2_REG   		0xE1
#define BME280_DIG_H3_REG   		0xE3
#define BME280_DIG_H4_REG   		0xE4
#define BME280_DIG_H5_REG   		0xE5
#define BME280_DIG_H6_REG   		0xE7

// Oversampling Defines
#define BME_OSR_1					0x01
#define BME_OSR_2					0x02
#define BME_OSR_4					0x03
#define BME_OSR_8					0x04

// Device ID
#define BME280_REG_CHIPID      		0xD0
#define BME280_DEVICE_ID         	0x60

/*=========================================================================*/

struct BME280_Calibration_Data
{
    public:
    
			uint16_t dig_T1;
	    int16_t  dig_T2;
	    int16_t  dig_T3;

	    uint16_t dig_P1;
	    int16_t  dig_P2;
	    int16_t  dig_P3;
	    int16_t  dig_P4;
	    int16_t  dig_P5;
	    int16_t  dig_P6;
	    int16_t  dig_P7;
	    int16_t  dig_P8;
	    int16_t  dig_P9;

	    uint8_t  dig_H1;
	    int16_t  dig_H2;
	    uint8_t  dig_H3;
	    int16_t  dig_H4;
	    int16_t  dig_H5;
	    int8_t   dig_H6;
    
};

/*=========================================================================*/

class xSW01: public xCoreClass
{
    public:
		/**
		* Constructor
		* Creates a new instance of Sensor class.
		*/	
		xSW01();
		xSW01(uint8_t addr);
		/*
		* Runs the setup of the sensor. 
		* Call this in setup(), before reading any sensor data.
		*
		* @return true if setup was successful.
		*/
		bool 	begin();
		
		/*
		* Used to set a calibration offset for the temperature sensor. 
		* Call this in setup(), after begin().
		*
		* @param offset. Float value of temperature off.
		* @return none
		*/		
		void 	setTempCal(float offset);				
		
		/*
		* Used read all raw sensors data and convert to usefull results. 
		* Call this in loop(). Used to call sensor data reads.
		*
		* @return none
		*/
		void 	poll(void);                      
		
		/*
		* Used to get the temperature value in degress celcuis.
		* Call this in loop(). Used to get sensor temperature.
		*
		* @return temperature. Returns float value of temperature.
		*/		
		float 	getTempC(void);				
		
		/*
		* Used to get the temperature value in degress farenhied.
		* Call this in loop(). Used to get sensor temperature.
		*
		* @return temperature. Returns float value of temperature.
		*/		
		float 	getTempF(void);			
		
		/*
		* Used to get the relative humidity percantage
		* Call this in loop(). Used to get sensor humidity value.
		*
		* @return humidity. Returns float value in percentage form.
		*/		
		float 	getHumidity(void);
		
		/*
		* Used to get the pressure in pascals
		* Call this in loop(). Used to get sensor temperature.
		*
		* @return pressure. Returns flost value in pascals
		*/		
		float 	getPressure(void);               	// pressure in pascals
		
		/*
		* Used to get Altitude based on standard sea-level pressure
		* Call this in loop(). Used to get sensor altitude value.
		*
		* @return altitude. Returns float value in meters.
		*/		
		float 	getQNE(void);
		
		/*
		* Used to get the approxiamte altitude in meters
		* Call this in loop(). Used to get sensor altitude value.
		*
		* @param sea_level_pressure, input sea level pressure.
		* @return altitude. Returns float value in meters.
		*/		
		float 	getAltitude(float sea_level_pressure);
		
		/*
		* Used to get dew point in celcuis
		* Call this in loop(). Used to get sensor dew point value.
		*
		* @return dewpoint. Returns float value in celcuis.
		*/				
		float 	getDewPoint(void);	
		
	private:	

		/*
		* Structure
		* Stores calibration data.
		*/		
		BME280_Calibration_Data cal_data;		
    
		/*
		* Reads RAW temperature data.
		*
		* @return tempC
		*/		
		float readTemperature(void);
		
		/*
		* Reads RAW pressure data.
		*
		* @return pressure
		*/			
		float readPressure(void);
		
		/*
		* Reads RAW humidity data.
		*
		* @return humidity
		*/			
		float readHumidity(void);
		
		/*
		* Reads sensors coefficients for calibration.
		* Stores data in calibration data structure.
		*
		* @return none
		*/			
		void readSensorCoefficients(void);
    
		float   tempcal;							// stores the temp offset calibration
		int32_t t_fine;
		
		
		//Device I2C Address
		uint8_t BME280_I2C_ADDRESS;
};

#endif
