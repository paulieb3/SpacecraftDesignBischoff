/*
	This is a library for the SN01 
	u-blox Neo-6 GPS

	The board uses I2C for communication.
	
	The board communicates with one I2C devices:
	- NEO-6
	
	Data Sheets:
	u-blox NEO-6 - https://www.u-blox.com/sites/default/files/products/documents/NEO-6_DataSheet_(GPS.G6-HW-09005).pdf
*/

#ifndef xSN01_h
#define xSN01_h

// System Include
#include <inttypes.h>
#include "xCore.h"
#include <stdio.h>
#include <string.h>
#include <limits.h>

#define _GPS_VERSION "1.0.2" // software version of this library
#define _GPS_MPH_PER_KNOT 1.15077945
#define _GPS_MPS_PER_KNOT 0.51444444
#define _GPS_KMPH_PER_KNOT 1.852
#define _GPS_MILES_PER_METER 0.00062137112
#define _GPS_KM_PER_METER 0.001
#define _GPS_FEET_PER_METER 3.2808399
#define _GPS_MAX_FIELD_SIZE 15

struct RawDegrees
{
	uint16_t deg;
	uint32_t billionths;
	bool negative;
public:
	RawDegrees() : deg(0), billionths(0), negative(false)
	{}
};

struct SN01_LOCATION
{
   	friend class xSN01;
public:
	bool isValid() const    { return valid; }
	bool isUpdated() const  { return updated; }
	uint32_t age() const    { return valid ? millis() - lastCommitTime : (uint32_t)ULONG_MAX; }
	const RawDegrees &rawLat()     { updated = false; return rawLatData; }
	const RawDegrees &rawLng()     { updated = false; return rawLngData; }
	double lat();
	double lng();

	SN01_LOCATION() : valid(false), updated(false)
	{}

private:
	bool valid, updated;
	RawDegrees rawLatData, rawLngData, rawNewLatData, rawNewLngData;
	uint32_t lastCommitTime;
	void commit();
	void setLatitude(const char *term);
	void setLongitude(const char *term);
};

struct SN01_DATE
{
   	friend class xSN01;
public:
	bool isValid() const       { return valid; }
	bool isUpdated() const     { return updated; }
	uint32_t age() const       { return valid ? millis() - lastCommitTime : (uint32_t)ULONG_MAX; }

	uint32_t value()           { updated = false; return date; }
	uint16_t year();
	uint8_t month();
	uint8_t day();

	SN01_DATE() : valid(false), updated(false), date(0)
	{}

private:
	bool valid, updated;
	uint32_t date, newDate;
	uint32_t lastCommitTime;
	void commit();
	void setDate(const char *term);
};

struct SN01_TIME
{
   	friend class xSN01;
public:
	bool isValid() const       { return valid; }
	bool isUpdated() const     { return updated; }
	uint32_t age() const       { return valid ? millis() - lastCommitTime : (uint32_t)ULONG_MAX; }

	uint32_t value()           { updated = false; return time; }
	uint8_t hour();
	uint8_t minute();
	uint8_t second();
	uint8_t centisecond();

	SN01_TIME() : valid(false), updated(false), time(0)
	{}

private:
	bool valid, updated;
	uint32_t time, newTime;
	uint32_t lastCommitTime;
	void commit();
	void setTime(const char *term);
};

struct SN01_DECIMAL
{
   	friend class xSN01;
public:
	bool isValid() const    { return valid; }
	bool isUpdated() const  { return updated; }
	uint32_t age() const    { return valid ? millis() - lastCommitTime : (uint32_t)ULONG_MAX; }
	int32_t value()         { updated = false; return val; }

	SN01_DECIMAL() : valid(false), updated(false), val(0)
	{}

private:
	bool valid, updated;
	uint32_t lastCommitTime;
	int32_t val, newval;
	void commit();
	void set(const char *term);
};

struct SN01_INTERGER
{
   	friend class xSN01;
public:
	bool isValid() const    { return valid; }
	bool isUpdated() const  { return updated; }
	uint32_t age() const    { return valid ? millis() - lastCommitTime : (uint32_t)ULONG_MAX; }
	uint32_t value()        { updated = false; return val; }

	SN01_INTERGER() : valid(false), updated(false), val(0)
	{}

private:
	bool valid, updated;
	uint32_t lastCommitTime;
	uint32_t val, newval;
	void commit();
	void set(const char *term);
};

struct SN01_SPEED : SN01_DECIMAL
{
	double knots()    { return value() / 100.0; }
	double mph()      { return _GPS_MPH_PER_KNOT * value() / 100.0; }
	double mps()      { return _GPS_MPS_PER_KNOT * value() / 100.0; }
	double kmph()     { return _GPS_KMPH_PER_KNOT * value() / 100.0; }
};

struct SN01_COURSE : public SN01_DECIMAL
{
   	double deg()      { return value() / 100.0; }
};

struct SN01_ALTITUDE : SN01_DECIMAL
{
	double meters()       { return value() / 100.0; }
	double miles()        { return _GPS_MILES_PER_METER * value() / 100.0; }
	double kilometers()   { return _GPS_KM_PER_METER * value() / 100.0; }
	double feet()         { return _GPS_FEET_PER_METER * value() / 100.0; }
};

struct SN01_HDOP : SN01_DECIMAL
{
   	double hdop() { return value() / 100.0; }
};

class xSN01;

class SN01_CUSTOM
{
public:
	SN01_CUSTOM() {};
	SN01_CUSTOM(xSN01 &gps, const char *sentenceName, int termNumber);
	void begin(xSN01 &gps, const char *_sentenceName, int _termNumber);

	bool isUpdated() const  { return updated; }
	bool isValid() const    { return valid; }
	uint32_t age() const    { return valid ? millis() - lastCommitTime : (uint32_t)ULONG_MAX; }
	const char *value()     { updated = false; return buffer; }

private:
	void commit();
	void set(const char *term);

	char stagingBuffer[_GPS_MAX_FIELD_SIZE + 1];
	char buffer[_GPS_MAX_FIELD_SIZE + 1];
	unsigned long lastCommitTime;
	bool valid, updated;
	const char *sentenceName;
	int termNumber;
	friend class xSN01;
	SN01_CUSTOM *next;
};

class xSN01: public xCoreClass
{
	public:
		xSN01();
		bool encode(char c); // process one character received from GPS
		xSN01 &operator << (char c) {encode(c); return *this;}

		bool begin(void);

		bool dataAvailable(uint16_t &bytes);
		void readGPS(void);
		void readStream(uint16_t numBytes);

		/*
		* Polls the sensor to check for new data
		* Call this in loop(), before reading any sensor data
		*/	
		void poll(void);						

		/*
		* GPS time
		* Call this in loop().
		*
		* @return Time 
		*/	
		String getTime(void);	

		/*
		* GPS Date
		* Call this in loop().
		*
		* @return Date
		*/	
		String getDate(void);									


		/*
		* GPS Latitude 
		* Call this in loop().
		*
		* @return true if setup was successful.
		*/
		float getLatitude(void);					

		/*
		* GPS Longitude
		* Call this in loop().
		*
		* @return Longitude
		*/	
		float getLongitude(void);					

		/*
		* GPS  Altitude
		* Call this in loop().
		*
		* @return Altitude
		*/	
		float getAltitude(void);					

		/*
		* GPS Horizontal Dilution of Precision
		* Call this in loop().
		*
		* @return HDOP
		*/	
		float getHDOP(void);			

		/*
		* GPS Speed over Ground
		* Call this in loop().
		*
		* @return SoG
		*/	
		float getSOG(void);				

		/*
		* GPS Course over Ground  
		* Call this in loop().
		*
		* @return CoG
		*/	
		float getCOG(void);												

		/*
		* GPS number of satelittes is connected to
		* Call this in loop().
		*
		* @return NumSats.
		*/	
		int getSatelitesConnected(void);	

		SN01_LOCATION location;
		SN01_DATE date;
		SN01_TIME time;
		SN01_SPEED speed;
		SN01_COURSE course;
		SN01_ALTITUDE altitude;
		SN01_INTERGER satellites;
		SN01_HDOP hdop;

		static const char *libraryVersion() { return _GPS_VERSION; }

		static double distanceBetween(double lat1, double long1, double lat2, double long2);
		static double courseTo(double lat1, double long1, double lat2, double long2);
		static const char *cardinal(double course);

		static int32_t parseDecimal(const char *term);
		static void parseDegrees(const char *term, RawDegrees &deg);

		uint32_t charsProcessed()   const { return encodedCharCount; }
		uint32_t sentencesWithFix() const { return sentencesWithFixCount; }
		uint32_t failedChecksum()   const { return failedChecksumCount; }
		uint32_t passedChecksum()   const { return passedChecksumCount; }

	private:

	  	unsigned long tick;
  		unsigned long looptick;

		String 	getDate(SN01_DATE &d);
		String 	getTime(SN01_TIME &t);
		String 	getFloat(float val, bool valid, int len, int prec);
		String	getInt(unsigned long val, bool valid, int len);

		enum {GPS_SENTENCE_GPGGA, GPS_SENTENCE_GPRMC, GPS_SENTENCE_OTHER};

		// parsing state variables
		uint8_t parity;
		bool isChecksumTerm;
		char term[_GPS_MAX_FIELD_SIZE];
		uint8_t curSentenceType;
		uint8_t curTermNumber;
		uint8_t curTermOffset;
		bool sentenceHasFix;

		// custom element support
		friend class SN01_CUSTOM;
		SN01_CUSTOM *customElts;
		SN01_CUSTOM *customCandidates;
		void insertCustom(SN01_CUSTOM *pElt, const char *sentenceName, int index);

		// statistics
		uint32_t encodedCharCount;
		uint32_t sentencesWithFixCount;
		uint32_t failedChecksumCount;
		uint32_t passedChecksumCount;

		// internal utilities
		int fromHex(char a);
		bool endOfTermHandler();
};

#endif // def(__TinyGPSPlus_h)

