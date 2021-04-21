/*
	This is a library for the SN01 
	u-blox Neo-6 GPS

	The board uses I2C for communication.
	
	The board communicates with one I2C devices:
	- NEO-6
	
	Data Sheets:
	u-blox NEO-6 - https://www.u-blox.com/sites/default/files/products/documents/NEO-6_DataSheet_(GPS.G6-HW-09005).pdf
*/

#include <xSN01.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define _GPRMCterm   "GPRMC"
#define _GPGGAterm   "GPGGA"
#define _GNRMCterm   "GNRMC"
#define _GNGGAterm   "GNGGA"

xSN01::xSN01()
  :  parity(0)
  ,  isChecksumTerm(false)
  ,  curSentenceType(GPS_SENTENCE_OTHER)
  ,  curTermNumber(0)
  ,  curTermOffset(0)
  ,  sentenceHasFix(false)
  ,  customElts(0)
  ,  customCandidates(0)
  ,  encodedCharCount(0)
  ,  sentencesWithFixCount(0)
  ,  failedChecksumCount(0)
  ,  passedChecksumCount(0)
{
  term[0] = '\0';
}

/*-------------------Public Function---------------------*/

bool xSN01::begin(void)
{
	if(xCore.ping(0x42))
	{
		return true;
	}
	return false;
}

void xSN01::poll(void)
{
	readGPS();
}

/********************************************************
 	Check if data is available from SN01
*********************************************************/
bool xSN01::dataAvailable(uint16_t &bytes)
{
	uint16_t numBytes = 0;
	// check if timeout on DDC
	if ( (millis() - tick) > 200 ){
		// check how many bytes available to read
		Wire.beginTransmission(0x42);
		Wire.write(0xFD);
		Wire.endTransmission();
		Wire.requestFrom(0x42, 2);    // request number of bytes available

		numBytes = Wire.read();
		numBytes <<= 8;
		numBytes |= Wire.read();

		//Serial.println(numBytes);

		if ((numBytes > 0) && (numBytes < 10000))
  		{
			bytes = numBytes;
			tick = millis();
			return true;
		}

		bytes = numBytes;

		// GPS must be read contineously
		tick = millis();
	}
	return false;	
}

/********************************************************
 	Read GPS data over I2C
********************************************************/
void xSN01::readGPS(void)
{
	uint16_t numBytes = 0;
	// check if timeout on DDC
	if ( (millis() - tick) > 100 ){
		// check how many bytes available to read
		Wire.beginTransmission(0x42);
		Wire.write(0xFD);
		Wire.endTransmission();
		Wire.requestFrom(0x42, 2);
		numBytes = Wire.read();
		numBytes <<= 8;
		numBytes |= Wire.read();

		if ((numBytes > 0) && (numBytes < 10000)){
			readStream(numBytes);
		}
		// GPS must be read contineously
		tick = millis();
	}
}

/********************************************************
 	Read GPS Stream over I2C
********************************************************/
void xSN01::readStream(uint16_t numBytes)
{
  	if ((numBytes > 0) && (numBytes < 10000))
  	{
   		char c = 0;

    	Wire.beginTransmission(0x42);
    	Wire.write(0xFF);
    	Wire.endTransmission();

    for ( int i = 0; i < numBytes; i++ ) {
      	Wire.requestFrom(0x42, 1);
      	c = Wire.read();
		encode(c);
    }
  }
}

/********************************************************
 	Request Time UTC from GPS
*********************************************************/
String xSN01::getTime(void)
{
	String tempData = getTime(time);
	return tempData;
}

/********************************************************
 	Request altitude from GPS
*********************************************************/
float xSN01::getAltitude(void)
{
	String tempData = getFloat(altitude.meters(), altitude.isValid(), 7, 2);
	return atof(tempData.c_str());
}

/********************************************************
 	Request Latitude from GPS
*********************************************************/
float xSN01::getLatitude(void)
{
	String tempData = getFloat(location.lat(), location.isValid(), 11, 6);
	return atof(tempData.c_str());
}

/********************************************************
 	Request longitude from GPS
*********************************************************/
float xSN01::getLongitude(void)
{
	String tempData = getFloat(location.lng(), location.isValid(), 12, 6);
	return atof(tempData.c_str());
}

/********************************************************
 	Request date from GPS
*********************************************************/
String xSN01::getDate(void)
{
	String tempData = getDate(date);
	return tempData;
}

/********************************************************
 	Request horizontal dilution of precision from GPS
*********************************************************/
float xSN01::getHDOP(void)
{
	String tempData = getFloat(hdop.hdop(), hdop.isValid(), 6, 1);
	return atof(tempData.c_str());
}

/********************************************************
 	Request speed over ground from GPS
*********************************************************/
float xSN01::getSOG(void)
{
	String tempData = getFloat(speed.kmph(), speed.isValid(), 6, 2);
	return atof(tempData.c_str());
}					
		
/********************************************************
 	Request course over ground from GPS
*********************************************************/
float xSN01::getCOG(void)
{
	String tempData = getFloat(course.deg(), course.isValid(), 7, 2);
	return atof(tempData.c_str());
}						
 
/********************************************************
 	Request number of satelittes connected from GPS
*********************************************************/
int xSN01::getSatelitesConnected(void)
{
	String tempData =getInt(satellites.value(), satellites.isValid(), 5);
	return atoi(tempData.c_str());
}	

/********************************************************
 	Proccess GPS Info
*********************************************************/

bool xSN01::encode(char c)
{
	++encodedCharCount;

	switch(c)
	{
	case ',': // term terminators
		parity ^= (uint8_t)c;
	case '\r':
	case '\n':
	case '*':
		{
		bool isValidSentence = false;
		if (curTermOffset < sizeof(term))
		{
			term[curTermOffset] = 0;
			isValidSentence = endOfTermHandler();
		}
		++curTermNumber;
		curTermOffset = 0;
		isChecksumTerm = c == '*';
		return isValidSentence;
		}
		break;

	case '$': // sentence begin
		curTermNumber = curTermOffset = 0;
		parity = 0;
		curSentenceType = GPS_SENTENCE_OTHER;
		isChecksumTerm = false;
		sentenceHasFix = false;
		return false;

	default: // ordinary characters
		if (curTermOffset < sizeof(term) - 1)
		term[curTermOffset++] = c;
		if (!isChecksumTerm)
		parity ^= c;
		return false;
	}

	return false;
}

//
// internal utilities
//
int xSN01::fromHex(char a)
{
  if (a >= 'A' && a <= 'F')
    return a - 'A' + 10;
  else if (a >= 'a' && a <= 'f')
    return a - 'a' + 10;
  else
    return a - '0';
}

// static
// Parse a (potentially negative) number with up to 2 decimal digits -xxxx.yy
int32_t xSN01::parseDecimal(const char *term)
{
	bool negative = *term == '-';
	if (negative) ++term;
	int32_t ret = 100 * (int32_t)atol(term);
	while (isdigit(*term)) ++term;
	if (*term == '.' && isdigit(term[1]))
	{
		ret += 10 * (term[1] - '0');
		if (isdigit(term[2]))
		ret += term[2] - '0';
	}
	return negative ? -ret : ret;
}

// static
// Parse degrees in that funny NMEA format DDMM.MMMM
void xSN01::parseDegrees(const char *term, RawDegrees &deg)
{
	uint32_t leftOfDecimal = (uint32_t)atol(term);
	uint16_t minutes = (uint16_t)(leftOfDecimal % 100);
	uint32_t multiplier = 10000000UL;
	uint32_t tenMillionthsOfMinutes = minutes * multiplier;

	deg.deg = (int16_t)(leftOfDecimal / 100);

	while (isdigit(*term))
		++term;

	if (*term == '.')
		while (isdigit(*++term))
		{
		multiplier /= 10;
		tenMillionthsOfMinutes += (*term - '0') * multiplier;
		}

	deg.billionths = (5 * tenMillionthsOfMinutes + 1) / 3;
	deg.negative = false;
}

#define COMBINE(sentence_type, term_number) (((unsigned)(sentence_type) << 5) | term_number)

// Processes a just-completed term
// Returns true if new sentence has just passed checksum test and is validated
bool xSN01::endOfTermHandler()
{
	// If it's the checksum term, and the checksum checks out, commit
	if (isChecksumTerm)
	{
		byte checksum = 16 * fromHex(term[0]) + fromHex(term[1]);
		if (checksum == parity)
		{
		passedChecksumCount++;
		if (sentenceHasFix)
			++sentencesWithFixCount;

		switch(curSentenceType)
		{
		case GPS_SENTENCE_GPRMC:
			date.commit();
			time.commit();
			if (sentenceHasFix)
			{
			location.commit();
			speed.commit();
			course.commit();
			}
			break;
		case GPS_SENTENCE_GPGGA:
			time.commit();
			if (sentenceHasFix)
			{
			location.commit();
			altitude.commit();
			}
			satellites.commit();
			hdop.commit();
			break;
		}

		// Commit all custom listeners of this sentence type
		for (SN01_CUSTOM *p = customCandidates; p != NULL && strcmp(p->sentenceName, customCandidates->sentenceName) == 0; p = p->next)
			p->commit();
		return true;
		}

		else
		{
		++failedChecksumCount;
		}

		return false;
	}

	// the first term determines the sentence type
	if (curTermNumber == 0)
	{
		if (!strcmp(term, _GPRMCterm) || !strcmp(term, _GNRMCterm))
		curSentenceType = GPS_SENTENCE_GPRMC;
		else if (!strcmp(term, _GPGGAterm) || !strcmp(term, _GNGGAterm))
		curSentenceType = GPS_SENTENCE_GPGGA;
		else
		curSentenceType = GPS_SENTENCE_OTHER;

		// Any custom candidates of this sentence type?
		for (customCandidates = customElts; customCandidates != NULL && strcmp(customCandidates->sentenceName, term) < 0; customCandidates = customCandidates->next);
		if (customCandidates != NULL && strcmp(customCandidates->sentenceName, term) > 0)
		customCandidates = NULL;

		return false;
	}

	if (curSentenceType != GPS_SENTENCE_OTHER && term[0])
		switch(COMBINE(curSentenceType, curTermNumber))
	{
		case COMBINE(GPS_SENTENCE_GPRMC, 1): // Time in both sentences
		case COMBINE(GPS_SENTENCE_GPGGA, 1):
		time.setTime(term);
		break;
		case COMBINE(GPS_SENTENCE_GPRMC, 2): // GPRMC validity
		sentenceHasFix = term[0] == 'A';
		break;
		case COMBINE(GPS_SENTENCE_GPRMC, 3): // Latitude
		case COMBINE(GPS_SENTENCE_GPGGA, 2):
		location.setLatitude(term);
		break;
		case COMBINE(GPS_SENTENCE_GPRMC, 4): // N/S
		case COMBINE(GPS_SENTENCE_GPGGA, 3):
		location.rawNewLatData.negative = term[0] == 'S';
		break;
		case COMBINE(GPS_SENTENCE_GPRMC, 5): // Longitude
		case COMBINE(GPS_SENTENCE_GPGGA, 4):
		location.setLongitude(term);
		break;
		case COMBINE(GPS_SENTENCE_GPRMC, 6): // E/W
		case COMBINE(GPS_SENTENCE_GPGGA, 5):
		location.rawNewLngData.negative = term[0] == 'W';
		break;
		case COMBINE(GPS_SENTENCE_GPRMC, 7): // Speed (GPRMC)
		speed.set(term);
		break;
		case COMBINE(GPS_SENTENCE_GPRMC, 8): // Course (GPRMC)
		course.set(term);
		break;
		case COMBINE(GPS_SENTENCE_GPRMC, 9): // Date (GPRMC)
		date.setDate(term);
		break;
		case COMBINE(GPS_SENTENCE_GPGGA, 6): // Fix data (GPGGA)
		sentenceHasFix = term[0] > '0';
		break;
		case COMBINE(GPS_SENTENCE_GPGGA, 7): // Satellites used (GPGGA)
		satellites.set(term);
		break;
		case COMBINE(GPS_SENTENCE_GPGGA, 8): // HDOP
		hdop.set(term);
		break;
		case COMBINE(GPS_SENTENCE_GPGGA, 9): // Altitude (GPGGA)
		altitude.set(term);
		break;
	}

	// Set custom values as needed
	for (SN01_CUSTOM *p = customCandidates; p != NULL && strcmp(p->sentenceName, customCandidates->sentenceName) == 0 && p->termNumber <= curTermNumber; p = p->next)
		if (p->termNumber == curTermNumber)
			p->set(term);

	return false;
}

/* static */
double xSN01::distanceBetween(double lat1, double long1, double lat2, double long2)
{
	// returns distance in meters between two positions, both specified
	// as signed decimal-degrees latitude and longitude. Uses great-circle
	// distance computation for hypothetical sphere of radius 6372795 meters.
	// Because Earth is no exact sphere, rounding errors may be up to 0.5%.
	// Courtesy of Maarten Lamers
	double delta = radians(long1-long2);
	double sdlong = sin(delta);
	double cdlong = cos(delta);
	lat1 = radians(lat1);
	lat2 = radians(lat2);
	double slat1 = sin(lat1);
	double clat1 = cos(lat1);
	double slat2 = sin(lat2);
	double clat2 = cos(lat2);
	delta = (clat1 * slat2) - (slat1 * clat2 * cdlong);
	delta = sq(delta);
	delta += sq(clat2 * sdlong);
	delta = sqrt(delta);
	double denom = (slat1 * slat2) + (clat1 * clat2 * cdlong);
	delta = atan2(delta, denom);
	return delta * 6372795;
}

double xSN01::courseTo(double lat1, double long1, double lat2, double long2)
{
	// returns course in degrees (North=0, West=270) from position 1 to position 2,
	// both specified as signed decimal-degrees latitude and longitude.
	// Because Earth is no exact sphere, calculated course may be off by a tiny fraction.
	// Courtesy of Maarten Lamers
	double dlon = radians(long2-long1);
	lat1 = radians(lat1);
	lat2 = radians(lat2);
	double a1 = sin(dlon) * cos(lat2);
	double a2 = sin(lat1) * cos(lat2) * cos(dlon);
	a2 = cos(lat1) * sin(lat2) - a2;
	a2 = atan2(a1, a2);
	if (a2 < 0.0)
	{
		a2 += TWO_PI;
	}
	return degrees(a2);
}

const char *xSN01::cardinal(double course)
{
	static const char* directions[] = {"N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE", "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW"};
	int direction = (int)((course + 11.25f) / 22.5f);
	return directions[direction % 16];
}

void SN01_LOCATION::commit()
{
	rawLatData = rawNewLatData;
	rawLngData = rawNewLngData;
	lastCommitTime = millis();
	valid = updated = true;
}

void SN01_LOCATION::setLatitude(const char *term)
{
   	xSN01::parseDegrees(term, rawNewLatData);
}

void SN01_LOCATION::setLongitude(const char *term)
{	
   	xSN01::parseDegrees(term, rawNewLngData);
}

double SN01_LOCATION::lat()
{
	updated = false;
	double ret = rawLatData.deg + rawLatData.billionths / 1000000000.0;
	return rawLatData.negative ? -ret : ret;
}

double SN01_LOCATION::lng()
{
	updated = false;
	double ret = rawLngData.deg + rawLngData.billionths / 1000000000.0;
	return rawLngData.negative ? -ret : ret;
}

void SN01_DATE::commit()
{
	date = newDate;
	lastCommitTime = millis();
	valid = updated = true;
}

void SN01_TIME::commit()
{
	time = newTime;
	lastCommitTime = millis();
	valid = updated = true;
}

void SN01_TIME::setTime(const char *term)
{
   	newTime = (uint32_t)xSN01::parseDecimal(term);
}

void SN01_DATE::setDate(const char *term)
{
   	newDate = atol(term);
}

uint16_t SN01_DATE::year()
{
	updated = false;
	uint16_t year = date % 100;
	return year + 2000;
}

uint8_t SN01_DATE::month()
{
	updated = false;
	return (date / 100) % 100;
}

uint8_t SN01_DATE::day()
{
	updated = false;
	return date / 10000;
}

uint8_t SN01_TIME::hour()
{
	updated = false;
	return time / 1000000;
}

uint8_t SN01_TIME::minute()
{
	updated = false;
	return (time / 10000) % 100;
}

uint8_t SN01_TIME::second()
{
	updated = false;
	return (time / 100) % 100;
}

uint8_t SN01_TIME::centisecond()
{
	updated = false;
	return time % 100;
}

void SN01_DECIMAL::commit()
{
	val = newval;
	lastCommitTime = millis();
	valid = updated = true;
}

void SN01_DECIMAL::set(const char *term)
{
   	newval = xSN01::parseDecimal(term);
}

void SN01_INTERGER::commit()
{
	val = newval;
	lastCommitTime = millis();
	valid = updated = true;
}

void SN01_INTERGER::set(const char *term)
{
   	newval = atol(term);
}

SN01_CUSTOM::SN01_CUSTOM(xSN01 &gps, const char *_sentenceName, int _termNumber)
{
   	begin(gps, _sentenceName, _termNumber);
}

void SN01_CUSTOM::begin(xSN01 &gps, const char *_sentenceName, int _termNumber)
{
	lastCommitTime = 0;
	updated = valid = false;
	sentenceName = _sentenceName;
	termNumber = _termNumber;
	memset(stagingBuffer, '\0', sizeof(stagingBuffer));
	memset(buffer, '\0', sizeof(buffer));

	// Insert this item into the GPS tree
	gps.insertCustom(this, _sentenceName, _termNumber);
}

void SN01_CUSTOM::commit()
{
	strcpy(this->buffer, this->stagingBuffer);
	lastCommitTime = millis();
	valid = updated = true;
}

void SN01_CUSTOM::set(const char *term)
{
   	strncpy(this->stagingBuffer, term, sizeof(this->stagingBuffer));
}

void xSN01::insertCustom(SN01_CUSTOM *pElt, const char *sentenceName, int termNumber)
{
	SN01_CUSTOM **ppelt;

	for (ppelt = &this->customElts; *ppelt != NULL; ppelt = &(*ppelt)->next)
	{
		int cmp = strcmp(sentenceName, (*ppelt)->sentenceName);
		if (cmp < 0 || (cmp == 0 && termNumber < (*ppelt)->termNumber))
			break;
	}

	pElt->next = *ppelt;
	*ppelt = pElt;
}

String xSN01::getDate(SN01_DATE &d)
{
	char sz[32] = "ERR";
	if (!d.isValid())
	{
		return sz;
	}
	else
	{
		char sz[32];
		sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
		return sz;
	}
}

String xSN01::getTime(SN01_TIME &t)
{
	char sz[32] = "ERR";
	if (!t.isValid())
	{
		return sz;
	}
	else
	{
		char sz[32];
		sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
		return sz;
	}
}

String xSN01::getFloat(float val, bool valid, int len, int prec)
{
	char sz[32] = "ERR";
	if (!valid)
	{
		return sz;
	}
	else
	{
		sprintf(sz, "%f", val);
		return sz;
	}
}

String xSN01::getInt(unsigned long val, bool valid, int len)
{
	char sz[32] = "ERR";
	if (valid)
	{
		sprintf(sz, "%ld", val);
		return sz;
	}
	else
	{
		return sz;
	}
}