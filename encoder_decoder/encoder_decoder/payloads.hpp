#ifndef payloads
#define payloads

#include <stdio.h>
#include <stdint.h>
#include <ctime>
#include <chrono>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <cstring>

#ifdef __linux__
#include <sys/time.h>
#endif

#include <math.h>
#include <map>
#include "CONFIG.h"


enum MSG_ID {
	TIME_SYNC_MSG_ID = 0,        // not yet implemented
	LOCATION_MSG_ID = 1,
	HEADING_MSG_ID = 2,
	SPEED_MSG_ID = 3,
	BRAKES_MSG_ID = 4
};


/** @struct time_stamp
 * @brief timestamp union structure, to be used with every other data struct
 */
struct time_stamp
{
	uint8_t hh  : 5;
	uint8_t mm  : 6;
	uint8_t ss  : 6;
	uint16_t ms : 10;       // max value (1024), suggested maximum (999)

	time_stamp() {
		hh = 0;
		mm = 0;
		ss = 0;
		ms = 0;
	}
#ifdef VERBOSE_RECIEVED_MESSAGES_DECODE
	void print() {
		std::cout << "\t<time>\n";
		std::cout << "\t\thh:\t" << (int)hh << '\n';
		std::cout << "\t\tmm:\t" << (int)mm << '\n';
		std::cout << "\t\tss:\t" << (int)ss << '\n';
		std::cout << "\t\tms:\t" << (int)ms << '\n';
		std::cout << "\t</time>\n";
	}
#endif
};


struct location_payload
{
	uint8_t     id; // id + padding
	// location
	int8_t      lat : 8;	// range shall be (-85 to 85)
	int16_t     lon : 9;    // range shall be (-180 to 180)
	uint16_t    lat_frac;   // max value (65535), suggested maximum (9999)
	uint16_t    lon_frac;   // max value (65535), suggested maximum (9999)
	time_stamp _last_time_stamp;

	//uint8_t time_value[sizeof time_stamp];

	location_payload() {
		id = LOCATION_MSG_ID;
		lat = 0;
		lon = 0;
		lat_frac = 0;
		lon_frac = 0;
	}

#ifdef VERBOSE_RECIEVED_MESSAGES_DECODE
	void print() {
		std::cout << "<location>\n";
		std::cout << "\tid:\t" << (int) id << '\n';
		std::cout << "\tlat:\t" << (int) lat << "." << (int)lat_frac << '\n';
		std::cout << "\tlon:\t" << (int) lon << "." << (int)lon_frac << '\n';
		_last_time_stamp.print();
		std::cout << "</location>\n";
	}
#endif // VERBOSE_RECIEVED_MESSAGES_DECODE

};

struct heading_payload
{
	uint8_t     id = 2;
	uint16_t    heading : 9;
	time_stamp _last_time_stamp;

	//uint8_t time_value[sizeof time_stamp];

	heading_payload() {
		id = HEADING_MSG_ID;
		heading = 0;
	}

#ifdef VERBOSE_RECIEVED_MESSAGES_DECODE
	void print() {
		std::cout << "<heading>\n";
		std::cout << "\tid:\t" << (int)id << '\n';
		std::cout << "\theading:\t" << (int)heading << '\n';
		_last_time_stamp.print();
		std::cout << "</heading>\n";
	}
#endif

};

struct speed_payload
{
	uint8_t  id;
	uint8_t  speed;
	time_stamp _last_time_stamp;

	//uint8_t time_value[sizeof time_stamp];

	speed_payload() {
		id = SPEED_MSG_ID;
		speed = 0;
	}

#ifdef VERBOSE_RECIEVED_MESSAGES_DECODE
	void print() {
		std::cout << "<speed>\n";
		std::cout << "\tid:\t" << (int)id << '\n';
		std::cout << "\tspeed:\t" << (int)speed << '\n';
		_last_time_stamp.print();
		std::cout << "</speed>\n";
	}
#endif
};

struct brakes_payload
{
	uint8_t  id;              // 3 bits
	uint8_t  brakes : 1;           // 1 bits
	//uint8_t time_value[sizeof time_stamp];;     // 27 bits
	time_stamp _last_time_stamp;

	brakes_payload() {
		id = BRAKES_MSG_ID;
		brakes = 0;
	}

#ifdef VERBOSE_RECIEVED_MESSAGES_DECODE
	void print() {
		std::cout << "<brakes>\n";
		std::cout << "\tid:\t" << (int)id << '\n';
		std::cout << "\tbrakes:\t" << (int)brakes << '\n';
		_last_time_stamp.print();
		std::cout << "</brakes>\n";
	}
#endif
};


struct full_payload
{
	location_payload    _location_payload;

	heading_payload     _heading_payload;

	speed_payload       _speed_payload;

	brakes_payload      _brakes_payload;

#ifdef VERBOSE_RECIEVED_MESSAGES_DECODE
	void print() {
		_location_payload.print();
		_heading_payload.print();
		_speed_payload.print();
		_brakes_payload.print();
	}
#endif

};


#endif